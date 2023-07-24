/*
* @Author: xiong
* @Date: 2023/6/2
*/

#ifndef EMX_UpdateServerLib_HPP
#define EMX_UpdateServerLib_HPP

namespace Emx {

    class UpdateServer;

    class UpdateServerLib {
    public:
        UpdateServerLib();

        virtual ~UpdateServerLib();

        void Create(bool newThread);

        void Destroy();

    private:
        UpdateServer *m_server;
    };
}

#endif //EMX_UpdateServerLib_HPP
