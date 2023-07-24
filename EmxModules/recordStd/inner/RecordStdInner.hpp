/*
* @Author: xiong
* @Date: 2023/2/7
*/

#ifndef EMX_RecordStdInner_HPP
#define EMX_RecordStdInner_HPP

#define RECORD_STD_MSG_ID 0x000C0000


namespace Emx {

    enum class RecordStdModuleE : uint8_t {
        RecordStd,
        RecordStdRecord,
        RecordStdSnap,
    };

    enum class RecordStdMethodE : uint8_t {
        GetParam,
        SetParam,
    };

    enum class RecordStdRecordMethodE : uint8_t {
        SetManualRecord,
        GetManualRecord,
        GetRecordList,
    };

    enum class RecordStdSnapMethodE : uint8_t {
        TriggerManualSnap,
        GetSnapList,
    };

}
#endif //EMX_RecordStdInner_HPP
