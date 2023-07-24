#include "HttpSession.h"
#include "DigestAuth.h"
#include "tinyxml2.h"
#include "sockutil.h"
#include "soapdefine.h"
#include "WSAuth.h"
#include "SoapDispatcher.h"
#include "ThreadPool.h"
#include "HttpMessage.h"
#include "EmxCore.hpp"

HttpSession::HttpSession(int fd, const EventPoller::Ptr poller) : TcpSession(fd, poller) {
    emxlogd("construct httpsession %p\n", this);
    _soapReq = std::make_shared<SoapRequest>();
}

HttpSession::~HttpSession() {
    emxlogd("deconstruct httpsession %p\n", this);
}

int HttpSession::handlePkt() {
    //if(_recvStatus == 0)
    //{
    //    auto ptr = strstr(recvBuf, "\r\n\r\n");//判断头接收完没
    //    if (ptr == nullptr)//需要继续接收
    //        return 1;

    //    _recvStatus = 1;
    //    ptr += 4;
    //    auto headerSize = ptr - recvBuf;
    //    std::string hd(recvBuf, headerSize);

    //    _httpHeader->parse(hd);
    //    _contentLength = _httpHeader->getContentSize();
    //    auto remainSize = availableSize - headerSize;
    //    availableSize = 0;
    //    idleSize = 1024 * 4;
    //    if (_contentLength > 0)
    //    {
    //        if (_httpHeader->_url == "/dev/firmware-upgrade")/*handle firmware upgrade */
    //        {
    //            if (!_fPtr)
    //                _fPtr = fopen("/tmp/rom.bin", "wb");

    //            fwrite(ptr, 1, remainSize, _fPtr);
    //        }
    //        else
    //            _body.append(ptr, remainSize);

    //        _writeBytes += remainSize;

    //        if (remainSize < _contentLength)
    //            return 0;
    //    }
    //    _recvStatus = 2;
    //}
    //if (_recvStatus == 1)
    //{
    //    _writeBytes += availableSize;
    //    if (_fPtr)/*continue recv upgrade file*/
    //        fwrite(recvBuf, 1, availableSize, _fPtr);
    //    else
    //        _body.append(recvBuf, availableSize);

    //    idleSize = 1024 * 4;
    //    availableSize = 0;
    //    if (_writeBytes != _contentLength)
    //        return 0;
    //    _recvStatus = 2;
    //}
    //if (_recvStatus == 1)
    //{
    //    _writeBytes += availableSize;
    //    if (_fPtr)/*continue recv upgrade file*/
    //        fwrite(recvBuf, 1, availableSize, _fPtr);
    //    else
    //        _body.append(recvBuf, availableSize);

    //    idleSize = 1024 * 4;
    //    availableSize = 0;
    //    if (_writeBytes != _contentLength)
    //        return 0;
    //    _recvStatus = 2;
    //}
    //if (_recvStatus == 2) {
    //    if (_connectionUpgrade)
    //        return HandleWebSocketPkt();
    //    if (_fPtr) {
    //        fclose(_fPtr);
    //        _fPtr = NULL;
    //        //DeviceConfigManager::Instance().firmwareUpgrade(_body, _httpHeader->_cmd);
    //        return sendWebResp(200, "{\"code\":0, \"result\":\"succ\"}", "application/json");
    //    }

    //    if (strstr(_httpHeader->_url.c_str(), "Subscription")) {
    //        auto strongRef = std::dynamic_pointer_cast<HttpSession>(shared_from_this());
    //        ThreadPool::Instance().pushTask([strongRef]() {
    //            strongRef->handleHTTP();
    //            return 0;
    //        });
    //    } else {
    //        /* judge ws*/
    //        auto it = _httpHeader->kv.find("Upgrade");
    //        if (it != _httpHeader->kv.end() && !strcasecmp(it->second.c_str(), "websocket"))/*websocket*/
    //        {
    //            HandleSwitchingProtocols();
    //            _connectionUpgrade = true;
    //            return 0;
    //        }

    //        handleHTTP();
    //    }
    //}

    if (_recvStatus == 0) {
        auto ptr = strstr(_buffer.c_str(), "\r\n\r\n");
        if (ptr == nullptr)
            return 1;
        else {
            _recvStatus = 1;
            ptr += 4;
            auto headerSize = ptr - _buffer.c_str();
            std::string hd(_buffer.c_str(), headerSize);

            _req = new HttpRequest();
            _req->Parse(hd);

            _contentLength = _req->ContentLength();

            //SPDLOG_INFO("{}", hd);

            _buffer.erase(0, headerSize);
            if (_contentLength > 0) {
                _body.append(_buffer);
                _buffer.clear();
                if (_body.size() < _contentLength)
                    return 0;
            }
            _recvStatus = 2;
        }
    }

    if (_recvStatus == 1) {
        _body.append(_buffer);
        _buffer.clear();
        if (_body.size() < _contentLength)
            return 0;
        _recvStatus = 2;
    }
    int ret = 0;
    if (_recvStatus == 2) {
        if (!_body.empty())
            emxlogd("%s\n", _body.c_str());

        ret = handleHTTP();

        _body.clear();
        delete _req;
        _req = nullptr;
        _recvStatus = 0;
    }
    return ret;
}

