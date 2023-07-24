#include "Gat1400Mgr.hpp"

using namespace Emx;

bool Gat1400Mgr::m_netConnect = false;

void Gat1400Mgr::SetNetState(bool netConnect) {
    m_netConnect = netConnect;
}

bool Gat1400Mgr::GetNetState() {
    return m_netConnect;
}

Gat1400Mgr::Gat1400Mgr(Gat1400Util::InitParam *param)
    : m_initParam(param)
    , m_gat1400Req(new Gat1400Req(param))
    , m_isRun(false)
    , m_isRunRet(false)
    , m_isGttRun(false)
    , m_isGttRunRet(false)
    , m_isAliveRun(false)
    , m_isAliveRunRet(false)
    , m_isRegistOk(false)
    , m_isGetgateOk(false)
    , m_isCreated(false)
    , m_reqLastTime(0) {
}

Gat1400Mgr::~Gat1400Mgr() {
    Stop();
    m_runTimer.Destroy();
    m_runWork.Destroy();
    m_ggtWork.Destroy();
    m_aliveWork.Destroy();
    m_isCreated = false;
}

void Gat1400Mgr::Create(EuvLoop *loop) {
    if (!m_isCreated) {
        m_loop = loop;
        m_runWork.Create(m_loop, nullptr
                    , std::bind(&Gat1400Mgr::Run, this, ph_1)
                    , std::bind(&Gat1400Mgr::RunDone, this, ph_1, ph_2));
        m_ggtWork.Create(m_loop, nullptr
                    , std::bind(&Gat1400Mgr::Ggt, this, ph_1)
                    , std::bind(&Gat1400Mgr::GgtDone, this, ph_1, ph_2));
        m_aliveWork.Create(m_loop, nullptr
                    , std::bind(&Gat1400Mgr::Alive, this, ph_1)
                    , std::bind(&Gat1400Mgr::AliveDone, this, ph_1, ph_2));
        m_runTimer.Create(*m_loop);
        if (m_gat1400Req != nullptr) {
            m_gat1400Req->UpdateDeviceId();
            m_gat1400Req->StartSyncCurlClient(m_loop);
        }
        m_isCreated = true;
    }
    Start();
}

void Gat1400Mgr::Start() {
    m_runTimer.Start(0, 1000, [this]() {
        if (!IsSyncTime()) {
            emxlogd("gat1400 run wait for time sync.\n");
            return;
        }
        if (!GetNetState()) {
            emxlogd("gat1400 run wait for net connect.\n");
            return;
        }
        if (!m_runWork.IsWorking()) {
            m_isRun = true;
            m_runWork.Run((void*)&m_isRunRet);
        }
        m_runTimer.Stop();
    });
}

void Gat1400Mgr::Stop() {
    m_runTimer.Stop();
    m_isRun = false;
    m_isGttRun = false;
    m_isAliveRun = false;
}

void Gat1400Mgr::ReStart() {
    Stop();
    while (m_runWork.IsWorking() 
        || m_ggtWork.IsWorking() 
        || m_aliveWork.IsWorking()) {
        // stop 阻塞等待工作线程退出
        usleep(200 * 1000);
    }
    Start();
}

void Gat1400Mgr::Run(void *arg) {
    bool *runRet = (bool*)arg;
    CHECK_GAT1400_PTR2(runRet);
    *runRet = false;
    while (m_isRun) {
        // 尝试注销
        TryUnRegister();

        // 获取1400平台配置信息
        if (TryGetGateWay(m_isRun) != Gat1400Util::GateWayRet::ReqSuccess) {
            emxlogd("gat1400 TryGetGateWay quit.\n");
            continue;
        }

        // 向平台注册
        if (!TryRegster(m_isRun)) {
            emxlogd("gat1400 TryRegster quit.\n");
            continue;
        }
        
        // 检验时间(暂未作用)
        if (!TryGetTime()) {
            emxlogd("gat1400 run wait for TryGetTime.\n");
            continue;
        }

        *runRet = true;
        break;
    }
}

void Gat1400Mgr::RunDone(Emx::ErrCodeE e, void *arg) {
    bool* runRet = (bool*)arg;
    CHECK_GAT1400_PTR2(runRet);
    emxlogd("gat1400 rundone ret: %d\n", *runRet);
    if (*runRet) {
        // 开启定时平台信息获取工作线程
        if (!m_ggtWork.IsWorking()) {
            m_isGttRun = true;
            m_ggtWork.Run(&m_isGttRunRet);
        }
        // 保活工作线程
        if (!m_aliveWork.IsWorking()) {
            m_isAliveRun = true;
            m_aliveWork.Run(&m_isAliveRunRet);
        }
    }
    m_isRun = false;
}

