//
// Created by xiong on 2022/4/15.
//

#ifndef EMXFCTOOLSV1_FTTOOLINNER_HPP
#define EMXFCTOOLSV1_FTTOOLINNER_HPP

#include "EmxCore.hpp"

#define EmxToolsMsgId 0x00030000


namespace Emx {

    enum FactoryMode {
        UnknownFactory = -1, //未知状态(一般是网络链路或服务未初始化完全)
        EnterFactory = 0,    //进入工厂模式：停用平台; 启用工厂工具；检验ip.cfg和autowifi.cfg；不禁用端口
        ExitFactory = 1,     //退出工厂模式：启用平台; 停用工厂工具; 不检验ip.cfg和autowifi.cfg; 禁用网络端口(telnet,tcp,rstp)
        KeepFactory = 2,     //维护工厂拓展模式：启用平台；启用工厂工具；不检验ip.cfg和autowifi.cfg；不禁用端口
    };
    
    enum class EmxToolsModule : uint8_t {
        Tools,
    };

    enum class EmxToolsMethod : uint8_t {
        Start,
        Stop,
        GetFactoryMode
    };

    struct CfgFilePath {
        /* zn_debug path*/
        char zn_debug[64] = { 0 };
        /* log.cfg path */
        char log_cfg[64] = { 0 };
        /* ip.cfg path */
        char ip_cfg[64] = { 0 };
        /* autowifi.cfg path */
        char autowifi_cfg[64] = { 0 };
        /* deviceNormal.log path*/
        char normal_log[64] = { 0 };
        /* deviceFlash.log path*/
        char flash_log[64] = { 0 };
    };

    class FtToolClient {
        public:
            static FactoryMode GetFactoryMode() {
                FactoryMode mode = UnknownFactory;
                std::string resp = "";
                if (MsgRpcReqSync::Request(EmxToolsMsgId
                    , (uint32_t) EmxToolsModule::Tools
                    , (uint32_t) EmxToolsMethod::GetFactoryMode
                    , nullptr, 0, &resp) != ErrCodeE::Success) {
                    emxloge("request factory mode failed!\n");
                    return mode;
                }
                memcpy(&mode, resp.data(), sizeof(mode));
                emxlogd("FactoryMode: %d\n", mode);
                return mode;
            }
    };
    
    
}
#endif //EMXFCTOOLSV1_FTTOOLINNER_HPP

