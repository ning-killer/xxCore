/*
* @Author: xiong
* @Date: 2022/10/11
*/
#include "cstring"
#include "cstdio"
#include "EmxCore.hpp"
#include "OvdUtils.hpp"

using namespace Emx;

#ifndef CHECK_ENCRYDECRY_DATA
#define CHECK_ENCRYDECRY_DATA(data, data_len, ret) \
  do {                                             \
    if (data == nullptr || data_len <= 0) {        \
        emxloge("CHECK_ENCRYDECRY_DATA ERROR![%p:%d]\n", data, data_len); \
        return ret;                                \
    }                                              \
  } while (0)                                      
#endif

/**
 * 获取
 * @param key
 * @return
 */
static int getNetValueByKey(char *src, const char *key) {
    //RX packets:1325873 errors:0 dropped:272709 overruns:0 frame:0
    int ret = -1;
    char *pstr;
    char *p = strtok(src, " ");
    while (p) {
        if ((pstr = strstr(p, key))) {
            char findStr[32] = {0};
            sprintf(findStr, "%s:%%d", key);
            sscanf(p, findStr, &ret);
            break;
        }
        p = strtok(NULL, " ");
    }
    return ret;
}

/**
 * 获取设备丢包率  packetLossRate 公式为  (drop_rx+drop_tx)/(rx+tx)
 * @return
 */
int OvdUtils::getPktLossRate(const char *netDev) {
    /*
    wlan0     Link encap:Ethernet  HWaddr F0:85:C1:3A:90:22
              inet addr:192.168.31.77  Bcast:192.168.31.255  Mask:255.255.255.0
              UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
              RX packets:1325873 errors:0 dropped:272709 overruns:0 frame:0
              TX packets:4671 errors:0 dropped:2 overruns:0 carrier:0
              collisions:0 txqueuelen:1000
              RX bytes:152479345 (145.4 MiB)  TX bytes:551667 (538.7 KiB)

    */

    FILE *pp;
    char *pstr, *pStr2;
    char tmp[256] = {0};
    char tmp2[256] = {0};
    int ret;
    unsigned int rx = 0, tx = 0, drop_rx = 0, drop_tx = 0;
    sprintf(tmp, "ifconfig -a %s", netDev);
    pp = popen(tmp, "r");
    if (pp) {
        memset(tmp, 0, sizeof(tmp));
        while (fgets(tmp, 256, pp)) {
            strcpy(tmp2, tmp);

            //获取rx
            pstr = strstr(tmp, "RX packets");
            if (pstr) {
//                printf("[%s:%d] tmp:%s\n", __FUNCTION__ , __LINE__,tmp);
                rx = getNetValueByKey(tmp, "packets");

                //获取drop_rx
//                printf("[%s:%d] tmp:%s\n", __FUNCTION__ , __LINE__,tmp2);
                pStr2 = strstr(tmp2, "dropped");
                if (pStr2)
                    drop_rx = getNetValueByKey(tmp2, "dropped");
            }

            //获取tx
            pstr = strstr(tmp, "TX packets");
            if (pstr) {
//                printf("[%s:%d] tmp:%s\n", __FUNCTION__ , __LINE__,tmp);
                tx = getNetValueByKey(tmp, "packets");

                //获取drop_tx
//                printf("[%s:%d] tmp:%s\n", __FUNCTION__ , __LINE__,tmp2);
                pStr2 = strstr(tmp2, "dropped");
                drop_tx = getNetValueByKey(tmp2, "dropped");
            }
            memset(tmp, 0, sizeof(tmp));
            memset(tmp2, 0, sizeof(tmp2));
        }
        pclose(pp);
    }
//    printf("[%s:%d], tx:%d, tx_drop:%d, rx:%d, rx_drop:%d\n", __FUNCTION__ , __LINE__, tx, drop_tx, rx, drop_rx);
    unsigned int totalRTX = rx + tx;
    unsigned int totalDropRTX = drop_rx + drop_tx;
    ret = (int) (((double) totalDropRTX / totalRTX) * 100);
    return ret;
}


/**
 * 根据小时数获取秒数 19:00:01 ---> 68401
 * @param timestring
 * @return
 */
int64_t OvdUtils::GetSecondByTimeStr(char *timestring) {
    char hour[4] = {0};
    char min[4] = {0};
    char sec[4] = {0};

    int numHour, numMin, numSec;
    uint32_t timeSecond = 0;

    if (strlen(timestring) < 8) {
        emxloge("timestring length is < 8, %s\n", timestring);
        return -1;
    }
    strncpy(hour, timestring, 2);
    strncpy(min, timestring + 3, 2);
    strncpy(sec, timestring + 6, 2);

    numHour = atoi(hour);
    numMin = atoi(min);
    numSec = atoi(sec);

    timeSecond = numHour * 60 * 60 + numMin * 60 + numSec;
    return timeSecond;
}