void Gat1400Mgr::Ggt(void *arg) {
    emxlogd("gat1400 ggt work is start.\n");
    bool* GgtRet = (bool*)arg;
    CHECK_GAT1400_PTR2(GgtRet);
    *GgtRet = false;
    Gat1400Util::GateWayRet ret = Gat1400Util::GateWayRet::GateWayError;
    bool firstRun = true;
    int sleepTime = 0, normalTime = 10 * 60, errorTime = 5;
    
    while (m_isGttRun) {
        if (!firstRun) {
            ret = TryGetGateWay(m_isGttRun);
            if (ret == Gat1400Util::GateWayRet::AddrChanged) {
                emxlogd("gat1400 gate way is changed, restart task\n");
                m_isRegistOk = false;
                *GgtRet = true;
                break;
            } else if (ret == Gat1400Util::GateWayRet::ReqSuccess) {
                sleepTime = m_gateParam.cfgInterval > 0 ? m_gateParam.cfgInterval : normalTime;
            } else {
                sleepTime = errorTime;
            }
        } else {
            firstRun = false;
            sleepTime = m_gateParam.cfgInterval > 0 ? m_gateParam.cfgInterval : normalTime;
        }
        emxlogd("ggt work, sleepTime(%ds) to retry.\n", sleepTime);
        int count = (sleepTime * 1000) / 500; //500ms sleep一次
        while (m_isGttRun && (count > 0)) {
            usleep(500 * 1000);
            count--;
        }
    }
    //note: 退出场景说明
    //1.业务中断退出。2.地址变更，重置初始化。
}

void Gat1400Mgr::GgtDone(Emx::ErrCodeE e, void *arg) {
    emxlogd("gat1400 ggt work done.\n");
    bool* GgtRet = (bool*)arg;
    CHECK_GAT1400_PTR2(GgtRet);
    if (!*GgtRet) {
        // 正常控制退出
        return;
    }
    ReStart();
}

void Gat1400Mgr::Alive(void *arg) {
    emxlogd("gat1400 alive work is start.\n");
    bool* AliveRet = (bool*)arg;
    CHECK_GAT1400_PTR2(AliveRet);
    *AliveRet = false;
    int ret = 0;
    int currentTime = Time::GetS(), intervalTime = 0, sleepTime = 0;

	//注册成功后，在1400配置获取接口中的keepAliveInterval字段时长内未交互信息则进行心跳保活。
	//90s内，有请求过/VIID/Faces这个接口，那就不需要保活，如果90s内没请求过任何接口就要保活，但是超过90s了，就需要注册了
    while (m_isAliveRun) {
        //根据配置获取的时间进行延时
        sleepTime = m_gateParam.keepAliveInterval - 10;
        int count = (sleepTime * 1000) / 500; //500ms sleep一次
        do {
            usleep(500 * 1000);
            count--;
        } while (count > 0 && m_isAliveRun);

        //当前时间减去请求最后的成功请求时间大于规定的保活时间，则需要保活
        currentTime = Time::GetS();
        intervalTime = currentTime - m_reqLastTime;
        emxlogd("intervalTime: %d; gat1400 keepAlive interval: %d\n", intervalTime, m_gateParam.keepAliveInterval);

        //0 < ts < 90; 保活请求(协议文档写的是在间隔时间内有其他请求也不需要请求保活，但是测试效果不好；所以这里到时间就请求心跳)
        if (intervalTime < m_gateParam.keepAliveInterval) {
            for (int i = 0; i < m_gateParam.retry; i++) {
                ret = m_gat1400Req->KeepAlive();
                if (ret == 0 || !m_isAliveRun) {
                    m_reqLastTime = Time::GetS();
                    break;
                } else if (ret == 100 || ret == 101 || ret == 401) {
                    m_isRegistOk = false;
                    m_isAliveRun = false;
                    *AliveRet = true;
                    break;
                }
                usleep(500 * 1000);
            }
        } else {
            m_isRegistOk = false;
            m_isAliveRun = false;
            *AliveRet = true;
        }
    }
    //note: 退出场景说明
    //1.业务中断退出。2.保活异常。
}

void Gat1400Mgr::AliveDone(Emx::ErrCodeE e, void *arg) {
    emxlogd("gat1400 ggt work done.\n");
    bool* AliveRet = (bool*)arg;
    CHECK_GAT1400_PTR2(AliveRet);
    if (!*AliveRet) {
        // 正常控制退出
        return;
    }
    ReStart();
}

