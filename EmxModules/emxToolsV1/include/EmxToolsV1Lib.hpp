/*
* @Author: xiong
* @Date: 2023/6/1
*/

#ifndef EMX_EmxToolsV1Lib_HPP
#define EMX_EmxToolsV1Lib_HPP

namespace Emx {
    class EmxToolsV1;

    class EmxToolsV1Lib {
    public:
        EmxToolsV1Lib();

        virtual ~EmxToolsV1Lib();

        void Create(bool newThread);

        void Destroy();

    private:
        EmxToolsV1 *m_tools;
    };
};

#endif //EMX_EmxToolsV1Lib_HPP
