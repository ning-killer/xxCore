/**
 * @file Gat1400Client.hpp
 * @author hening
 * @brief Gat1400协议客户端
 * @date 2023-6-27
 */

#ifndef EMX_GAT1400CLIENT_HPP
#define EMX_GAT1400CLIENT_HPP

#include "EmxCore.hpp"
#include "Gat1400Util.hpp"
#include "Gat1400Mgr.hpp"
#include "OvdCtx.hpp"
namespace Emx {
    class Gat1400Client {
      public:
        static Gat1400Client* Instance();
        ~Gat1400Client();

        /**
         * @brief start gat1400客户端 
         * @param ctx [in] ovdCtx句柄
         * @return true 成功；false 失败
         * @note Create()和Start()函数解决作用一致
         */
        bool Create(OvdCtx *ctx);

        /**
         * @brief start gat1400客户端
         * @param param [in] 
         * @return true 成功；false 失败
         */
        bool Start(const Gat1400Util::InitParam &param);

        /**
         * @brief stop gat1400客户端
         * @return true 成功; false 失败
         */
        bool Stop();

        /**
         * @brief gat1400数据上传入口
         * @param upload_param [in] gat1400上传数据参数结构体
         * @return true 成功; false 失败 
         * @note 内部异步投递上传，返回值无法检验是否成功上传。
         */
        bool Upload(const Gat1400Util::UploadDataParam *upload_param);

        /**
         * @brief 获取设备ID
         * @return string gat1400协议注册设备id
         */
        std::string GetDeviceId() {
            return std::string(m_initParam.deviceId);
        }
        
        /**
         * @brief 获取SourceId
         * @param devceid [in] gat1400注册设备ID
         * @param shottime [in] shot时间 yyyyMMddHHmmss
         * @return string gat1400协议注册SourceId
         */
        std::string GetGetSourceId(const std::string &devceid, const std::string &shottime);

        /**
         * @brief 获取FaceId
         * @param sourceid [in] gat1400 SourceId
         * @return string 人脸Id
         */
        std::string GetFaceId(const std::string &sourceid);

        /**
         * @brief 获取是否上传人脸全景图
         * @return true 成功; false 失败 
         */
        bool IsUploadSceneImg() {
            CHECK_GAT1400_PTR(m_gat1400Mgr, false);
            return m_gat1400Mgr->IsUploadSceneImg();
        }

        /**
         * @brief 数据投递上传
         * @param onConnect [in] 网络状态
         * @return true 成功; false 失败
         */
        static bool SetNetWorkStatus(const bool onConnect);

      private:
        Gat1400Client();
        
      private:
        static Gat1400Client *m_instance;
        bool m_started;
        EuvLoop m_loop;
        Gat1400Mgr *m_gat1400Mgr;
        Gat1400Util::InitParam m_initParam;
    };
}

#endif //EMX_GAT1400CLIENT_HPP