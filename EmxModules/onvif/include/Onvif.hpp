/*
* @Author: xiong
* @Date: 2023/1/12
*/

#ifndef EMX_Onvif_HPP
#define EMX_Onvif_HPP

#include "EmxCore.hpp"

namespace Emx {

    class Onvif {
    public:

        //!@brief 用户级别
        enum class UserLevel : uint8_t {
            Administrator, //!<管理员
            Operator, //!<操作员
            User, //!<普通用户
        };

        //!@brief 用户信息
        struct User {
            UserLevel level; //!<用户级别
            std::string name; //!<用户名
            std::string password; //!<用户密码
        };

        //!@brief Onvif参数
        struct Param {
        public:
            void Serialization(std::string &out) const {
                out.append((char *) &port, sizeof(port));
                int userNum = (int) userArray.size();
                out.append((char *) &userNum, sizeof(userNum));
                for (auto &user : userArray) {
                    out.append((char *) &user.level, sizeof(user.level));
                    int len = (int) user.name.size();
                    out.append((char *) &len, sizeof(len));
                    out.append(user.name.data(), user.name.size());
                    len = (int) user.password.size();
                    out.append((char *) &len, sizeof(len));
                    out.append(user.password.data(), user.password.size());
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
                    memcpy(&user.level, data + off, sizeof(user.level));
                    off += sizeof(user.level);
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
            }

        public:
            uint16_t port; //!<Onvif监听端口
            std::vector<User> userArray; //!<用户信息
        };

        /**
         * @brief 获取Onvif参数
         * @param param [out] 参数
         * @return ErrCodeE
         */
        virtual ErrCodeE GetParam(Param &param) = 0;

        /**
         * @brief 设置Onvif参数
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
            return UserChanged(paramA, paramB);
        }

        /**
         * @brief 判断user参数是否改变
         * @param paramA [in] 参数A
         * @param paramB [in] 参数B
         * @retval true:改变
         * @retval false:未改变
         */
        static bool UserChanged(const Param &paramA, const Param &paramB) {
            for (int i = 0; i < (int) paramA.userArray.size(); i++) {
                if (paramA.userArray[i].level != paramB.userArray[i].level)
                    return true;
                if (paramA.userArray[i].name != paramB.userArray[i].name)
                    return true;
                if (paramA.userArray[i].password != paramB.userArray[i].password)
                    return true;
            }
            return false;
        }
    };

}
#endif //EMX_Onvif_HPP
