/*
* @Author: xiong
* @Date: 2023/1/16
*/

#ifndef EMX_Rtsp_HPP
#define EMX_Rtsp_HPP

#include "EmxCore.hpp"

namespace Emx {

    class Rtsp {
    public:
        //!@brief 用户信息
        struct User {
            std::string name; //!<用户名
            std::string password; //!<用户密码
        };

        struct Media {
            static const int MaxNameSize = 32;
            char name[MaxNameSize];
            int vencChn;
            int aencChn;
        };

        //!@brief Rtsp参数
        struct Param {
        public:
            void Serialization(std::string &out) const {
                out.append((char *) &port, sizeof(port));
                int userNum = (int) userArray.size();
                out.append((char *) &userNum, sizeof(userNum));
                for (auto &user : userArray) {
                    int len = (int) user.name.size();
                    out.append((char *) &len, sizeof(len));
                    out.append(user.name.data(), user.name.size());
                    len = (int) user.password.size();
                    out.append((char *) &len, sizeof(len));
                    out.append(user.password.data(), user.password.size());
                }
                int mediaNum = (int) mediaArray.size();
                out.append((char *) &mediaNum, sizeof(mediaNum));
                for (auto &media : mediaArray) {
                    out.append((char *) media.name, sizeof(media.name));
                    out.append((char *) &media.vencChn, sizeof(media.vencChn));
                    out.append((char *) &media.aencChn, sizeof(media.aencChn));
                }
            }

            void Parse(const char *data, int size) {
                int off = 0;
                memcpy(&port, data + off, sizeof(port));
                off += sizeof(port);
                int userNum = 0;
                memcpy(&userNum, data + off, sizeof(userNum));
                off += sizeof(userNum);
                for (int i = 0; i < userNum; i++) {
                    User user = {};
                    int len = 0;
                    memcpy(&len, data + off, sizeof(len));
                    off += sizeof(len);
                    user.name.assign(data + off, len);
                    off += len;
                    memcpy(&len, data + off, sizeof(len));
                    off += sizeof(len);
                    user.password.assign(data + off, len);
                    off += len;
                    userArray.emplace_back(user);
                }
                int mediaNum = 0;
                memcpy(&mediaNum, data + off, sizeof(mediaNum));
                off += sizeof(mediaNum);
                for (int i = 0; i < mediaNum; i++) {
                    Media media = {};
                    memcpy(media.name, data + off, sizeof(media.name));
                    off += sizeof(media.name);
                    memcpy(&media.vencChn, data + off, sizeof(media.vencChn));
                    off += sizeof(media.vencChn);
                    memcpy(&media.aencChn, data + off, sizeof(media.aencChn));
                    off += sizeof(media.aencChn);
                    mediaArray.emplace_back(media);
                }
            }

        public:
            uint16_t port; //!<Rtsp监听端口
            std::vector<User> userArray; //!<用户信息
            std::vector<Media> mediaArray;
        };

        /**
       * @brief 获取参数
       * @param param [out] 参数
       * @return ErrCodeE
       */
        virtual ErrCodeE GetParam(Param &param) = 0;

        /**
         * @brief 设置参数
         * @param param [in] 参数
         * @return ErrCodeE
         */
        virtual ErrCodeE SetParam(const Param &param) = 0;

        /**
         * @brief 判断参数是否改变
         * @param paramA [in] 参数A
         * @param paramB [in] 参数B
         * @retval true:改变
         * @retval false:未改变
         */
        static bool ParamChanged(const Param &paramA, const Param &paramB) {
            if (paramA.port != paramB.port)
                return true;
            if (paramA.userArray.size() != paramB.userArray.size())
                return true;
            for (int i = 0; i < (int) paramA.userArray.size(); i++) {
                if (paramA.userArray[i].name != paramB.userArray[i].name)
                    return true;
                if (paramA.userArray[i].password != paramB.userArray[i].password)
                    return true;
            }
            return false;
        }
    };

}
#endif //EMX_Rtsp_HPP
