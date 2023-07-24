/*
* @Author: xiong
* @Date: 2023/2/7
*/

#ifndef EMX_RecordStd_HPP
#define EMX_RecordStd_HPP

#include "EmxCore.hpp"
#include "Schedule.hpp"
#include "Event.hpp"

namespace Emx {

    class RecordStd {
    public:

        struct RecFile {
            uint32_t startUtc;
            uint32_t endUtc;
            int32_t size;
            int32_t date;
            int32_t time;
        };

        struct Param {
        public:
            void Serialize(std::string &out);

            void Parse(const char *data, int size);

            void Serialize(Json::Value &json);

            void Parse(const Json::Value &json);

        public:
            char path[EMX_MAX_PATH_SIZE]; //!<录像文件存储顶层目录路径
            int reservedSpace; //!<保留的存储空间大小，当小于此空间时，定时类操作会进行空间释放，非定时类录制操作将无法进行
            enum class ModeE : uint8_t {
                Schedule = 0, //!<计划模式
                Manual, //!<手动模式
                Event, //!<事件触发模式
            };
            //! 计划配置信息
            struct ScheduleBase {
                Schedule::Param schedule;
            };
            //! 监听事件类型和通道号
            struct EventBaseItem {
                Event::TypeE type;
                int chn;
            };
            struct Record {
                int vencChn; //!<录像使用的视频编码通道号
                int aencChn; //!<录像使用的音频编码通道号
                ModeE mode; //!<录像模式
                int splitTimeS; //!<录像分割时间，取值范围60-300单位秒
                ScheduleBase schedule; //!<计划录像的配置信息
                int preRecTimeS; //!<事件录像预录时间，取值范围3-8单位秒
                std::vector<EventBaseItem> events; //!<事件录像的配置信息
            } record;

            struct Snap {
                int chn; //!<抓图使用的视频编码通道号
                ModeE mode; //!<抓图模式
                int snapInvS; //!<计划抓图间隔
                ScheduleBase schedule; //!<计划抓图的配置信息
                std::vector<EventBaseItem> events; //!<事件抓图的配置信息
            } snap;
        };

        virtual ErrCodeE SetParam(const std::shared_ptr<Param> &param) = 0;

        virtual ErrCodeE GetParam(std::shared_ptr<Param> &param) = 0;

        virtual ErrCodeE GetManualRecordStat(bool &ena) = 0;

        virtual ErrCodeE SetManualRecordStat(bool ena) = 0;

        virtual ErrCodeE GetRecordList(uint32_t startUtc, uint32_t endUtc, bool merge, std::vector<RecFile> &list) = 0;

        static char *GetRecordFileNameByRecFile(RecFile &rec, char buffer[EMX_MAX_PATH_SIZE]) {
            snprintf(buffer, EMX_MAX_PATH_SIZE, "VID_%08d_%06d(%u_%u).mp4",
                     rec.date, rec.time, rec.startUtc, rec.endUtc);
            return buffer;
        }

        static char *GetRecordFilePathByRecFile(const char *top, RecFile &rec, char buffer[EMX_MAX_PATH_SIZE]) {
            snprintf(buffer, EMX_MAX_PATH_SIZE, "%s/record/%08d/%02d/VID_%08d_%06d(%u_%u).mp4",
                     top, rec.date, rec.time / 10000,
                     rec.date, rec.time, rec.startUtc, rec.endUtc);
            return buffer;
        }

        virtual ErrCodeE ManualSnap() = 0;

        virtual ErrCodeE GetSnapList(uint32_t startUtc, uint32_t endUtc, std::vector<RecFile> &list) = 0;

        static char *GetSnapFileNameByRecFile(RecFile &rec, char buffer[EMX_MAX_PATH_SIZE]) {
            snprintf(buffer, EMX_MAX_PATH_SIZE, "PIC_%08d_%06d(%u).jpg",
                     rec.date, rec.time, rec.startUtc);
            return buffer;
        }

        static char *GetSnapFilePathByRecFile(const char *top, RecFile &rec, char buffer[EMX_MAX_PATH_SIZE]) {
            snprintf(buffer, EMX_MAX_PATH_SIZE, "%s/snap/%08d/%02d/PIC_%08d_%06d(%u).jpg",
                     top, rec.date, rec.time / 10000,
                     rec.date, rec.time, rec.startUtc);
            return buffer;
        }

    };

}
#endif //EMX_RecordStd_HPP
