/*
* @Author: xiong
* @Date: 2023/6/1
*/

#ifndef EMX_OnvifServiceLib_HPP
#define EMX_OnvifServiceLib_HPP

namespace Emx {

    class EmxOnvifService;

    class OnvifServiceLib {
    public:
        OnvifServiceLib();

        virtual ~OnvifServiceLib();

        void Create(bool newThread);

        void Destroy();

    private:
        EmxOnvifService *m_service;
    };
}

#endif //EMX_OnvifServiceLib_HPP
