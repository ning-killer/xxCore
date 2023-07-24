/*
* @Author: xiong
* @Date: 2023/6/1
*/

#ifndef EMX_NightVisionServerLib_HPP
#define EMX_NightVisionServerLib_HPP

namespace Emx {

    class NightVisionServerBase;

    class NightVisionServerLib {
    public:
        NightVisionServerLib();

        virtual ~NightVisionServerLib();

        using OnCreated = std::function<void()>;

        void Create(OnCreated cb, bool newThread);

        void Destroy();

    private:
        NightVisionServerBase *m_server;
    };

}

#endif //EMX_NightVisionServerLib_HPP
