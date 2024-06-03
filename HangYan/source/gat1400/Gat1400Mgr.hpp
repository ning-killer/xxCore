/**
 * @file Gat1400Mgr.hpp
 * @author hening
 * @brief Gat1400协议管理类
 * @date 2023-6-27
 */

#ifndef EMX_GAT1400MGR_HPP
#define EMX_GAT1400MGR_HPP

#include "EmxCore.hpp"
#include "Gat1400Util.hpp"
#include "Gat1400Req.hpp"

namespace Emx {
    class Gat1400Mgr {
      public:
        Gat1400Mgr(Gat1400Util::InitParam *param);
        ~Gat1400Mgr();
        void Create(EuvLoop *loop);
        void Start();
        void Stop();
        void ReStart();
        bool Upload(const Gat1400Util::UploadDataParam *upload_param);
        static void SetNetState(bool netConnect);
        static bool GetNetState();
        bool IsUploadSceneImg() {
            return m_gateParam.postSceneImg;
        }

      private:
        void Run(void *arg);
        void RunDone(Emx::ErrCodeE e, void *arg);
        void Ggt(void *arg);
        void GgtDone(Emx::ErrCodeE e, void *arg);
        void Alive(void *arg);
        void AliveDone(Emx::ErrCodeE e, void *arg);
        bool TryUnRegister();
        bool TryRegster(bool &isRun);
        bool TryGetTime();
        Gat1400Util::GateWayRet TryGetGateWay(bool &isRun);
        bool IsSyncTime();

      private:
        static Gat1400Mgr *m_instance;
        static bool m_netConnect;
        Gat1400Util::InitParam *m_initParam;
        Gat1400Req *m_gat1400Req;
        Gat1400Util::GateParam m_gateParam;
        EuvLoop *m_loop;
        EuvWork m_runWork;   //启动入口工作线程
        EuvWork m_ggtWork;   //定时平台信息工作线程
        EuvWork m_aliveWork; //保活工作线程
        EuvTimer m_runTimer;
        bool m_isRun;
        bool m_isRunRet;
        bool m_isGttRun;
        bool m_isGttRunRet;
        bool m_isAliveRun;
        bool m_isAliveRunRet;
        bool m_isRegistOk;
        bool m_isGetgateOk;
        bool m_isCreated;
        int m_reqLastTime;
    };
};

#endif // EMX_GAT1400MGR_HPP
