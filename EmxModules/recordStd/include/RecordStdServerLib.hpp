/*
* @Author: xiong
* @Date: 2023/6/1
*/

#ifndef EMX_RecordStdServerLib_HPP
#define EMX_RecordStdServerLib_HPP

namespace Emx {

    class RecordStdServer;

    class RecordStdServerLib {
    public:
        RecordStdServerLib();

        virtual ~RecordStdServerLib();

        void Create(bool newThread);

        void Destroy();

    private:
        RecordStdServer *m_server;
    };

}

#endif //EMX_RecordStdServerLib_HPP