int HttpSession::handleHTTP() {
    /*onvif*/
    auto uri = _req->Url();
    emxlogi("%d %s\n", _req->method(), uri.c_str());
    if (strncmp(uri.c_str(), "/onvif", 6) == 0)/*onvif*/
        return HandleOnvif();
    else {
        return ResponseSC(200, "");
    }
}

int HttpSession::sendPostResponse(int sc, std::string b) {
    HttpResponse r;
    r.set("Access-Control-Allow-Origin", "*");
    r.set("Connection", "close");
    r.setStatus(sc);
    if (sc == 401)
        r.set("WWW-Authenticate", DigestAuth::Instance().createWWWAuth().c_str() + 18);

    if (!b.empty())
        r.set("Content-Type", "application/soap+xml; charset=utf-8");

    r.setContentLength(b.size());
    auto header = r.toStr();

    if (_body.empty())
        return sendBinaryData(_fd, header.c_str(), header.size());
    else {
        sendBinaryData(_fd, header.c_str(), header.size());
        return sendBinaryData(_fd, b.c_str(), b.size());
    }
}

int HttpSession::parseXmlBody() {
    std::string tag;
    tinyxml2::XMLDocument doc;
    auto err = doc.Parse(_body.c_str(), _body.size());
    if (err != tinyxml2::XML_SUCCESS) {
        //std::cout << "parse xml failed" << err << std::endl;
        return 1;
    }
    //Envelope
    auto envelopeNode = doc.FirstChildElement();
//	auto kp = envelopeNode->Value();
    if (!strstr(envelopeNode->Value(), "Envelope")) {
        //std::cout << "not a incorrect soap message" << std::endl;
        return 1;
    }
    //Header or body
    auto child = envelopeNode->FirstChildElement();
    if (strstr(child->Value(), "Header")) {
        /**/
        for (auto i = child->FirstChildElement(); i; i = i->NextSiblingElement()) {
            if (strstr(i->Value(), "Security")) {
                _soapReq->securityHeader = std::make_shared<_wsse__Security>();
                auto userNameToken = i->FirstChildElement();
                if (userNameToken && strstr(userNameToken->Value(), "UsernameToken")) {
                    _soapReq->securityHeader->userToken = std::make_shared<_wsse__UsernameToken>();
                    for (auto token = userNameToken->FirstChildElement(); token; token = token->NextSiblingElement()) {
                        if (strstr(token->Value(), "Username")) {
                            _soapReq->securityHeader->userToken->Username = token->GetText();
                        } else if (strstr(token->Value(), "Password")) {
                            _soapReq->securityHeader->userToken->cipherPassword = token->GetText();
                            auto encType = token->FindAttribute("Type");
                            if (encType)
                                _soapReq->securityHeader->userToken->Type = encType->Value();
                        } else if (strstr(token->Value(), "Nonce")) {
                            _soapReq->securityHeader->userToken->cipherNonce = token->GetText();
                            auto encType = token->FindAttribute("Type");
                            if (encType)
                                _soapReq->securityHeader->userToken->EncodingType = encType->Value();
                        } else if (strstr(token->Value(), "Created")) {
                            _soapReq->securityHeader->userToken->wsu__Created = token->GetText();
                        }
                    }
                }//token
                break;
            }
        }
        //auto securityNode = child->FirstChildElement();
        //if(securityNode && strstr(securityNode->Value(), "Security"))
        //{

        //}//security
        child = child->NextSiblingElement();
    }
    if (strstr(child->Value(), "Body")) {
        auto p = child->FirstChildElement()->Value();
        auto q = strchr(p, ':');
        if (q)
            _soapReq->tag = q + 1;
        else
            _soapReq->tag = p;
    }
    return 0;
}

