/**
 * @file AgeingTool.hpp
 * @author hening
 * @brief 老化工具
 * @date 2023-4-20
 */

#ifndef EMXFACTORYTOOLSV1_AGEINGTOOL_HPP
#define EMXFACTORYTOOLSV1_AGEINGTOOL_HPP

#include "EmxCore.hpp"
#include "AgeingCtx.hpp"

#define EMX_TOOLS_BUFFER_SIZE 512

namespace Emx {
    class AgeingTool {
      public:
        AgeingTool();
        virtual ~AgeingTool();

      public:
        ErrCodeE Create();
        ErrCodeE Stop();

        /**
         * @brief  获取老化配置信息。
         * @param  info [in|out] 老化配置信息AgingInfo。
         * @return 结果错误码。
         */
        ErrCodeE GetAgeingInfo(AgingInfo &info);

      private:
        ErrCodeE OnRecvMsg(uint32_t module, uint32_t method,
                           const char *data, int32_t size,
                           std::string &out);

      private:
        EuvLoop m_loop;
        AgeingCtx *m_ageing_ctx;
        MsgRpcRep m_resp;
        char m_buffer[EMX_TOOLS_BUFFER_SIZE] = { 0 };
    };
}

#endif //EMXFACTORYTOOLSV1_AGEINGTOOL_HPP
