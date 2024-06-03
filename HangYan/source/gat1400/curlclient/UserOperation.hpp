/**
 * @file UserOperation.hpp
 * @author hening
 * @brief 用户操作类，提供curl异步上传用户自定义控制
 * @date 2023-7-3
 */

#ifndef EMX_USEROPERATION_HPP
#define EMX_USEROPERATION_HPP

#include "EmxCore.hpp"
#include "Gat1400Util.hpp"
#include "curl.h"

namespace Emx {
    //!@brief HTTP上传事件
    enum HttpPostEvent : uint8_t {
        gat1400Faces = 0,          //!< 1400协议人脸
        gat1400Persons,            //!< 1400协议人形
        gat1400MotorVehicles,      //!< 1400协议机动车
        gat1400NonMotorVehicles,   //!< 1400协议非机动车
        gat1400Kitchen,            //!< 1400协议明厨亮灶
        gat1400Falling,            //!< 1400协议高空抛物批量
        gat1400Region,             //!< 1400协议区域人数统计批量
        gat1400Traffic,            //!< 1400协议倾斜客流
        gat1400OnLeave,            //!< 1400协议离岗检测
    };  
    //!@brief 用户操作类
    class UserOperation {
      public:
        UserOperation(HttpPostEvent postEvent) : 
          m_postEvent(postEvent), m_initParam(nullptr), m_authParam(nullptr) { }
        virtual ~UserOperation() { }
        virtual bool isUploadOk(const std::string &result) = 0;

        std::string PackAuthHeader();
        bool UpdateAuthHeader(struct curl_slist *header);
        void UpdateParams(Gat1400Util::InitParam *init_param, Gat1400Util::RegisterAuthParam *auth_param) {
          m_initParam = init_param;
          m_authParam = auth_param;
        }
        HttpPostEvent m_postEvent;

      protected:
        Gat1400Util::InitParam *m_initParam;
        Gat1400Util::RegisterAuthParam *m_authParam;
    };

    class Gat1400FaceUploadUserOpera : public UserOperation {
      public:
        Gat1400FaceUploadUserOpera(): UserOperation(gat1400Faces) {}
        bool isUploadOk(const std::string &result) override;
    };

    class Gat1400MotorVehiclesUploadUserOpera : public UserOperation {
      public:
        Gat1400MotorVehiclesUploadUserOpera(): UserOperation(gat1400MotorVehicles) {}
        bool isUploadOk(const std::string &result) override;
    };

    class Gat1400NonMotorVehiclesUploadUserOpera : public UserOperation {
      public:
        Gat1400NonMotorVehiclesUploadUserOpera(): UserOperation(gat1400NonMotorVehicles) {}
        bool isUploadOk(const std::string &result) override;
    };

    class Gat1400TrafficUploadUserOpera : public UserOperation {
      public:
        Gat1400TrafficUploadUserOpera(): UserOperation(gat1400Traffic) {}
        bool isUploadOk(const std::string &result) override;
    };

    class Gat1400RegionUploadUserOpera : public UserOperation {
      public:
        Gat1400RegionUploadUserOpera(): UserOperation(gat1400Region) {}
        bool isUploadOk(const std::string &result) override;
    };

    class Gat1400OnLeaveUploadUserOpera : public UserOperation {
      public:
        Gat1400OnLeaveUploadUserOpera(): UserOperation(gat1400OnLeave) {}
        bool isUploadOk(const std::string &result) override;
    };
}

#endif //EMX_USEROPERATION_HPP
