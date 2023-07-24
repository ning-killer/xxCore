/*
* @Author: xiong
* @Date: 2023/6/1
*/

#ifndef EMX_EmxToolsV2Lib_HPP
#define EMX_EmxToolsV2Lib_HPP

namespace Emx {
    class EmxToolsV2;

    class EmxToolsV2Lib {
    public:
        EmxToolsV2Lib();

        virtual ~EmxToolsV2Lib();

        void Create(bool newThread);

        void Destroy();

    private:
        EmxToolsV2 *m_tools;
    };
};

#endif //EMX_EmxToolsV2Lib_HPP