int64_t OvdUtils::GetDayOffSecondByUtc(int64_t utc) {
    auto t = (time_t) utc;
    tm tm = {};
    localtime_r(&t, &tm);
    return tm.tm_hour * 60 * 60 + tm.tm_min * 60 + tm.tm_sec;
}

int64_t OvdUtils::GetDay0TimeUtcByUtc(int64_t utc) {
    auto t = (time_t) utc;
    tm tm = {};
    localtime_r(&t, &tm);
    tm.tm_sec = 0;
    tm.tm_min = 0;
    tm.tm_hour = 0;
    return mktime(&tm);
}

char *OvdUtils::GetDateString(char *buffer) {
    static const char *monthName[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    int yyyy = 0, MM = 0, dd = 0;
    char szMonth[4] = {0};
    sscanf(__DATE__, "%s %d %d", szMonth, &dd, &yyyy);
    for (int i = 0; i < 12; i++) {
        if (strcmp(szMonth, monthName[i]) == 0) {
            MM = i + 1;
            break;
        }
    }
    sprintf(buffer, "%d-%d", yyyy, MM);
    return buffer;
}

static size_t read_callback(void *ptr, size_t size, size_t number, void *stream) {
    size_t ret = 0;
    /* in real-world cases, this would probably get this data differently
       as this fread() stuff is exactly what the library already would do
       by default internally */
    if (ptr != NULL && stream != NULL) {
//        emxlogd("ptr: %s\n", (char *) ptr);
//        emxlogd("stream: %p\n", stream);
//        emxlogd("size: %u\n", size);
//        emxlogd("number: %d\n", number);
        ret = fread(ptr, size, number, (FILE *) stream);
    }
//    emxlogd("[%s] fread ret: %d\n", __FUNCTION__, ret);
    return ret;
}

static size_t callback_get_head_etag(void *ptr, size_t size, size_t number, void *stream) {
    size_t ret = number * size;
    if (ptr != nullptr && stream != nullptr && strstr((const char *) ptr, "ETag") != nullptr) {
        memcpy(stream, ptr, ret);
    }
    return ret; //必须返回number * size, 否则只回调一次
}

static void set_common_option(CURL *curl, const char *url, struct curl_slist *headers, int debug) {
    if (url != nullptr) {
        curl_easy_setopt(curl, CURLOPT_URL, url);               //设置url地址
    }

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 60);
    curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 50);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 60);
    curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, 60);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);      //设定为不验证证书和HOST
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
//    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);      //设定为不验证证书和HOST
//    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
//    curl_easy_setopt(curl, CURLOPT_CAINFO, "/mnt/sdcard/cacert.pem");

    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

    if (headers != nullptr) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    // curl_easy_setopt(curl, CURLOPT_VERBOSE, debug);
    //curl_easy_setopt(curl, CURLOPT_SSLVERSION, 5);
}


int OvdUtils::https_put_func_with_headers(const char *file_name, const char *url, char *out_data,
                                          struct ::curl_slist *headers,
                                          long *http_code) {
    FILE *fp = fopen(file_name, "rb");
    if (fp == nullptr) {
        emxloge("fopen %s failed\n", file_name);
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (file_size <= 0) {
        emxloge("file_name %s file_size is: %d\n", file_name, file_size);
        fclose(fp);
        return -1;
    }


    CURL *curl = curl_easy_init();
    if (curl == nullptr) {
        emxloge("curl_easy_init failed\n");
        fclose(fp);
        return -1;
    }

    /* we want to use our own read function */
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
    /* enable uploading */
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    /* HTTP PUT please */
    curl_easy_setopt(curl, CURLOPT_PUT, 1L);

    /* now specify which file to upload */
    curl_easy_setopt(curl, CURLOPT_READDATA, fp);
    /* provide the size of the upload, we specicially typecast the value
       to curl_off_t since we must be sure to use the correct data size */
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t) file_size);

    if (headers != nullptr) {
        char content_length[32] = {0};
        snprintf(content_length, sizeof(content_length), "Content-Length: %ld", file_size);
        emxlogd("content_length: %s\n", content_length);
        headers = curl_slist_append(headers, content_length);
    }

    set_common_option(curl, url, headers, 0);

    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, callback_get_head_etag); //头部数据的回调函数
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, out_data);//头部数据的指针

    curl_easy_setopt(curl, CURLOPT_INFILESIZE, file_size);


    CURLcode res = curl_easy_perform(curl);

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, http_code);

    emxlogd("http_code: %ld\n", *http_code);

    curl_easy_cleanup(curl);
    fclose(fp);

    if (res != CURLE_OK) {
        emxloge("curl_easy_perform failed: %s\n", curl_easy_strerror(res));
    }

    return res;
}

static size_t download_callback(void *ptr, size_t size, size_t number, void *stream) {
    size_t ret = 0;
    if (ptr != nullptr && stream != nullptr) {
//        emxlogd("ptr: %s\n", (char *) ptr);
//        emxlogd("stream: %p\n", stream);
//        emxlogd("size: %u\n", size);
//        emxlogd("number: %d\n", number);
        ret = fwrite(ptr, size, number, (FILE*)stream);
    }
//    emxlogd("[%s] fread ret: %d\n", __FUNCTION__, ret);
    return ret;
}

