#include "RtspSession.h"
#include <iostream>
#include "sockutil.h"
#include "EventPoller.h"
#include "MediaSource.h"
#include "MediaSourceManager.h"
#include "DigestAuth.h"
#include "common.h"
#include "MediaSource.h"
#include "MediaSourceManager.h"
//#include "spdlog/spdlog.h"
#include "EmxCore.hpp"
#ifdef _WIN32
#pragma warning(disable:4996)
#define strncasecmp _strnicmp
#else
#include <string.h>
#endif

bool parseRangeParam(char const* paramStr, double& rangeStart, double& rangeEnd, char*& absStartTime,
                     char*& absEndTime, bool& startTimeIsNow)
{
    delete[] absStartTime; delete[] absEndTime;
    absStartTime = absEndTime = NULL; // by default, unless "paramStr" is a "clock=..." string
    startTimeIsNow = false; // by default
    double start, end;
    int numCharsMatched1 = 0, numCharsMatched2 = 0, numCharsMatched3 = 0, numCharsMatched4 = 0;
    int startHour = 0, startMin = 0, endHour = 0, endMin = 0;
    double startSec = 0.0, endSec = 0.0;
    //Locale l("C", Numeric);
    if (sscanf(paramStr, "npt = %d:%d:%lf - %d:%d:%lf", &startHour, &startMin, &startSec, &endHour, &endMin, &endSec) == 6)
    {
        rangeStart = startHour * 3600 + startMin * 60 + startSec;
        rangeEnd = endHour * 3600 + endMin * 60 + endSec;
    }
    else if (sscanf(paramStr, "npt =%lf - %d:%d:%lf", &start, &endHour, &endMin, &endSec) == 4)
    {
        rangeStart = start;
        rangeEnd = endHour * 3600 + endMin * 60 + endSec;
    }
    else if (sscanf(paramStr, "npt = %d:%d:%lf -", &startHour, &startMin, &startSec) == 3)
    {
        rangeStart = startHour * 3600 + startMin * 60 + startSec;
        rangeEnd = 0.0;
    }
    else if (sscanf(paramStr, "npt = %lf - %lf", &start, &end) == 2)
    {
        rangeStart = start;
        rangeEnd = end;
    }
    else if (sscanf(paramStr, "npt = %n%lf -", &numCharsMatched1, &start) == 1)
    {
        if (paramStr[numCharsMatched1] == '-')
        {
            // special case for "npt = -<endtime>", which matches here:
            rangeStart = 0.0; startTimeIsNow = true;
            rangeEnd = -start;
        }
        else
        {
            rangeStart = start;
            rangeEnd = 0.0;
        }
    }
    else if (sscanf(paramStr, "npt = now - %lf", &end) == 1)
    {
        rangeStart = 0.0; startTimeIsNow = true;
        rangeEnd = end;
    }
    else if (sscanf(paramStr, "npt = now -%n", &numCharsMatched2) == 0 && numCharsMatched2 > 0)
    {
        rangeStart = 0.0; startTimeIsNow = true;
        rangeEnd = 0.0;
    }
    else if (sscanf(paramStr, "clock = %n", &numCharsMatched3) == 0 && numCharsMatched3 > 0)
    {
        rangeStart = rangeEnd = 0.0;
        char const* utcTimes = &paramStr[numCharsMatched3];
        size_t len = strlen(utcTimes) + 1;
        char* as = new char[len];
        char* ae = new char[len];
        int sscanfResult = sscanf(utcTimes, "%[^-]-%[^\r\n]", as, ae);
        if (sscanfResult == 2)
        {
            absStartTime = as;
            absEndTime = ae;
        }
        else if (sscanfResult == 1)
        {
            absStartTime = as;
            delete[] ae;
        }
        else
        {
            delete[] as; delete[] ae;
            return false;
        }
    }
    else if (sscanf(paramStr, "smtpe = %n", &numCharsMatched4) == 0 && numCharsMatched4 > 0)
    {
        // We accept "smtpe=" parameters, but currently do not interpret them.
    }
    else
    {
        return false; // The header is malformed
    }
    return true;
}

RtspSession::RtspSession(int fd, EventPoller::Ptr poller) :TcpSession(fd, poller)
{
    udpBuf = new char[1500];
    m_curTime = time(NULL);
    responseBuf = new char[1024];
    emxlogd("construct rtspsession %p\n", this);
}

