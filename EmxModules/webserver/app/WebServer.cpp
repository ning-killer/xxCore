/*
* @Author: xiong
* @Date: 2023/1/17
*/
#include "EmxCore.hpp"
#include "WebServer.hpp"
#include "mongoose.h"

using namespace Emx;

void WebServer::Create(bool newThread) {
    LoadParam();
    mg_log_set(MG_LL_INFO);
    m_mg = new mg_mgr;
    mg_mgr_init(m_mg);
    m_mg->userdata = this;
    char url[128];
    snprintf(url, sizeof(url), "http://0.0.0.0:%d", m_param.port);
    mg_http_listen(m_mg, url, StaticEntry, m_mg);
    emxlogi("listen on %s\n", url);
    mg_timer_add(m_mg, 10, MG_TIMER_REPEAT, TimerCallback, this);
    if (newThread) {
        m_run = true;
        m_thread = std::thread([this]() {
            while (m_run && m_mg->conns != nullptr) mg_mgr_poll(m_mg, 10);
            mg_mgr_free(m_mg);
            delete m_mg;
        });
    } else {
        while (m_mg->conns != nullptr) mg_mgr_poll(m_mg, 10);
        mg_mgr_free(m_mg);
        delete m_mg;
    }
}

void WebServer::Destroy() {
    m_run = false;
    m_thread.join();
}

ErrCodeE WebServer::LoadParam() {
    ::Param param("webserver");
    Json::Value json;
    if (param.Get(json) != ErrCodeE::Success) {
        emxloge("load param failed\n");
        return ErrCodeE::Failure;
    }
    m_param.port = json["port"].asInt();
    m_param.userList.clear();
    for (auto &userJson : json["user"]) {
        auto user = std::make_shared<User>();
        user->role = RoleString2Enum(userJson["role"].asCString());
        user->username = userJson["username"].asString();
        user->password = userJson["password"].asString();
        m_param.userList.emplace_back(user);
    }
    return ErrCodeE::Success;
}

ErrCodeE WebServer::SaveParam() {
    Json::Value json;
    json["port"] = m_param.port;
    auto &userArray = json["user"];
    userArray.resize(0);
    for (auto &user : m_param.userList) {
        Json::Value userJson;
        userJson["role"] = RoleEnum2String(user->role);
        userJson["username"] = user->username;
        userJson["password"] = user->password;
        userArray.append(userJson);
    }
    ::Param param("webserver");
    if (param.Set(json) != ErrCodeE::Success) {
        emxloge("save param failed\n");
        return ErrCodeE::Failure;
    }
    return ErrCodeE::Success;
}

void WebServer::StaticEntry(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
    auto web = (WebServer *) c->mgr->userdata;
    web->CallBack(c, ev, ev_data, fn_data);
}