int OvdUtils::https_download_file_func(const char *url, const char *file_name, long *http_code) {
    FILE *fd = fopen(file_name, "wb+");
    if (fd == nullptr) {
        emxloge("fopen %s failed\n", file_name);
        return -1;
    }

    CURL *curl = curl_easy_init();
    if (curl == nullptr) {
        emxloge("curl_easy_init failed\n");
        return -1;
    }

    set_common_option(curl, url, nullptr, 0);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, download_callback);      //设置回调函数
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) fd);        //设置写数据
    curl_easy_setopt(curl, CURLOPT_POST, 0);

    CURLcode res = curl_easy_perform(curl);
    fflush(fd);
    fsync(fileno(fd));
    fclose(fd);

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, http_code);
    emxlogd("<==============================================\n");
    emxlogd("url: %s\n", url);
    emxlogd("res: %d\n", res);
    emxlogd("http_code: %ld\n", *http_code);
    emxlogd("==============================================>\n");
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) {
        emxloge("curl_easy_perform failed: %s\n", curl_easy_strerror(res));
    }
    return res;
}
//
//Log::LevelE OvdUtils::OvdLevelToEapilLevel(OVDLogLevel level) {
//    switch (level) {
//        case OVD_LOGLEVEL_TRACE:
//            return Log::LevelE::TRACE;
//            break;
//        case OVD_LOGLEVEL_DEBUG:
//            return Log::LevelE::DEBUG;
//            break;
//        case OVD_LOGLEVEL_INFO:
//            return Log::LevelE::INFO;
//            break;
//        case OVD_LOGLEVEL_WARN:
//            return Log::LevelE::WARN;
//            break;
//        case OVD_LOGLEVEL_ERROR:
//            return Log::LevelE::ERR;
//            break;
//        case OVD_LOGLEVEL_FATAL:
//            return Log::LevelE::CRT;
//            break;
//        default:
//            return Log::LevelE::DEBUG;
//    }
//}

std::mutex OvdUtils::m_uptime_locker;
std::mutex OvdUtils::m_encrydecry_locker;
char OvdUtils::m_key[EncryDecry_KeySize + 1] = "eapil.com-123456789-123456789-123456789-123456789-123456789-123456789-123456789-123456789-123456789-123456789-123456789-12345678";

unsigned long OvdUtils::GetUptime() {
    std::lock_guard<std::mutex> locker(m_uptime_locker);
    double uptime = 0;
    FILE *fp = fopen("/proc/uptime", "r");
    if (fp != nullptr) {
        fscanf(fp, "%lf", &uptime);
        fclose(fp);
        fp = nullptr;
    }
    return (unsigned long)uptime;
}

bool OvdUtils::EncryDecry(unsigned char *data, const int data_len) {
    CHECK_ENCRYDECRY_DATA(data, data_len, false);
    for (int i = 0; i < data_len; i++) {
        int key = i % EncryDecry_KeySize;
        data[i] = data[i] ^ m_key[key];
    }
    return true;
}

bool OvdUtils::EncryDecryMp4Video(unsigned char *data, const int data_len, bool iframe) {
    std::lock_guard<std::mutex> locker(m_encrydecry_locker);
    CHECK_ENCRYDECRY_DATA(data, data_len, false);
    bool is_ok = false;
    if (iframe) {
        const unsigned char ly_startcode4[4] = { 0, 0, 0, 1 };
        int i = 0, en_len = 0, last_frame_start = 5;
        int s1 = memcmp(data, ly_startcode4, 4) == 0;
        int startcode_len = s1 ? 4 : 3;
        last_frame_start = startcode_len + 1;
        unsigned char *pstr_frame = data;
        for (int i = startcode_len; i < data_len; i++){
            if (pstr_frame[i] == 0x0 && pstr_frame[i+1] == 0x0) {
                if ((startcode_len == 3 && pstr_frame[i+2] == 0x1) 
                    || (pstr_frame[i+2] == 0x0 && pstr_frame[i+3] == 0x1)){
                    en_len = i - last_frame_start;
                    if (en_len > 0) {
                        is_ok = EncryDecry(&pstr_frame[last_frame_start], en_len);
                    }
                    i += startcode_len;
                    last_frame_start = i + 1;
                }
            }
        }
        en_len = i - last_frame_start;
        if (en_len > 0) {
            is_ok = EncryDecry(&pstr_frame[last_frame_start], en_len);
        }
    } else {
        is_ok = EncryDecry(data + 5, data_len);
    }

    return is_ok;
}

bool OvdUtils::EncryDecryMp4Audio(unsigned char *data, const int data_len) {
    std::lock_guard<std::mutex> locker(m_encrydecry_locker);
    CHECK_ENCRYDECRY_DATA(data, data_len, false);
    return EncryDecry(data, data_len);
}