RtspSession::~RtspSession()
{
    if (_streamMode == "UDP")
    {
        for (size_t i = 0; i < 2; i++)
        {
            if (trackSock[i].first != -1)
            {
                _poller->delEvent(trackSock[i].first, [](bool) {});
                close(trackSock[i].first);
            }
            if (trackSock[i].second != -1)
            {
                _poller->delEvent(trackSock[i].second, [](bool) {});
                close(trackSock[i].second);
            }
        }
    }

    delete[] udpBuf;
    delete[] responseBuf;
    emxlogd("deconstruct rtspsession %p\n", this);
}

int RtspSession::handlePkt()
{
    m_curTime = time(NULL);

    auto ptr = (uint8_t*)_buffer.c_str();
    if (ptr[0] == 0x24)
        return handleRtp();

    if (handleRtsp())
        return 1;
    return 0;
}

void RtspSession::handleCmd_OPTIONS(const std::string& cseq)
{
    auto sz = sprintf(responseBuf,
                      "RTSP/1.0 200 OK\r\nCSeq: %s\r\n"
                      "%s"
                      "Public: %s\r\n\r\n",
                      cseq.c_str(),
                      dateHeader("Date", time(NULL)), "OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY");

    sendBinaryData(_fd, responseBuf, sz, 1);
}

void RtspSession::handleCmd_DESCRIBE(std::shared_ptr<HttpRequest>& hd)
{
    if (!CheckAuth(hd))
    {
        auto sz = sprintf(responseBuf,
                          "RTSP/1.0 401 Unauthorized\r\nCSeq: %s\r\n"
                          "%s"
                          "\r\n",
                          hd->get("CSeq").c_str(), DigestAuth::Instance().createWWWAuth(1).c_str());
        sendBinaryData(_fd, responseBuf, sz, 1);
        emxloge("RTSP Not passed identity verification\n");
        return;
    }

    bool findSrc = false;
    auto pos = hd->Url().rfind('/');
    if (pos != hd->Url().npos)
    {
        std::string app = hd->Url().substr(pos + 1);
        //std::string app = "main";
        _src = MediaSourceManager::Instance().searchMedia(app);
        if (_src)
            findSrc = true;
    }
    if (!findSrc)
    {
        auto sz = sprintf(responseBuf,
                          "RTSP/1.0 404 Stream Not Found\r\nCSeq: %s\r\n"
                          "%s"
                          "\r\n",
                          hd->get("CSeq").c_str(), dateHeader("Date", time(NULL)));
        sendBinaryData(_fd, responseBuf, sz, 1);
        return;
    }

    std::string contentBase;
    if (hd->Url().back() != '/')
    {
        m_baseUrl = hd->Url() + "/";
        contentBase = "Content-Base: " + m_baseUrl + "\r\n";
    }

    std::string sdp = _src->getSDP();

    auto sz = sprintf(responseBuf,
                      "RTSP/1.0 200 OK\r\nCSeq: %s\r\n"
                      "%s"
                      "%s"
                      "Content-Length: %d\r\n"
                      "Content-Type: application/sdp\r\n\r\n%s",
                      hd->get("CSeq").c_str(), dateHeader("Date", time(NULL)), contentBase.c_str(), (int)sdp.size(), sdp.c_str());

    sendBinaryData(_fd, responseBuf, sz, 1);
}