void WebServer::CallBack(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
    if (ev == MG_EV_HTTP_MSG) {
        auto *hm = (struct mg_http_message *) ev_data;
        emxlogd("url[%.*s]\n", (int) hm->uri.len, hm->uri.ptr);
        auto loginStatus = GetLogInStatus(hm);
//        auto loginStatus = LoginStatusE::Login;
        if (loginStatus != LoginStatusE::Login && mg_http_match_uri(hm, "/api/#")) {
            Json::Value json;
            if (mg_http_match_uri(hm, "/api/login")) {
                UserLogin(hm, json);
            } else if (loginStatus == LoginStatusE::Timeout) {
                json["code"] = -203;
                json["desc"] = "Token 已过期";
            } else {
                json["code"] = -201;
                json["desc"] = "未授权";
            }
            mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                          "%s", json.toStyledString().c_str());
        } else if (mg_http_match_uri(hm, "/api/login")) {
            Json::Value json;
            UserLogin(hm, json);
            mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                          "%s", json.toStyledString().c_str());
        } else if (mg_http_match_uri(hm, "/api/logout")) {
            UserLogout(hm);
            Json::Value json;
            json["code"] = 0;
            json["desc"] = "登出成功";
            mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                          "%s", json.toStyledString().c_str());
        } else if (mg_http_match_uri(hm, "/api/users")) {
            Json::Value json;
            if (strncmp(hm->method.ptr, "PUT", hm->method.len) == 0) {
                AddUser(hm, json);
            } else if (strncmp(hm->method.ptr, "DELETE", hm->method.len) == 0) {
                DelUser(hm, json);
            } else if (strncmp(hm->method.ptr, "POST", hm->method.len) == 0) {
                ModUser(hm, json);
            } else if (strncmp(hm->method.ptr, "GET", hm->method.len) == 0) {
                GetUser(hm, json);
            } else {
                json["code"] = -209;
                json["desc"] = "未定义操作";
            }
            mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                          "%s", json.toStyledString().c_str());
        } else if (mg_http_match_uri(hm, "/api/protov2")) {
            emxlogd("req[%.*s]\n", (int) hm->body.len, hm->body.ptr);
            std::string out;
            m_proto.Proc(hm->body.ptr, (int32_t) hm->body.len, out);
            emxlogd("resp[%s]\n", out.c_str());
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", out.c_str());
        } else if (mg_http_match_uri(hm, "/api/live/#")) {
            mg_ws_upgrade(c, hm, nullptr);
            char *p = (char *) strstr((const char *) hm->uri.ptr, "/api/live/chn");
            char *end = nullptr;
            int chn = strtol(p + strlen("/api/live/chn"), &end, 10);
            if (end) {
                std::string liveToken = end + 1;

            }
            m_live.Create(c, chn);
        } else {
            static char root[EMX_MAX_PATH_SIZE] = {};
            snprintf(root, sizeof(root), "%s/webserver/web", Misc::GetAppConfigsDir());
            struct mg_http_serve_opts opts = {.root_dir = root};
            mg_http_serve_dir(c, hm, &opts);
        }
    } else if (ev == MG_EV_WS_OPEN) {
        auto wm = (struct mg_ws_message *) ev_data;
        emxlogd("MG_EV_WS_OPEN[%.*s]\n", wm->data.len, wm->data.ptr);
    } else if (ev == MG_EV_WS_MSG) {
        auto wm = (struct mg_ws_message *) ev_data;
        m_live.RecvPcm((uint8_t *) wm->data.ptr, (int) wm->data.len);
    } else if (ev == MG_EV_WS_CTL) {
        auto wm = (struct mg_ws_message *) ev_data;
        emxlogd("MG_EV_WS_CTL[%.*s]\n", wm->data.len, wm->data.ptr);
        m_live.Destroy();
    } else if (ev == MG_EV_ERROR) {
        m_live.Destroy();
    }
    (void) fn_data;
}

std::string WebServer::CreateToken() {
    // base64(sha1(nonce+createTimestamp))
    const char *seed = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    auto seedSize = (int) strlen(seed);
    char nonce[17] = {0};
    for (int i = 0; i < 16; ++i) {
        nonce[i] = seed[Misc::GetRandom(0, seedSize - 1)];
    }

    uint64_t createTimestamp = Time::GetMonoClockUs();

    std::string digestPlain = nonce + std::to_string(createTimestamp);

    unsigned char digestCrypt[20] = {};
    mg_sha1_ctx sha1 = {};
    mg_sha1_init(&sha1);
    mg_sha1_update(&sha1, (unsigned char *) digestPlain.data(), digestPlain.size());
    mg_sha1_final(digestCrypt, &sha1);

    char digestBase64[40] = {};
    mg_base64_encode(digestCrypt, sizeof(digestCrypt), digestBase64);
    return digestBase64;
}

void WebServer::TimerCallback(void *arg) {
    auto web = (WebServer *) arg;
    web->m_live.SendRtpFrame();
}

const char *WebServer::RoleEnum2String(WebServer::Role level) {
    switch (level) {
        case Role::Admin:
            return "admin";
        case Role::Operator:
            return "operator";
        case Role::Normal:
            return "normal";
        default:
            return "illegal";
    }
}

WebServer::Role WebServer::RoleString2Enum(const char *str) {
    if (strcmp(str, "admin") == 0)
        return Role::Admin;
    else if (strcmp(str, "operator") == 0)
        return Role::Operator;
    else if (strcmp(str, "normal") == 0)
        return Role::Normal;
    else
        return Role::Illegal;
}

