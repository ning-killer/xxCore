/**
 * @file Gat1400Req.hpp
 * @author hening
 * @brief Gat1400协议请求类
 * @date 2023-6-27
 */

#ifndef EMX_GAT1400REQ_HPP
#define EMX_GAT1400REQ_HPP

#include "EmxCore.hpp"
#include "EnvAlarmFace.hpp"
#include "Gat1400Util.hpp"
#include "CurlSyncClient.hpp"
#include "CurlAsyncClient.hpp"
#include "UserOperation.hpp"

namespace Emx {

    class Gat1400Req {
      public:
        Gat1400Req(Gat1400Util::InitParam *param);
        ~Gat1400Req();

        void UpdateDeviceId();
        bool StartSyncCurlClient(EuvLoop *loop);
        bool StopSyncCurlClient();
        Gat1400Util::RegisterRet Register(Gat1400Util::RegisterAuthParam &auth_param);
        bool UnRegister();
        int KeepAlive();
        bool GetGateWay(Gat1400Util::GateParam &gate_param);
        bool GetTime();
        bool UpLoadFace(const Gat1400Util::UploadDataParam &upload_param);
        
      private:
        std::string PackAuthHeader(const Gat1400Util::RegisterAuthParam &param);
        bool PackUploadFacesParam(const Gat1400Util::UploadDataParam &upload_param, std::string &param);
        bool ParseRegisterRplyHeader(const std::string &result, std::map<std::string, std::string> &dst);
        std::string &TrimLeft(std::string &str);
        std::string &TrimRight(std::string &str);
        std::string &Trim(std::string &str);
        std::string GetStringValueByKey(const std::string &key, const std::string &start_end, const std::string &src);

      private:
        static size_t OnUnRegisterWriteData(void* buffer
            , size_t size, size_t nmemb, void* user);
        
        static size_t OnKeepAliveWriteData(void* buffer
            , size_t size, size_t nmemb, void* user);
        
        static size_t OnRegisterWriteData(void* buffer
            , size_t size, size_t nmemb, void* user);

        static size_t OnGetGateWayWriteData(void* buffer
            , size_t size, size_t nmemb, void* user);

        static size_t OnGetTimeWriteData(void* buffer
            , size_t size, size_t nmemb, void* user);
        
        static size_t OnUpLoadFaceWriteData(void* buffer
            , size_t size, size_t nmemb, void* user);
          
        static size_t OnRegisterWriteHeader(void* buffer
            , size_t size, size_t nmemb, void* user);
      
      private:
        Gat1400Util::InitParam *m_initParam;
        Gat1400Util::GateParam m_gateParam; 
        Gat1400Util::RegisterAuthParam m_regauthParam;
        Gat1400Util::SystemTimeParam m_sysTimeParam;
        CurlAsyncClient *m_faceAsyncCurlClient;
        UserOperation *m_faceUploadUserOpera;
        static int m_index; 
    };
}

#endif //EMX_GAT1400REQ_HPP