void RtspSession::handleCmd_SETUP(std::shared_ptr<HttpRequest>& hd)
{
    if (!CheckAuth(hd))
    {
        auto sz = sprintf(responseBuf,
                          "RTSP/1.0 401 Unauthorized\r\nCSeq: %s\r\n"
                          "%s"
                          "\r\n",
                          hd->get("CSeq").c_str(), DigestAuth::Instance().createWWWAuth(1).c_str());
        sendBinaryData(_fd, responseBuf, sz, 1);
        emxloge("RTSP Not passed identity verification\n");
        return;
    }

    if (!_src)
    {
        auto sz = sprintf(responseBuf,
                          "RTSP/1.0 404 Stream Not Found\r\nCSeq: %s\r\n"
                          "%s"
                          "\r\n",
                          hd->get("CSeq").c_str(), dateHeader("Date", time(NULL)));
        sendBinaryData(_fd, responseBuf, sz, 1);
        return;
    }

    if (strcmp(sessionID, "") == 0)
        sprintf(sessionID, "%08d", randInt());

    auto pos = hd->Url().rfind('/');
    auto controlPath = hd->Url().substr(pos + 1);

    if (controlPath == "track=0")
    {
        _index = 0;
        _enableMedia = 0x01;
    }
    else if (controlPath == "track=1")
    {
        _index = 1;
        _enableMedia = 0x11;
    }
    else
    {
        _poller->delEvent(_fd, [](bool) {});
        _errCB();
        std::cout << "not found track" << std::endl;
        return;
    }

    int sz = 0;
    hd->parseTransport();
    if (hd->_streamMode == "TCP")
    {
        _streamMode = "TCP";
        sz = sprintf(responseBuf,
                     "RTSP/1.0 200 OK\r\nCSeq: %s\r\n"
                     "%s"
                     "Transport: RTP/AVP/TCP;unicast;interleaved=%hu-%hu;\r\n"
                     "Session: %s\r\n\r\n",
                     hd->get("CSeq").c_str(),
                     dateHeader("Date", time(NULL)), hd->iRtp, hd->iRtcp, sessionID);
    }
    else//UDP
    {
        _streamMode = "UDP";
        std::string localIp = SockUtil::get_local_ip(_fd);
        std::string peerIp = SockUtil::get_peer_ip(_fd);
        int rtpSock = SockUtil::bindUdpSock(0, localIp.c_str());
        if (rtpSock == -1)
        {
            _poller->delEvent(_fd, [](bool) {});
            _errCB();
            return;
        }
        auto rtpPort = SockUtil::get_local_port(rtpSock);

        int rtcpSock = SockUtil::bindUdpSock(rtpPort + 1, localIp.c_str());
        if (rtcpSock == -1)
        {
            _poller->delEvent(_fd, [](bool) {});
            _errCB();
            return;
        }

        trackSock[_index].first = rtpSock;
        trackSock[_index].second = rtcpSock;

        trackAddr[_index].first.sin_family = AF_INET;
        trackAddr[_index].first.sin_port = htons(hd->rtpPort);
        inet_pton(AF_INET, peerIp.c_str(), &trackAddr[_index].first.sin_addr);

        trackAddr[_index].second.sin_family = AF_INET;
        trackAddr[_index].second.sin_port = htons(hd->rtcpPort);
        inet_pton(AF_INET, peerIp.c_str(), &trackAddr[_index].second.sin_addr);

        int i = _index;
        std::weak_ptr<RtspSession> weakRef = std::dynamic_pointer_cast<RtspSession>(shared_from_this());
        _poller->addEvent(rtpSock, [weakRef, rtpSock, i]()
        {
            auto strongRef = weakRef.lock();
            if (strongRef)
                strongRef->OnRecvUdpData(rtpSock, i);
        });
        i += 1;
        _poller->addEvent(rtcpSock, [weakRef, rtcpSock, i]()
        {
            auto strongRef = weakRef.lock();
            if (strongRef)
                strongRef->OnRecvUdpData(rtcpSock, i);
        });

        sz = sprintf(responseBuf,
                     "RTSP/1.0 200 OK\r\nCSeq: %s\r\n"
                     "%s"
                     "Transport: RTP/AVP;unicast;destination=%s;source=%s;client_port=%hu-%hu;server_port=%hu-%hu\r\n"
                     "Session: %s\r\n\r\n",
                     hd->get("CSeq").c_str(),
                     dateHeader("Date", time(NULL)), peerIp.c_str(), localIp.c_str(), hd->rtpPort, hd->rtcpPort, rtpPort, rtpPort + 1,
                     sessionID);
    }

    sendBinaryData(_fd, responseBuf, sz, 1);
}

