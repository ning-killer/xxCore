//
// Created by xiong on 2021/5/28.
//

#ifndef EMX_MediaClientSnap_HPP
#define EMX_MediaClientSnap_HPP

#include "MediaSnap.hpp"

namespace Emx {

    //!@brief 缩略图触发
    class MediaClientSnap {
    public:

        /**
         * @brief 触发缩略图编码，调用此接口后，媒体服务会编码一帧缩略图，
         *        并通过MediaFrame(ringbuffer)广播出去.
         *        注意调用此接口前需要保证已经在监听MediaFrame
         * @param chn [in] 通道号
         * @param timeoutMs [in] 超时时间
         * @return
         */
        static ErrCodeE Trigger(int chn, int32_t timeoutMs);
    };
}
#endif //EMX_MediaClientSnap_HPP

