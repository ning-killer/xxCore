/*
 * @Author: xiong
 * @Date: 2022/9/2
*/

#ifndef EMX_UpdateInner_HPP
#define EMX_UpdateInner_HPP

#include "EmxCore.hpp"
//ParamServer Msg ID
#define UPDATE_SERVER_MSG_ID 0x00070000

#define EMX_UPDATE_UDP_BUFFER_SIZE (32*1024)
#define EMX_UPDATE_DEFAULT_BLOCK_SIZE (128*1024)
#define EMX_DOMAIN_UPDATE_SERVER "/tmp/UpdateServer"

#define EMX_UPDATE_PACK_SYNC 0x75425315
#define EMX_UPDATE_TIMEOUT (10000)

#define EMX_UPDATE_PRE_SCRIPT "/tmp/update_pre_script.sh"
#define EMX_UPDATE_POST_SCRIPT "/tmp/update_post_script.sh"


namespace Emx {
    //!@brief 升级方式
    enum UpdateWayE {
        Burn_After_Downloading = 0,   //!< 全量下载至内存后再进行烧写 
        Burn_While_Downloading,       //!< 边下载边进行烧写
        Burn_After_DownloadtoFile     //!< 全量下载到文件后再进行烧写
    };
    struct UpdatePackHeader {
        uint32_t sync;
        uint32_t session;
        uint32_t seq;
        UpdateWayE way;
    };
};

#endif //EMX_UpdateInner_HPP