bool Gat1400Mgr::TryUnRegister() {
    CHECK_GAT1400_PTR(m_gat1400Req, false);
	if (m_isRegistOk) {
        m_reqLastTime = Time::GetS();
        if (!m_gat1400Req->UnRegister()) {
            emxloge("gat1400req unregister failed.\n");
            return false;
        }
        m_isRegistOk = false;
	}
	return true;
}

Gat1400Util::GateWayRet Gat1400Mgr::TryGetGateWay(bool &isRun) {
    Gat1400Util::GateWayRet ret = Gat1400Util::GateWayRet::GateWayError;
	Gat1400Util::GateParam gate_param;
    int failedCount = 0, sleepTime = 30;

    while (isRun) {
        if (m_gat1400Req->GetGateWay(gate_param)) {
            ret = Gat1400Util::GateWayRet::ReqSuccess;
            emxlogd("gat1400 get gateway succ.\n");
            if (m_isGetgateOk) {
                if (gate_param.host1400 != m_gateParam.host1400) {
                    emxlogd("gat1400 gate way is changed.\n");
                    ret = Gat1400Util::GateWayRet::AddrChanged;
                }
            }
            m_gateParam = gate_param;
            break;
        } else {
            failedCount++;
        }

        // 细化重连时间
        if (failedCount <= 5) {
            sleepTime = 5;
        } else if (failedCount > 5 && failedCount <= 10) {
            sleepTime = 10;
        } else if (failedCount > 10) {
            sleepTime = 30;
        }
        emxlogd("gat1400 get gateway failed, failedCount(%d), sleepTime(%ds) to retry.\n", failedCount, sleepTime);
        int count = (sleepTime * 1000) / 500; //500ms sleep一次
        while (isRun && (count > 0)) {
            usleep(500 * 1000);
            count--;
        }
    }
    //note: 退出场景说明
    //1.业务中断退出。2.成功请求退出
    if (ret == Gat1400Util::GateWayRet::ReqSuccess) {
        m_isGetgateOk = true;
    }
    return ret;
}

bool Gat1400Mgr::TryRegster(bool &isRun) {
    if (m_isRegistOk) {
        emxlogd("gat1400 is Registeed.\n");
        return true;
    }
    Gat1400Util::RegisterAuthParam regauth_param;
    Gat1400Util::RegisterRet register_ret = Gat1400Util::RegisterRet::RegisterError;
    int failedCount = 0, sleepTime = 30;

    while(isRun) {
        register_ret = m_gat1400Req->Register(regauth_param);
        if (register_ret == Gat1400Util::RegisterRet::Authorized) {
            emxlogd("gat1400 regster succ.\n");
            m_isRegistOk = true;
            m_reqLastTime = Time::GetS();
            break;
        } else if (register_ret == Gat1400Util::RegisterRet::UnAuthorized) {
            continue;
        } else {
            failedCount++;
        }

        // 细化重连时间
        if (failedCount <= 5) {
            sleepTime = 5;
        } else if (failedCount > 5 && failedCount <= 10) {
            sleepTime = 10;
        } else if (failedCount > 10) {
            sleepTime = 30;
        }
        emxlogd("gat1400 try regster failed, failedCount(%d), sleepTime(%ds) to retry.\n", failedCount, sleepTime);
        int count = (sleepTime * 1000) / 500; //500ms sleep一次
        while (isRun && (count > 0)) {
            usleep(500 * 1000);
            count--;
        }
    }
    //note: 退出场景说明
    //1.业务中断退出。2.完成注册请求退出
    return register_ret == Gat1400Util::RegisterRet::Authorized ? true : false;
}

bool Gat1400Mgr::TryGetTime() {
    if (m_gat1400Req->GetTime()) {
        m_reqLastTime = Time::GetS();
        return true;
    }
    return false;
}

bool Gat1400Mgr::Upload(const Gat1400Util::UploadDataParam &upload_param) {
    if (m_gat1400Req == nullptr) {
        return false;
    }
    bool ret = true;
    switch (upload_param.type) {
    case Gat1400Util::UploadType::Faces:
        m_gat1400Req->UpLoadFace(upload_param);
        break;
    default:
        emxloge("gat1400 unsupport type(%d) upload!\n", upload_param.type);
        ret = false;
        break;
    }
    return ret;
}

bool Gat1400Mgr::IsSyncTime() {
    int currentTime = Time::GetS();
    int compareTime = 1642003200; ///2022-01-13 00:00:00
    if (currentTime < compareTime) {
        return false;
    } else {
        return true;
    }
}