void RtspSession::handleCmd_PLAY(std::shared_ptr<HttpRequest>& hd)
{
    if (!CheckAuth(hd))
    {
        auto sz = sprintf(responseBuf,
                          "RTSP/1.0 401 Unauthorized\r\nCSeq: %s\r\n"
                          "%s"
                          "\r\n",
                          hd->get("CSeq").c_str(), DigestAuth::Instance().createWWWAuth(1).c_str());
        sendBinaryData(_fd, responseBuf, sz, 1);
        emxloge("RTSP Not passed identity verification\n");
        return;
    }

    if (_isReplay)
    {
        handleReplay(hd);
        return;
    }

    auto sz = sprintf(responseBuf,
                      "RTSP/1.0 200 OK\r\nCSeq: %s\r\n"
                      "%s"
                      "Range: npt=0.000-\r\n"
                      "RTP-Info: url=%s\r\n"
                      "Session: %s\r\n\r\n",
                      hd->get("CSeq").c_str(),
                      dateHeader("Date", time(NULL)), hd->Url().c_str(), sessionID);

    SockUtil::setSendBuf(_fd, 500 * 1024);
    SockUtil::setNoblock(_fd, true);
    SockUtil::setNoDelay(_fd, false);

    sendBinaryData(_fd, responseBuf, sz, 1);

    if (_src)
    {
        std::weak_ptr<RtspSession> weakRef = std::dynamic_pointer_cast<RtspSession>(shared_from_this());
        _poller->async([&]()
                       {
                           _src->bbc(_poller, [weakRef](const RTPFrame::Ptr& k)
                           {
                               auto strongRef = weakRef.lock();
                               if (strongRef)
                               {
                                   if(!strongRef->_sendKeyFrame) {
                                       if (!k->_isKey)
                                           return 0;
                                       else
                                           strongRef->_sendKeyFrame = true;
                                   }
//					int ret;

                                   for (auto& item : k->_pktCache)
                                   {
                                       if (strongRef->_streamMode == "UDP")
                                       {
                                           if (item->data()[1] == 0x00 && ((strongRef->_enableMedia & 0x01) == 0x01))
                                           {
                                               ::sendto(strongRef->trackSock[0].first, item->data() + 4, item->size() - 4,
                                                        MSG_NOSIGNAL,
                                                        (sockaddr*)&strongRef->trackAddr[0].first,
                                                        sizeof(sockaddr_in));
                                           }
                                           else if (item->data()[1] == 0x02 && ((strongRef->_enableMedia & 0x10) == 0x10))
                                           {
                                               ::sendto(strongRef->trackSock[1].first, item->data() + 4, item->size() - 4,
                                                        MSG_NOSIGNAL,
                                                        (sockaddr*)&strongRef->trackAddr[1].first,
                                                        sizeof(sockaddr_in));
                                           }
                                       }
                                       else
                                       {
                                           if (item->data()[1] == 0x00 && ((strongRef->_enableMedia & 0x01) == 0x01))
                                           {
                                               if (sendBinaryData(strongRef->_fd, item->data(), item->size(), 1))
                                                   return 1;
                                           }
                                           else if (item->data()[1] == 0x02 && ((strongRef->_enableMedia & 0x10) == 0x10))
                                           {
                                               if (sendBinaryData(strongRef->_fd, item->data(), item->size(), 1))
                                                   return 1;
                                           }
                                       }
                                   }
                                   return 0;
                               }
                               return 1;
                           });
                       });

    }
}

void RtspSession::handleCmd_TEARDOWN(std::shared_ptr<HttpRequest>& hd)
{
    auto sz = sprintf(responseBuf,
                      "RTSP/1.0 200 OK\r\nCSeq: %s\r\n"
                      "%s"
                      "Session: %s\r\n\r\n",
                      hd->get("CSeq").c_str(),
                      dateHeader("Date", time(NULL)), sessionID);

    sendBinaryData(_fd, responseBuf, sz, 1);
}

void RtspSession::handleCmd_PAUSE(std::shared_ptr<HttpRequest>& hd)
{
    auto sz = sprintf(responseBuf,
                      "RTSP/1.0 200 OK\r\nCSeq: %s\r\n"
                      "%s"
                      "Session: %s\r\n\r\n",
                      hd->get("CSeq").c_str(),
                      dateHeader("Date", time(NULL)), sessionID);
    sendBinaryData(_fd, responseBuf, sz, 1);
}

void RtspSession::handleCmd_bad(const std::string& cseq)
{
    auto sz = sprintf(responseBuf,
                      "RTSP/1.0 400 Bad Request\r\nCSeq: %s\r\n"
                      "%s\r\n",
                      cseq.c_str(),
                      dateHeader("Date", time(NULL)));

    sendBinaryData(_fd, responseBuf, sz, 1);
}

void RtspSession::handleCmd_notFound(const std::string& cseq)
{
    auto sz = sprintf(responseBuf,
                      "RTSP/1.0 404 Not Found\r\nCSeq: %s\r\n"
                      "%s\r\n",
                      cseq.c_str(),
                      dateHeader("Date", time(NULL)));

    sendBinaryData(_fd, responseBuf, sz, 1);
}

void RtspSession::handleCmd_sessionNotFound(const std::string& cseq)
{
    auto sz = sprintf(responseBuf,
                      "RTSP/1.0 454 Session Not Found\r\nCSeq: %s\r\n"
                      "%s\r\n",
                      cseq.c_str(),
                      dateHeader("Date", time(NULL)));

    sendBinaryData(_fd, responseBuf, sz, 1);
}