bool HttpSession::CheckAuth(const char *method, unsigned char &mode) {
    /*���ws-security*/
    if (_soapReq->securityHeader && _soapReq->securityHeader->userToken) {
        mode = 0;
        auto token = _soapReq->securityHeader->userToken;
        if (token->Username.empty() || token->cipherPassword.empty()) {
            fprintf(stderr, "Username or Password is empty!\n");
            return false;
        }
        if (!token->Type.empty() && !strcmp(token->Type.c_str(), EPSoap::Instance().PasswordDigestURI.c_str())) {
            if (!_soapReq->securityHeader->userToken->cipherNonce.empty()
                && !token->wsu__Created.empty()
                && (token->cipherPassword.size() == 28)) {
                if (token->EncodingType.empty() ||
                    !strcmp(token->EncodingType.c_str(), EPSoap::Instance().Base64BinaryURI.c_str())) {
                    return WSAuth::Instance().VerifyPassword(token->cipherPassword, token->cipherNonce,
                                                             token->wsu__Created);
                } else if (!strcmp(token->EncodingType.c_str(), EPSoap::Instance().HexBinaryURI.c_str())) {
                    emxloge("No function is implemented here!\n");
                }
            }
        } else {
            if (!strcmp(token->cipherPassword.c_str(), "admin")) {
                return true;
            }

        }
    }
    /*http-digest*/
    auto authStr = _req->get("Authorization");
    if (!authStr.empty()) {
        mode = 1;
        AuthorizationInfo info;
        /*realm*/
        auto p1 = authStr.find("realm=");
        if (p1 == authStr.npos)
            return false;
        auto p2 = authStr.find('"', p1 + 7);
        info.realm = authStr.substr(p1 + 7, p2 - p1 - 7);
        /*uri*/
        p1 = authStr.find("uri=");
        if (p1 == authStr.npos)
            return false;
        p2 = authStr.find('"', p1 + 5);
        info.uri = authStr.substr(p1 + 5, p2 - p1 - 5);
        /*nonce*/
        p1 = authStr.find("nonce=");
        if (p1 == authStr.npos)
            return false;
        p2 = authStr.find('"', p1 + 7);
        info.nonce = authStr.substr(p1 + 7, p2 - p1 - 7);
        /*response*/
        p1 = authStr.find("response=");
        if (p1 == authStr.npos)
            return false;
        p2 = authStr.find('"', p1 + 10);
        info.response = authStr.substr(p1 + 10, p2 - p1 - 10);
        /*nc*/
        p1 = authStr.find("nc=");
        if (p1 != authStr.npos) {
            p2 = authStr.find(',', p1 + 3);
            info.nc = authStr.substr(p1 + 3, p2 - p1 - 3);
        }
        /*cnonce*/
        p1 = authStr.find("cnonce=");
        if (p1 != authStr.npos) {
            p2 = authStr.find('"', p1 + 8);
            info.cnonce = authStr.substr(p1 + 8, p2 - p1 - 8);
        }
        /*qop*/
        p1 = authStr.find("qop=");
        if (p1 != authStr.npos) {
            do {
                p2 = authStr.find("auth-int", p1 + 4);
                if (p2 != authStr.npos) {
                    info.qop = "auth-int";
                    break;
                }
                p2 = authStr.find("auth", p1 + 4);
                if (p2 != authStr.npos)
                    info.qop = "auth";
            } while (0);
        }
        info.method = method;
        return DigestAuth::Instance().VerifyResponse(info);
    }

    return false;
}

