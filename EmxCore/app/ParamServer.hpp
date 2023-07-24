//
// Created by xiong on 2022/3/2.
//

#ifndef EMX_ParamServer_HPP
#define EMX_ParamServer_HPP

#include "core/EmxUtils.hpp"
#include "core/EmxUV.hpp"
#include "core/EmxMsg.hpp"

namespace Emx {
    class ParamServer {
    public:
        ParamServer();

        void Create();

        void Destroy();

    private:
        ErrCodeE OnRecvReq(uint32_t module, uint32_t method,
                           const char *data, int32_t size,
                           std::string &out);

        ErrCodeE Set(const char *name, const char *data);

        ErrCodeE Get(const char *name, std::string &data);

        ErrCodeE GetDefault(const char *name, std::string &data);

        ErrCodeE Reset(const char *name);

        ErrCodeE Load(const char *path, std::string &data);

        void CreateSubDir(char *def, char *normal, char *backup);

        ErrCodeE ResetToFactory();

    private:
        EuvLoop m_loop;
        MsgRpcRep m_rep;
        char m_default[EMX_MAX_PATH_SIZE];
        char m_normal[EMX_MAX_PATH_SIZE];
        char m_backup[EMX_MAX_PATH_SIZE];
        char m_buffer[4096];
        AES_CBC m_aes;
        uint8_t m_key[17];
        uint8_t m_iv[17];
    };

}

#endif //EMX_ParamServer_HPP