int RtspSession::handleReplay(std::shared_ptr<HttpRequest>& hd)
{
//	int respCode = 200;
//	char scaleHeader[100] = { 0 };
//	char rangeHeader[100] = { 0 };
    //do
    //{
    //	float sc;
    //	auto strScale = hd.kv.find("Scale");
    //	if (it1 != hd.kv.end())
    //	{
    //		if (sscanf(it1->second.c_str(), "%f", &sc) == 1)
    //		{
    //			_scale = sc;
    //			snprintf(scaleHeader, 100, "Scale: %f\r\n", _scale);
    //		}
    //		else
    //		{
    //			respCode = 400;
    //			break; // The header is malformed
    //		}
    //	}
    //
    //	auto it2 = hd.kv.find("Range");
    //	if (it2 != hd.kv.end())
    //	{
    //		double rangeStart = 0.0, rangeEnd = 0.0;
    //		char* absStart = NULL; char* absEnd = NULL;
    //		bool startTimeIsNow;

    //		if (parseRangeParam(it2->second.c_str(), rangeStart, rangeEnd, absStart, absEnd, startTimeIsNow))
    //		{
    //			if (absStart)
    //			{
    //				int year, month, day, hour, minute, second, millisecond;
    //				if (sscanf(absStart, "%4d%2d%2dT%2d%2d%2d.%2dZ", &year, &month, &day, &hour, &minute, &second, &millisecond) == 7)
    //				{

    //				}

    //				if (absEnd && sscanf(absEnd, "%4d%2d%2dT%2d%2d%2d.%2dZ", &year, &month, &day, &hour, &minute, &second, &millisecond) == 7)
    //				{

    //				}
    //				if (absEnd == NULL)
    //					snprintf(rangeHeader, 100, "Range: clock=%s-\r\n", absStart);
    //				else
    //					snprintf(rangeHeader, 100, "Range: clock=%s-%s\r\n", absStart, absEnd);
    //			}
    //			else
    //			{
    //				if (rangeEnd == 0.0)
    //					snprintf(rangeHeader, 100, "Range: npt=%.3f-\r\n", rangeStart);
    //				else
    //					snprintf(rangeHeader, 100, "Range: npt=%.3f-%.3f\r\n", rangeStart, rangeEnd);
    //			}
    //		}
    //		respCode = 400;
    //	}
    //} while (0);

    //int sz = 0;
    //switch (respCode)
    //{
    //case 200:
    //{
    //	sz = sprintf(responseBuf,
    //		"RTSP/1.0 200 OK\r\nCSeq: %s\r\n"
    //		"%s"
    //		"%s"
    //		"%s"
    //		"Session: %s\r\n\r\n",
    //		hd.kv["CSeq"].c_str(),
    //		dateHeader("Date", time(NULL)), scaleHeader, rangeHeader, sessionID);
    //}break;
    //case 400:
    //{
    //	sz = sprintf(responseBuf, "RTSP/1.0 400 Bad Request\r\nCSeq: %s\r\n"
    //		"Session: %s\r\n\r\n", hd.kv["CSeq"].c_str(), sessionID);
    //}break;
    //}
    //
    //sendResponse(sz);
    return 0;
}

int RtspSession::handleRtsp()
{
    if (_recvStatus == 0)
    {
        auto ptr = strstr(_buffer.c_str(), "\r\n\r\n");
        if (ptr == nullptr)
            return 0;
        else
        {
            _recvStatus = 1;
            ptr += 4;
            auto headerSize = ptr - _buffer.c_str();
            std::string hd(_buffer.c_str(), headerSize);
            //printf("%s\n", hd.c_str());

            _req.reset(new HttpRequest);
            _req->Parse(hd);

            _contentLength = _req->ContentLength();


            emxlogi("%s\n", hd.c_str());

            _buffer.erase(0, headerSize);
            if (_contentLength > 0)
            {
                _body.append(_buffer);
                _buffer.clear();
                if (_body.size() < _contentLength)
                    return 0;
            }
            _recvStatus = 2;
        }
    }

    if (_recvStatus == 1)
    {
        _body.append(_buffer);
        _buffer.clear();
        if (_body.size() < _contentLength)
            return 0;
        _recvStatus = 2;
    }
    int ret = 0;
    if (_recvStatus == 2)
    {
        if (strcmp(_req->_method.c_str(), "OPTIONS") == 0)
            handleCmd_OPTIONS(_req->get("CSeq"));
        else if (strcmp(_req->_method.c_str(), "DESCRIBE") == 0)
            handleCmd_DESCRIBE(_req);
        else if (strcmp(_req->_method.c_str(), "SETUP") == 0)
            handleCmd_SETUP(_req);
        else if (strcmp(_req->_method.c_str(), "PLAY") == 0)
            handleCmd_PLAY(_req);
        else if (strcmp(_req->_method.c_str(), "TEARDOWN") == 0)
        {
            handleCmd_TEARDOWN(_req);
            ret = 1;
        }
        else if (strcmp(_req->_method.c_str(), "PAUSE") == 0)
            handleCmd_PAUSE(_req);
        else
        {

        }

        _body.clear();
        //delete _req;
        //_req = nullptr;
        _recvStatus = 0;
    }
    return ret;
}