std::string WebServer::GetToken(struct mg_http_message *hm) {
    struct mg_str *v = mg_http_get_header(hm, "Authorization");
    if (!v) {
        if (strstr(hm->uri.ptr, "/api/live/")) {
            std::string strUrl(hm->uri.ptr, hm->uri.len);
            return strUrl.substr(strlen("/api/live/chn0/"), strUrl.size() - strlen("/api/live/chn0/"));
        }
    }
    return v ? std::string(v->ptr, v->len) : "";
}


void WebServer::UserLogin(struct mg_http_message *hm, Json::Value &out) {
    Json::Value body;
    EasyJson::Parse(hm->body.ptr, (int) hm->body.len, body);
    std::shared_ptr<UserCtx> userCtx = nullptr;
    for (auto &usr : m_param.userList) {
        if (body["username"].asString() == usr->username && body["password"].asString() == usr->password) {
            //检查用户是否已经登录，如果已经登录则重新生成token
            for (auto &ctx : m_userCtx) {
                auto c = ctx.second;
                if (c->user.get() == usr.get()) {
                    m_userCtx.erase(ctx.first);
                    c->token = CreateToken();
                    c->tokenTimeoutClockMs = Time::GetMonoClockMS() + UserCtx::NoneOptWaitTimeMs;
                    m_userCtx[c->token] = c;
                    userCtx = c;
                }
            }
            //如果没有登录则创建新的ctx
            if (userCtx == nullptr) {
                userCtx = std::make_shared<UserCtx>();
                userCtx->user = usr;
                userCtx->token = CreateToken();
                userCtx->tokenTimeoutClockMs = Time::GetMonoClockMS() + UserCtx::NoneOptWaitTimeMs;
                m_userCtx[userCtx->token] = userCtx;
                break;
            }

        }
    }
    if (userCtx) {
        out["code"] = 0;
        out["desc"] = "登录成功";
        auto &data = out["data"];
        data["username"] = userCtx->user->username;
        data["role"] = RoleEnum2String(userCtx->user->role);
        data["token"] = userCtx->token;
    } else {
        out["code"] = -204;
        out["desc"] = "登录失败";
    }
}

void WebServer::UserLogout(struct mg_http_message *hm) {
    Json::Value body;
    EasyJson::Parse(hm->body.ptr, (int) hm->body.len, body);
    if (m_userCtx.count(body["token"].asString())) {
        m_userCtx.erase(body["token"].asString());
    }
}

WebServer::LoginStatusE WebServer::GetLogInStatus(struct mg_http_message *hm) {
    std::string token = GetToken(hm);
    if (!m_userCtx.count(token)) {
        return LoginStatusE::UnLogin;
    }
    auto clock = Time::GetMonoClockMS();
    //token超时
    if (clock > m_userCtx[token]->tokenTimeoutClockMs) {
        emxloge("%s token timeout\n", m_userCtx[token]->user->username.c_str());
        return LoginStatusE::Timeout;
    }
    //更新token超时时间
    m_userCtx[token]->tokenTimeoutClockMs = clock + UserCtx::NoneOptWaitTimeMs;
    return LoginStatusE::Login;
}

void WebServer::AddUser(struct mg_http_message *hm, Json::Value &out) {
    std::string token = GetToken(hm);
    if (m_userCtx[token]->user->role != Role::Admin) {
        out["code"] = -202;
        out["desc"] = "无权限";
        return;
    }
    Json::Value body;
    EasyJson::Parse(hm->body.ptr, (int) hm->body.len, body);
    for (auto &user : m_param.userList) {
        if (user->username == body["username"].asString()) {
            out["code"] = -205;
            out["desc"] = "用户已存在";
            return;
        }
    }

    if (!body.isMember("username") || !body.isMember("password")) {
        out["code"] = -210;
        out["desc"] = "参数错误";
        return;
    }

    if (body["username"].asString().size() < 3) {
        out["code"] = -210;
        out["desc"] = "用户名不足3位";
        return;
    }

    if (body["password"].asString().size() < 8) {
        out["code"] = -210;
        out["desc"] = "密码不足8位";
        return;
    }
    auto user = std::make_shared<User>();
    user->username = body["username"].asString();
    user->password = body["password"].asString();
    user->role = RoleString2Enum(body["role"].asCString());
    m_param.userList.emplace_back(user);
    SaveParam();
    out["code"] = 0;
    out["desc"] = "添加用户成功";
}

