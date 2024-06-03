/**
 * @file CurlAsyncClient.hpp
 * @author hening
 * @brief curl单线程异步Post客户端
 * @date 2023-6-1
 */

#ifndef EMX_CURL_ASYNC_CLIENT_HPP
#define EMX_CURL_ASYNC_CLIENT_HPP

#include "CurlClient.hpp"

namespace Emx {

    //!@brief Curl异步发送客户端
    class CurlAsyncClient : public CurlClient {
      public:
        CurlAsyncClient(int max_size = 3);
        ~CurlAsyncClient();

      public:
        /**
         * @brief 创建客户端
         * @param loop [in] curl客户端工作线程池绑定loop
         * @return bool
         */
        bool Create(EuvLoop *loop);

        /**
         * @brief 启动客户端
         * @return bool
         */
        bool Start();

        /**
         * @brief 停下客户端
         * @note 只停下send动作(当次post的动作暂未设计中断)，数据链表未清空
         * @return bool
         */
        bool Stop();

        bool Post(const PostData *data) override;
    
      private:
        void PostMsg(void *arg);
        void PostMsgDone(Emx::ErrCodeE e, void *arg);
        void RunPostWork();
        bool AddMsg(const PostData *data);
        PostData* GetMsg();
        PostData* Clone(const PostData *data);
        int GetTimerInterval();
        void OnMsgSync();
        
      private:
        EuvLoop *m_loop;
        EuvWork m_postWork;
        EuvTimer m_postTimer;
        EuvAsync m_msgSyncInfo;
        int m_maxSize;
        bool m_isStarted;
        bool m_isCreated;
        std::list<PostData*> m_msgList;
        std::mutex m_msgListMtx;
        int m_failedCount;
    };
}

#endif //EMX_CURL_ASYNC_CLIENT_HPP