int RtspSession::handleRtp()
{
    auto ptr = (uint8_t*)_buffer.c_str();
    if (ptr[0] != 0x24)//may be a rtsp request
        return handleRtsp();

    if (_buffer.size() <= 12)//��������
        return 0;

    int channel = ptr[1];
    uint8_t a = ptr[2];
    uint8_t b = ptr[3];
    unsigned short sz = (a << 8) | b;
    if (sz + 4 > (int)_buffer.size())//��������
        return 0;

    switch (channel)
    {
        case 0://video rtp
        {
            //_src->handleOneRtp(recvBuf + 4, sz);
        }break;
        case 1://video rtcp
        {

        }break;
        case 2:
        {
        }break;
        case 3:
        {
        }break;
    }

    _buffer.erase(0, sz + 4);
    return handleRtp();
}

void RtspSession::OnRecvUdpData(int fd, int interval)
{
    sockaddr peerAddr;
    socklen_t len = sizeof(struct sockaddr);
    auto nRead = recvfrom(fd, udpBuf, 1500, 0, &peerAddr, &len);
    if (nRead == -1)
    {
        std::cout << "rtsp udp read failed" << std::endl;
        return;
    }
    std::cout << "recv rtcp data" << std::endl;
}

bool RtspSession::CheckAuth(std::shared_ptr<HttpRequest>& hd)
{
    auto strAuth = hd->get("Authorization");
    if (!strAuth.empty())
    {
        AuthorizationInfo info;
        /*realm*/
        auto p1 = strAuth.find("realm=");
        if (p1 == strAuth.npos)
            return false;
        auto p2 = strAuth.find('"', p1 + 7);
        info.realm = strAuth.substr(p1 + 7, p2 - p1 - 7);
        /*uri*/
        p1 = strAuth.find("uri=");
        if (p1 == strAuth.npos)
            return false;
        p2 = strAuth.find('"', p1 + 5);
        info.uri = strAuth.substr(p1 + 5, p2 - p1 - 5);
        /*nonce*/
        p1 = strAuth.find("nonce=");
        if (p1 == strAuth.npos)
            return false;
        p2 = strAuth.find('"', p1 + 7);
        info.nonce = strAuth.substr(p1 + 7, p2 - p1 - 7);
        /*response*/
        p1 = strAuth.find("response=");
        if (p1 == strAuth.npos)
            return false;
        p2 = strAuth.find('"', p1 + 10);
        info.response = strAuth.substr(p1 + 10, p2 - p1 - 10);
        /*nc*/
        p1 = strAuth.find("nc=");
        if (p1 != strAuth.npos)
        {
            p2 = strAuth.find(',', p1 + 3);
            info.nc = strAuth.substr(p1 + 3, p2 - p1 - 3);
        }
        /*cnonce*/
        p1 = strAuth.find("cnonce=");
        if (p1 != strAuth.npos)
        {
            p2 = strAuth.find('"', p1 + 8);
            info.cnonce = strAuth.substr(p1 + 8, p2 - p1 - 8);
        }
        /*qop*/
        p1 = strAuth.find("qop=");
        if (p1 != strAuth.npos)
        {
            do
            {
                p2 = strAuth.find("auth-int", p1 + 4);
                if (p2 != strAuth.npos)
                {
                    info.qop = "auth-int";
                    break;
                }
                p2 = strAuth.find("auth", p1 + 4);
                if (p2 != strAuth.npos)
                    info.qop = "auth";
            } while (0);
        }
        info.method = hd->_method;
        return DigestAuth::Instance().VerifyResponse(info);
    }
    emxloge("Auth is NULL!\n");
    return false;
}
