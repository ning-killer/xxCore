/**
 * @file Gat1400Util.hpp
 * @author hening
 * @brief Gat1400协议工具类
 * @date 2023-6-27
 */

#ifndef EMX_GAT1400UTIL_HPP
#define EMX_GAT1400UTIL_HPP

#include "EmxCore.hpp"
#include "EnvAlarmFace.hpp"
#include <list>

// #define CMCC_1400_TEST

#ifdef CMCC_1400_TEST
#define CMCC_1400_GATEWAY_URL "http://36.133.154.67:8000/gateway"
#else
#define CMCC_1400_GATEWAY_URL "https://vis.komect.com:8886/gateway" 
#endif

#ifndef CHECK_GAT1400_PTR
#define CHECK_GAT1400_PTR(ptr, ret)                \
  do {                                             \
    if (ptr == nullptr) {                          \
        emxloge("ptr(%p) object is NULL.\n", ptr); \
        return ret;                                \
    }                                              \
  } while (false)
#endif

#ifndef CHECK_GAT1400_PTR2
#define CHECK_GAT1400_PTR2(ptr)                    \
  do {                                             \
    if (ptr == nullptr) {                          \
        emxloge("ptr(%p) object is NULL.\n", ptr); \
        return;                                    \
    }                                              \
  } while (false)
#endif

namespace Emx {
    class Gat1400Util {
        public:
        //!@brief 平台信息获取返回结果
        enum GateWayRet : uint8_t {
            ReqSuccess = 0,     //!< 请求成功
            GateWayError,       //!< 请求错误
            AddrChanged,        //!< 地址变更，重置初始化流程
        }; 

        //!@brief 请求注册返回结果
        enum RegisterRet : uint8_t {
            Authorized = 0,     //!< 已授权
            RegisterError,      //!< 请求错误
            UnAuthorized,       //!< 未授权
        }; 

        //!@brief gat1400初始化参数结构体
        struct InitParam {
            char deviceId[64];
            char password[64];
            EnvAlarmFace::SnapMode mode;
        };

        //!@brief gat1400网关参数结构体
        struct GateParam {
            int cfgInterval;
            int retry = 3;
            int timeout;
            int keepAliveInterval;
            bool postSceneImg;
            std::string host1400;
        };
  
        //!@brief gat1400注册授权参数结构体
        struct RegisterAuthParam {
            std::string realm;
            std::string qop;
            std::string nonce;
            std::string url;
            std::string httpMethod;
        };

        //!@brief gat1400系统时间参数结构体
        struct SystemTimeParam {
            std::string VIIDServerID;
            std::string TimeMode;
            std::string LocalTime;
            std::string TimeZone;
        };

        //!@brief gat1400子图数据参数结构体
        struct SubImgDataParam {
            int EventSort;
            int Width;
            int Height;
            int FileSize;
            std::string ImageID = "";
            std::string StoragePath = "";
            std::string Type = "";
            std::string FileFormat = "";
            std::string ShotTime = "";
            std::string Data = "";
        };

        //!@brief 抓拍模式
        enum UploadType : uint8_t {
            Faces = 0,          //!< 人脸
            Persons,            //!< 人形
            MotorVehicles,      //!< 机动车
            NonMotorVehicles,   //!< 非机动车
            Kitchen,            //!< 明厨亮灶
            Falling,            //!< 高空抛物批量
            Region,             //!< 区域人数统计批量
            Traffic,            //!< 倾斜客流
            OnLeave,            //!< 离岗分析
        };  

        //!@brief gat1400上传数据参数结构体
        struct UploadDataParam {
            UploadType type;
            int InfoKind;
            int LeftTopX;
            int LeftTopY;
            int RightBtmX;
            int RightBtmY;
            std::string SourceID;
            std::string deviceId;
            std::string Id;
            std::string LocationMarkTime;
            std::string AppearTime;
            std::string DisAppearTime;
            virtual ~UploadDataParam() {}
        };

        //!@brief 人脸上传数据
        struct  UploadFaceData : public UploadDataParam  {
            bool isHaveFullImg = false;
            int IsSuspectedTerrorist;
            int IsCriminalInvolved;
            int IsDetainees;
            int IsVictim;
            int IsSuspiciousPerson;
            std::string RespiratorColor = "";
            SubImgDataParam fullImg;
            std::list<SubImgDataParam> subImgList;
        };

        //!@brief 非机动车上传数据
        struct UploadNonMotorVehiclesData : public UploadDataParam  {
            bool HasPlate;
            std::string PlateClass = "";
            std::string PlateNo = "";
            std::string PlateColor = "";
            std::string VehicleColor = "";
            std::list<SubImgDataParam> subImgList;
        };

        //!@brief 一组车辆车牌数据
        struct VehiclePlateNo {
            int LaneNo;
            std::string HasPlate = "";
            std::string PlateClass = "";
            std::string PlateColor = "";
            std::string PlateNo = "";
            std::string VehicleClass = "";
            std::string Direction = "";
            std::string VehicleColor = "";
            std::string PassTime = "";
            std::string StorageUrl1 = "";
            SubImgDataParam Vehicle;
            SubImgDataParam Pl;
        };

        //!@brief 机动车上传数据
        struct UploadMotorVehiclesData : public UploadDataParam  {
            std::list<VehiclePlateNo> subImgList;
        };

        //!@brief 倾斜客流统计上传数据
        struct UploadTrafficData : public UploadDataParam  {
            std::string trafficID = "";
            std::string endTime = "";
            int inCount;
            int outCount;
        };

        //!@brief 区域人数统计上传数据
        struct UploadRegionData : public UploadDataParam  {
            std::string regionID = "";
            std::string scheduledTime = "";
            int peopleCount;
            int cordon = 2;
            int typeE = 0;
            SubImgDataParam img;
        };

        enum OnLeaveStatus {
            Unknown = 0, // 未知
            OnDuty,      // 在岗 
            OffDuty,     // 离岗  
        };

        //!@brief 离岗检测上传数据
        struct UploadOnLeaveData : public UploadDataParam  {
            std::string onLeaveID = "";
            std::string endTime = "";
            OnLeaveStatus eventType = Unknown;
            int onDutyCount;
            int offDutyDuration;
            Json::Value alarmZone;
            Json::Value alertTime;
        };

        //!@brief 图片类型
        enum ImgType : uint8_t {
            JPG = 0,        //!< jpg
            BMP,            //!< bmp
            PNG,            //!< png
        };

        //!@brief 获取上传标准时间精度
        enum TimePrecision : uint8_t {
            MS = 0,        //!< 毫秒
            S,             //!< 秒
        }; 

        static std::string GetNonceStr(const int size);

        static bool GetMd5(const char* src, std::string &md5);

        static std::string GetMapValueByKey(const std::string &key, std::map<std::string, std::string> &srcMap);

        static bool GetSign(const std::string nonce, const std::string devId, const std::string pwd, std::string &sign);

        static bool AesEcbPcsk5Encrypt(const std::string &in, std::string &out, char *key, int key_len);

        static void byteToHex(const unsigned char *input, int length, char *output);

        //note: 只支持秒级
        static std::string GetTimeStr(TimePrecision precision);

        static char *GetTimeStr(uint32_t utc, char *timeStr, int maxSize);

        static int GetRandom(int a, int b);

        static std::string GetImgType(ImgType type);

        static void Pkcs5Padding(std::string &data);

        static int m_index; 
    };
}

#endif //EMX_GAT1400UTIL_HPP