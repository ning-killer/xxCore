/*
* @Author: xiong
* @Date: 2023/1/17
*/

#ifndef EMX_WebServer_HPP
#define EMX_WebServer_HPP

#include "EmxProtoV2.hpp"
#include "WebLive.hpp"
#include <thread>

struct mg_mgr;
struct mg_connection;
struct mg_http_message;

namespace Emx {

    class WebServer {
    public:

        void Create(bool newThread);

        void Destroy();

    private:
        static void StaticEntry(struct mg_connection *c, int ev, void *ev_data, void *fn_data);

        void CallBack(struct mg_connection *c, int ev, void *ev_data, void *fn_data);

        ErrCodeE LoadParam();

        ErrCodeE SaveParam();

    private:
        //!@brief 用户级别
        enum class Role : uint8_t {
            Admin, //!<管理员
            Operator, //!<操作员
            Normal, //!<普通用户
            Illegal,//!等级非法
        };

        enum class LoginStatusE : uint8_t {
            UnLogin = 0,//未登录
            Login,//已登录
            Timeout,//登录超时
        };

        //!@brief 用户信息
        struct User {
            Role role; //!<用户级别
            std::string username; //!<用户名
            std::string password; //!<用户密码
        };

        //!@brief Onvif参数
        struct Param {
            uint16_t port; //!<监听端口
            std::vector<std::shared_ptr<User>> userList; //!<用户信息
        };

        struct UserCtx {
            std::shared_ptr<User> user;
            std::string token;
            static const int NoneOptWaitTimeMs = 5 * 60 * 1000;//5min
            uint64_t tokenTimeoutClockMs;
        };
    private:

        static const char *RoleEnum2String(Role level);

        static Role RoleString2Enum(const char *str);

    private:
        LoginStatusE GetLogInStatus(struct mg_http_message *hm);

        void UserLogin(struct mg_http_message *hm, Json::Value &out);

        void UserLogout(struct mg_http_message *hm);

        static std::string GetToken(struct mg_http_message *hm);

        static std::string CreateToken();

        void AddUser(struct mg_http_message *hm, Json::Value &out);

        void DelUser(struct mg_http_message *hm, Json::Value &out);

        void ModUser(struct mg_http_message *hm, Json::Value &out);

        void GetUser(struct mg_http_message *hm, Json::Value &out);

    private:
        static void TimerCallback(void *arg);

    private:
        bool m_run;
        std::thread m_thread;
        Param m_param;
        mg_mgr *m_mg;
        std::map<std::string, std::shared_ptr<UserCtx> > m_userCtx; //<token:ctx>
        EmxProtoV2 m_proto;
        WebLive m_live;

    };
}

#endif //EMX_WebServer_HPP