int HttpSession::sendImageResponse(const std::string &data) {
    char buf[256];
    std::string header;
    snprintf(buf, 256, "HTTP/1.1 %d OK\r\n", 200);
    header.append(buf);

    header.append("Content-Type: image/jpeg\r\n");
    header.append("Content-Transfer-Encoding: binary\r\n");
    header.append("Connection: close\r\n");
    char cntLength[32];
    snprintf(cntLength, 32, "Content-Length: %lu\r\n\r\n", (unsigned long int) data.size());
    header.append(cntLength);

    if (data.empty())
        return sendBinaryData(_fd, header.c_str(), header.size());
    else {
        sendBinaryData(_fd, header.c_str(), header.size());
        return sendBinaryData(_fd, data.c_str(), data.size());
    }
}

int HttpSession::HandleOnvif() {
    auto uri = _req->Url();
    if (_req->method() == 1) {
        int ret = parseXmlBody();
        if (ret) {
            _body = EPSoap::Instance().CreateSoapFaultMessage("s:Sender", "ter:WellFormed", "Well-formed Error");
            sendPostResponse(400, _body);
            return 1;
        }
        /*authentication*/
        unsigned char m = 1;
        if (!CheckAuth("POST", m)) {
            if (m) {
                _body.clear();
                sendPostResponse(401, "");
            } else {
                _body = EPSoap::Instance().strAuthenticationError;
                sendPostResponse(400, _body);
            }
            return 1;
        }
        std::function<int(void *, std::string &)> handler;

        if (SoapDispatcher::Instance().QueryHandler(uri, _soapReq->tag, handler)) {
            handler(nullptr, _body);
            sendPostResponse(200, _body);
        } else {
            sendPostResponse(501, "");
        }
    } else if (_req->method() == 0) {
        /*authentication*/
        unsigned char m = 1;
        if (!CheckAuth("GET", m)) {
            sendPostResponse(m ? 401 : 400, "");
            return 1;
        }

        if (uri == "/onvifsnapshot/media_service") {
            //unsigned char *snap_ptr = NULL;
            //int img_len = 0;
            //snap_ptr = media_snap_get(&img_len);

            //if (snap_ptr!=NULL)
            //    sendImageResponse(std::string((char *)snap_ptr, img_len));
            //if (snap_ptr) {
            //    free(snap_ptr);
            //    snap_ptr = NULL;
            // }
        } else {
            return ResponseSC(200, "");
        }
    } else {
        //TODO
        return ResponseSC(200, "");
    }
    return 0;
}

int HttpSession::ResponseSC(int sc, const std::string &strJson) {
    HttpResponse r;
    r.set("Access-Control-Allow-Origin", "*");
    r.set("Connection", "close");
    r.setStatus(sc);
    if (!strJson.empty())
        r.set("Content-Type", "application/json");
    r.setContentLength(strJson.size());
    auto header = r.toStr();
    if (strJson.empty())
        return sendBinaryData(_fd, header.c_str(), header.size());
    else {
        sendBinaryData(_fd, header.c_str(), header.size());
        return sendBinaryData(_fd, strJson.c_str(), strJson.size());
    }
}