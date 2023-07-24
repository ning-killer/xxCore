//
// Created by xiong on 2021/7/19.
//

#ifndef EMX_Core_HPP
#define EMX_Core_HPP
namespace Emx {
    class LogServer;

    class MsgRpcServer;

    class MsgTopicServer;

    class ParamServer;

    class Core {
    public:
        Core() : m_log(nullptr), m_rpc(nullptr), m_topic(nullptr), m_param(nullptr), m_created(false) {}

        void Create(const char *dir);

        void Destroy();

    protected:
        LogServer *m_log;
        MsgRpcServer *m_rpc;
        MsgTopicServer *m_topic;
        ParamServer *m_param;
        bool m_created;
    };
}

#endif //EMX_Core_HPP