void WebServer::DelUser(struct mg_http_message *hm, Json::Value &out) {
    std::string token = GetToken(hm);
    if (m_userCtx[token]->user->role != Role::Admin) {
        out["code"] = -202;
        out["desc"] = "无权限";
        return;
    }
    Json::Value body;
    EasyJson::Parse(hm->body.ptr, (int) hm->body.len, body);
    if (!body.isMember("username")) {
        out["code"] = -210;
        out["desc"] = "参数错误";
        return;
    }
    if (m_param.userList[0]->username == body["username"].asString()) {
        out["code"] = -207;
        out["desc"] = "无法删除默认用户";
        return;
    }
    if (m_userCtx[token]->user->username == body["username"].asString()) {
        out["code"] = -208;
        out["desc"] = "无法删除自己";
        return;
    }
    for (auto it = m_param.userList.begin(); it < m_param.userList.end(); it++) {
        if ((*it)->username == body["username"].asString()) {
            for (auto &ctx: m_userCtx) {
                if (ctx.second->user->username == body["username"].asString()) {
                    m_userCtx.erase(ctx.first);
                    break;
                }
            }
            m_param.userList.erase(it);
            out["code"] = 0;
            out["desc"] = "删除成功";
            SaveParam();
            return;
        }
    }

    out["code"] = -206;
    out["desc"] = "用户不存在";
}

void WebServer::ModUser(struct mg_http_message *hm, Json::Value &out) {
    Json::Value body;
    EasyJson::Parse(hm->body.ptr, (int) hm->body.len, body);

    if (!body.isMember("username") ||
        !body.isMember("role") ||
        RoleString2Enum(body["role"].asCString()) == Role::Illegal) {
        out["code"] = -210;
        out["desc"] = "参数错误";
        return;
    }

    std::string token = GetToken(hm);
    if (m_userCtx[token]->user->role != Role::Admin &&
        m_userCtx[token]->user->username != body["username"].asString()) {
        out["code"] = -202;
        out["desc"] = "无权限";
        return;
    }

    if (m_param.userList[0]->username == body["username"].asString() &&
        RoleString2Enum(body["role"].asCString()) != m_param.userList[0]->role) {
        out["code"] = -202;
        out["desc"] = "无权限";
        return;
    }

    if (body.isMember("password") && body["password"].asString().size() < 8) {
        out["code"] = -210;
        out["desc"] = "密码不足8位";
        return;
    }

    for (auto it = m_param.userList.begin(); it < m_param.userList.end(); it++) {
        if ((*it)->username == body["username"].asString()) {
            if (body.isMember("password")) {
                (*it)->password = body["password"].asString();
            }
            (*it)->role = RoleString2Enum(body["role"].asCString());
            for (auto &ctx: m_userCtx) {
                if (ctx.second->user->username == body["username"].asString()) {
                    m_userCtx.erase(ctx.first);
                    ctx.second->token = CreateToken();
                    ctx.second->tokenTimeoutClockMs = Time::GetMonoClockMS() + UserCtx::NoneOptWaitTimeMs;
                    m_userCtx[ctx.second->token] = ctx.second;
                    break;
                }
            }
            out["code"] = 0;
            out["desc"] = "修改成功";
            SaveParam();
            return;
        }
    }

    out["code"] = -206;
    out["desc"] = "用户不存在";
}

void WebServer::GetUser(struct mg_http_message *hm, Json::Value &out) {
    auto &data = out["data"];
    data.resize(0);
    for (auto &usr: m_param.userList) {
        Json::Value json;
        json["username"] = usr->username;
        json["role"] = RoleEnum2String(usr->role);
        data.append(json);
    }
    out["code"] = 0;
    out["desc"] = "获取成功";
}


