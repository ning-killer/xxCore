/*
* @Author: xiong
* @Date: 2023/2/7
*/

#ifndef EMX_RecordStdClient_HPP
#define EMX_RecordStdClient_HPP

#include "RecordStd.hpp"

namespace Emx {

    class RecordStdClient : public RecordStd {
    public:
        ErrCodeE GetParam(std::shared_ptr<Param> &param) override;

        ErrCodeE SetParam(const std::shared_ptr<Param> &param) override;

        ErrCodeE GetManualRecordStat(bool &ena) override;

        ErrCodeE SetManualRecordStat(bool ena) override;

        ErrCodeE GetRecordList(uint32_t startUtc, uint32_t endUtc, bool merge, std::vector<RecFile> &list) override;

        ErrCodeE ManualSnap() override;

        ErrCodeE GetSnapList(uint32_t startUtc, uint32_t endUtc, std::vector<RecFile> &list) override;
    };

}

#endif //EMX_RecordStdClient_HPP
