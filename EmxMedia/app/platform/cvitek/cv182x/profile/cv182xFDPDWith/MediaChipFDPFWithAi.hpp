//
// Created by cx on 2023/6/27.
//

#ifndef EMX_MediaChipFDPDWithAi_HPP
#define EMX_MediaChipFDPDWithAi_HPP


#include "MediaChipAi.hpp"

namespace Emx {
    class MediaChipFDPDWithAi : public MediaChipAi {
    public:
        MediaChipFDPDWithAi(int32_t chn, MediaServerResource &res) : MediaChipAi(chn, res) {}

    protected:
        void AddAiPlug() override;

        ErrCodeE SetParam(const Param &param) override;

    private:
        int SetFaceAlgorithmParam(const Param &param);
    };
}

#endif //EMX_MediaChipFDPDWithAi_HPP
