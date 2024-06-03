/*
* @Author: xiong
* @Date: 2023/6/1
*/

#ifndef EMX_NtpServerLib_HPP
#define EMX_NtpServerLib_HPP

namespace Emx {

    class NtpServer;

    class NtpServerLib {
    public:
        NtpServerLib();

        virtual ~NtpServerLib();

        using OnCreated = std::function<void()>;

        void Create(OnCreated cb, bool newThread);

        void Destroy();

    private:
        NtpServer *m_server;
    };

}// namespace Emx

#endif//EMX_NtpServerLib_HPP
