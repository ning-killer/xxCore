/*
* @Author: xiong
* @Date: 2023/6/1
*/

#ifndef EMX_NtpClientLib_HPP
#define EMX_NtpClientLib_HPP

namespace Emx {
    class NtpClient;

    class NtpClientLib {
    public:
        NtpClientLib();

        virtual ~NtpClientLib();

        void Create(bool newThread);

        void Destroy();

    private:
        NtpClient *m_client;
    };
}

#endif //EMX_NtpClientLib_HPP
