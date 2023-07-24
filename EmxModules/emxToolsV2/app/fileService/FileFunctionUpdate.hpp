/*
* @Author: xiong
* @Date: 2023/2/14
*/

#ifndef EMX_FileFunctionUpdate_HPP
#define EMX_FileFunctionUpdate_HPP

#include "FileFunction.hpp"
#include "UpdateClient.hpp"

namespace Emx {

    class FileFunctionUpdate : public FileFunction {
    public:
        FileFunctionUpdate(FileClient *c);

        virtual ~FileFunctionUpdate();

        void OnGetData(char *data, int size) override;

    private:

        struct OutHeader {
            static const int Size = 1 + 4 + (4 + 4) * 2;
        };
        UpdateClient m_updateHandle;
        UpdateEvent m_updateEvent;
    };
}

#endif //EMX_FileFunctionUpdate_HPP
