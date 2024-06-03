#ifndef EAPIL_EnvInitMsg_HPP
#define EAPIL_EnvInitMsg_HPP

#include "EnvBase.hpp"

namespace Emx {

    class EnvInitMsg : public EnvBase {
    public:
        enum OtaUpdateStatus {
            IDLE	      =	  0,  //空闲态
            DOWNLOADING	  =	  1,  //安装包下载中
            INSTALLING    =   2,  //安装中
            FAILED        =   3,  //升级失败
            DONE          =   4,  //安装成功
            unknown       =   5,  //未知
        };
    public:
        EnvInitMsg() : EnvBase("initMsg") {}

        virtual ~EnvInitMsg() {}

        ErrCodeE Load();

        ErrCodeE Save();
    
    public:
        int reason;
        uint64_t ts;
        std::string message;
        std::string machash;
        int enable_servicschedule;
        std::string servicescheduleurl;
        std::string cafilepath;
        int tz;
        std::string local_storage_path;
        std::string ovd_data_path;
        std::string ovd_log_path;
        OtaUpdateStatus ota_update_status;
        bool is_normal_reboot;
    };

}
#endif //EAPIL_EnvInitMsg_HPP
