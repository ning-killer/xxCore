/*
* @Author: xiong
* @Date: 2023/2/7
*/

#ifndef EMX_RecordStdServer_HPP
#define EMX_RecordStdServer_HPP

#include "RecordStd.hpp"
#include "RecordStdResource.hpp"
#include "SDCardClient.hpp"
#include "RecordStdRecordFile.hpp"
#include "RecordStdRecord.hpp"
#include "RecordStdSnap.hpp"

namespace Emx {

    class RecordStdServer {
    public:
        RecordStdServer() {}

        virtual ~RecordStdServer() {}

        void Create(bool newThread);

        void Destroy();

    private:
        ErrCodeE LoadParam();

        ErrCodeE SaveParam();

        ErrCodeE OnRecvReq(uint32_t module, uint32_t method,
                           const char *data, int32_t size, std::string &out);

        void OnSDCardInfoEvent(int chn, SDCard::Info &info);

    private:
        RecordStdResource m_res;
        MsgRpcRep m_resp;
        RecordStdRecordFile m_file;
        struct SDCardStat {
            bool normal;
            SDCardClientEvent *event;
        } m_sdcard;
        RecordStdRecord m_record;
        RecordStdSnap m_snap;
    };

}

#endif //EMX_RecordStdServer_HPP
