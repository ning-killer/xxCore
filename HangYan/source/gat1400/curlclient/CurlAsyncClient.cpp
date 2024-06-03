#include "CurlAsyncClient.hpp"

using namespace Emx;

CurlAsyncClient::CurlAsyncClient(int max_size) 
    : m_maxSize(max_size)
    , m_isStarted(false)
    , m_isCreated(false)
    , m_failedCount(0) { 
}

CurlAsyncClient::~CurlAsyncClient() {
    Stop();
    m_postTimer.Destroy();
    m_postWork.Destroy();
    m_isCreated = false;
    std::lock_guard<std::mutex> locker(m_msgListMtx);
    for(auto &msg : m_msgList) {
        if (msg != nullptr) {
            FreePostData(msg);
        }
    }
    m_msgList.clear();
}

bool CurlAsyncClient::Create(EuvLoop *loop) {
    if (loop == nullptr) {
        return false;
    }
    if (m_isCreated) {
        return true;
    }
    m_postWork.Create(loop, nullptr
                , std::bind(&CurlAsyncClient::PostMsg, this, ph_1)
                , std::bind(&CurlAsyncClient::PostMsgDone, this, ph_1, ph_2));
    m_msgSyncInfo.Create(*loop, std::bind(&CurlAsyncClient::OnMsgSync, this));
    m_postTimer.Create(*loop);
    m_isCreated = true;
    Start();
    return true;
}

bool CurlAsyncClient::Start() {
    if (m_isStarted || m_postWork.IsWorking()) {
        return true;
    }
    m_isStarted = true;
    m_postTimer.Start(0, 0, [this]() {
        if (!m_postWork.IsWorking()) {
            RunPostWork();
        }
    });
    return true;
}

bool CurlAsyncClient::Stop() {
    if (!m_isStarted) {
        return true;
    }
    m_isStarted = false;
    return false;
}

bool CurlAsyncClient::Post(const PostData *data) {
    if (!AddMsg(data)) {
        return false;
    }
    m_msgSyncInfo.Send();
    return true;
}

void CurlAsyncClient::OnMsgSync() {
    if (!m_postWork.IsWorking()) {
        RunPostWork();
    }
}

bool CurlAsyncClient::AddMsg(const PostData *data) {
    bool ret = false;
    do {
        if (data == nullptr) {
            break;
        }
        {
            std::lock_guard<std::mutex> locker(m_msgListMtx);
            if ((int)(m_msgList.size() + 1) > m_maxSize) {
                emxlogd("curl client more than max size(%d)\n", m_maxSize);
                return false; 
            }
            PostData *cloned = Clone(data);
            if (cloned != nullptr) {
                m_msgList.push_back(cloned);
            }
        }
        ret = true;
    } while(false);
    return ret;
}

//深拷贝取头节点数据
CurlAsyncClient::PostData* CurlAsyncClient::GetMsg() {
    PostData *data = nullptr;
    do {
        std::lock_guard<std::mutex> locker(m_msgListMtx);
        if ((int)m_msgList.size() <= 0) {
            break;
        }
        data = m_msgList.front();
        emxlogt("data header[%p]\n", data->header);
    } while(false);
    return data;
}

CurlAsyncClient::PostData* CurlAsyncClient::Clone(const PostData *data) {
    PostData* cloned = new PostData();
    do {
        if (data == nullptr || cloned == nullptr) {
            break;
        }
        cloned->url = data->url;
        cloned->param = data->param;
        cloned->response = data->response;
        cloned->responseHead = data->responseHead;
        cloned->code = data->code;
        cloned->timeout = data->timeout;
        cloned->isdebug = data->isdebug;
        cloned->way = data->way;
        cloned->proxy = data->proxy;
        cloned->cbWriteData = data->cbWriteData;
        cloned->cbWriteDebug = data->cbWriteDebug;
        cloned->cbWriteHeader = data->cbWriteHeader;
        cloned->userOperation = data->userOperation;
        // curl7.9.3版本开始提供curl_slist_dup()深拷贝curl_slist
        // clone->header = curl_slist_dup(data->header); 
        struct curl_slist *tmp = data->header;
        while (tmp) {
            cloned->header = curl_slist_append(cloned->header, tmp->data);
            tmp = tmp->next;
        }
    } while(false);
    return cloned;
}

void CurlAsyncClient::RunPostWork() {
    if (m_postWork.IsWorking()) {
        return;
    }
    if (!m_isStarted) {
        emxlogd("CurlAsyncClient is stop!\n");
        return;
    }
    PostData *data = GetMsg();
    if (data != nullptr) {
        m_postWork.Run((void*)data);
    }
}

void CurlAsyncClient::PostMsg(void *arg) {
    PostData* msg = (PostData*)arg;
    if (arg == nullptr) {
        emxloge("post msg is null\n");
        return;
    }
    //note: 重置应答
    msg->response.clear();
    msg->response.shrink_to_fit();
    msg->responseHead.clear();
    msg->responseHead.shrink_to_fit();
    Send(msg, false);
}

void CurlAsyncClient::PostMsgDone(Emx::ErrCodeE e, void *arg) {
    emxlogd("curl syncclient post msg done\n");
    PostData* msg = (PostData*)arg;
    if (msg == nullptr) {
        emxloge("post msg is null\n");
        return;
    }
    emxlogd("result:%s; code:%d; response: %s; postevent: %d\n"
                , msg->code == CURLE_OK ? "success" : "failed"
                , msg->code, msg->response.c_str()
                , (int)msg->userOperation->m_postEvent);
    bool isPosted = false;
    if (msg->userOperation != nullptr 
        && msg->userOperation->isUploadOk(msg->response) 
        && msg->code == CURLE_OK) {
        isPosted = true;
    }
    if (isPosted) {
        emxlogd("event[%d] upload is ok!\n", (int)msg->userOperation->m_postEvent);
        m_failedCount = 0;
        //note: 判断上传成功时，链表头节点资源释放
        {   
            std::lock_guard<std::mutex> locker(m_msgListMtx);
            FreePostData(m_msgList.front());
            m_msgList.pop_front();
        }
    } else {
        //note: 更新鉴权头信息
        // msg->userOperation->UpdateAuthHeader(msg->header);
        m_failedCount++;
        emxlogt("m_failedCount[%d]\n", m_failedCount);
        emxlogt("msg header[%p]\n", msg->header);
    }

    if (!m_msgList.empty()) {
        m_postTimer.Start(GetTimerInterval(), 0, [this]() {
            if (!m_postWork.IsWorking()) {
                RunPostWork();
            }
        });
    }
}

int CurlAsyncClient::GetTimerInterval() {
    int interval = 1000; //ms
    if (m_failedCount <= 5) {
        interval = 1000;
    } else if (m_failedCount > 5 && m_failedCount <= 10) {
        interval = 5000;
    } else if (m_failedCount > 10 && m_failedCount <= 20) {
        interval = 10000;
    } else {
        interval = 30000;
    }
    return interval;
}