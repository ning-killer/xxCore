/*
* @Author: xiong
* @Date: 2022/10/11
*/

#ifndef EAPIL_OvdUtils_HPP
#define EAPIL_OvdUtils_HPP

#include "OVD_define.h"
#include "curl.h"
#include "EmxCore.hpp"
#include <mutex>

#define EncryDecry_KeySize 128
namespace Emx {

    class OvdUtils {
      public:
        static int getPktLossRate(const char *netDev);

        static int64_t GetSecondByTimeStr(char *timestring);

        static int64_t GetDayOffSecondByUtc(int64_t utc);

        static int64_t GetDay0TimeUtcByUtc(int64_t utc);

        static int TimeZoneToNum(const char *zone) {
            return 0 - strtol(zone + 4, nullptr, 10);
        }

        static char *NumToTimeZone(int num, char *buffer) {
            sprintf(buffer, "GMT%c%d", num >= 0 ? '-' : '+', abs(num));
            return buffer;
        }

        /**
         * 获取时间字符串
         * @return 2021-09
         */
        static char *GetDateString(char *buffer);

        static int https_put_func_with_headers(const char *file_name, const char *url, char *out_data,
                                               struct ::curl_slist *headers, long *http_code);

        static int https_put_func(const char *file_name, const char *url, char *out_data, long *http_code) {
            return OvdUtils::https_put_func_with_headers(file_name, url, out_data, nullptr, http_code);
        }

        static int https_download_file_func(const char *url, const char *file_name, long *http_code);

        /**
         * @brief 获取系统运行时间。
         * @return 运行时间 unsigned long 单位：秒
         */
        static unsigned long GetUptime();

        /**
         * @brief mp4 video 加解密。
         * @param data 视频帧数据。
         * @param data_len 视频帧数据长度。
         * @param iframe 关键帧。false: 非关键帧; true: 关键帧
         * @return 加解密结果
         */
        static bool EncryDecryMp4Video(unsigned char *data, const int data_len, bool iframe);
        
        /**
         * @brief mp4 audio 加解密。
         * @param data 视频帧数据。
         * @param data_len 视频帧数据长度。
         * @return 加解密结果
         */
        static bool EncryDecryMp4Audio(unsigned char *data, const int data_len);

      private:
        static bool EncryDecry(unsigned char *data, const int data_len);

      private:
        static std::mutex m_uptime_locker;
        static std::mutex m_encrydecry_locker;
        static char m_key[EncryDecry_KeySize + 1];
    };
}

#endif //EAPIL_OvdUtils_HPP
