/*
* @Author: xiong
* @Date: 2023/6/2
*/

#ifndef EMX_SDCardUpdateLib_HPP
#define EMX_SDCardUpdateLib_HPP

namespace Emx {

    class SDCardUpdate;

    class SDCardUpdateLib {
    public:
        SDCardUpdateLib();

        virtual ~SDCardUpdateLib();

        void Create(bool newThread);

        void Destroy();

    private:
        SDCardUpdate *m_update;
    };
}

#endif //EMX_SDCardUpdateLib_HPP
