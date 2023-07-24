//
// Created by xiong on 2021/8/5.
//

#ifndef EMX_MediaClientServer_HPP
#define EMX_MediaClientServer_HPP

#include "core/EmxTypeDef.hpp"

namespace Emx {

    //!@brief 控制媒体服务整体的启停
    class MediaClientServer {
    public:
        /**
         * @brief 创建媒体
         * @return ErrCodeE
         */
        static ErrCodeE CreateMedia();

        /**
         * @brief 停止媒体
         * @return ErrCodeE
         */
        static ErrCodeE DestroyMedia();
    };

}

#endif //EMX_MediaClientServer_HPP
