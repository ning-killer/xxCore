//
// Created by xiong on 2022/4/27.
//

#ifndef EMX_MediaAi_HPP
#define EMX_MediaAi_HPP

#include <memory.h>
#include "MediaTypedef.hpp"
#include "media/MediaRegion.hpp"
#include "json/json.h"

namespace Emx {

    class MediaAi {
    public:
        //!@brief AI类型
        enum class AiTypeE : uint8_t {
            MotionDetection = 0, //!<移动侦测
            PersonDetection = 1, //!<人形侦测
            Classify = 2, //!<分类
            FaceRecognition = 3, //!<人脸识别
            PersonRecognition = 4, //!<人形识别
            FaceSnap = 5,//!<人脸抓拍
        };

        //!@brief 分类类型
        enum class ClassifyTypeE : uint32_t {
            None = 0, //!<默认无效
            Face = 1, //!<人脸
            Person = 2, //!<人
            Bicycle = 3, //!<自行车
            Car = 4, //!<小汽车
            Motorbike = 5, //!<摩托车
            Bus = 6, //!<公交车
            Truck = 7, //!<卡车
            Bird = 8, //!<鸟
            Cat = 9, //!<猫
            Dog = 10, //!<狗
            Horse = 11, //!<马
            CellPhone = 12, //!<手机
            End,
        };

        //!@brief AI持久化参数
        struct Param {
            bool ena; //!<全局使能

            //!@brief 移动侦测
            struct MotionDetection {
                bool ena; //!<是否使能
                uint32_t threshold = 64; //!<阈值
            } motionDetection;

            //!@brief 人形侦测
            struct PersonDetection {
                bool ena;  //!<是否使能
            } personDetection;

            //!@brief 分类
            struct Classify {
                bool ena; //!<是否使能
                int num; //!<类型的数量
                static const int MaxNum = 64; //!<类型最大数量
                ClassifyTypeE type[MaxNum]; //!<类型数组
            } classify;

            //!@brief 人形识别
            struct PersonRecognition {
                bool ena; //!<是否使能
            } personRecognition;

            //!@brief 人脸识别
            struct FaceRecognition {
                bool ena; //!<是否使能
            } faceRecognition;

            //!@brief 人脸人形协同侦测
            struct facePersonDetection{
                /*!auto(0)模式：SDK会自己缓存，挑出质量最好的一张。缓存时间:帧率*captureInterval决定
                 * fast(1)模式：只要满足人脸瞳距条件，SDK就会抓图返回，如果只需要一个人脸抓一张图，就把captureNum设置为1
                 */
                bool ena; //!<是否使能
                bool faceDraw;//!<是否开启人脸画框
                bool personDraw;//!<是否开启人形画框
                int aeEna;//!是否开启智能AE
                int aeValue;//!智能AE设置值
                int captureNum;//!人脸抓拍张数（每个人脸抓几张）
                int captureMode;//!抓拍模式(auto  fast)
                int captureInterval;//!抓拍间隔
                float captureQuality;//!抓拍质量(瞳距控制)
                float captureSensitivity;//!抓拍灵敏度(设置算法的灵敏度)
            }facePersonDetection;
        };

        //!@brief AI信息头
        struct AiInfoHeader {
            int32_t chn; //!<通道号
            AiTypeE type; //!<Ai类型
            uint64_t timestampMs; //!<时间戳
            Size srcPicSize; //!<AI信息里坐标或长宽信息基于的原始图像大小
            int32_t off; //!<ringbuffer偏移，用户不需要关心
            int32_t size; //!<信息数据大小
            uint8_t *data; //!<信息数据内存指针
        };

        struct picAlarmBase{
            int imgType;   /*0=jpg,1=bmp,2=png*/
            unsigned int Width;/*图片宽*/
            unsigned int Height; /*图片高*/
            unsigned long imgSize;   /*图片大小*/
            unsigned char *img;/*图片数据缓存指针*/
        };

        struct facePicMsg:public picAlarmBase{
        public:
            bool  isFullPic;
            unsigned long long id;             /*人脸id*/
            unsigned long long capture_time;   /*人脸抓拍到的毫秒时间戳*/
            unsigned long long start_time;     /*人脸开始出现时间的毫秒时间戳*/
            unsigned long long end_time;       /*人脸结束时间,毫秒时间戳*/
            int face_quality ;                 /*人脸质量*/
            int face_sharpness ;               /*人脸清晰度*/
            Rect rect;                         /*当前人脸位置信息*/
        };

        typedef struct facePicAlarm{
            int toTalPic;
            facePicMsg faceMsg[10];
        }facePicAlarm;

        //!@brief 移动侦测数据类型
        using AiInfoMotionDetectionData = Rect;
        //!@brief 人形侦测数据类型
        using AiInfoPersonDetectionData = Rect;
        //!@brief 人脸侦测数据类型
        using AiInfoFaceDetectionData = facePicAlarm;
//
//        struct AiInfoPersonRecognitionData {
//            char id[256];
//            Rect rect;
//        };
//
//        struct AiInfoClassifyData {
//            ClassifyTypeE type;
//            char text[256];
//            Rect rect;
//        };

        explicit MediaAi(int32_t chn) : m_chn(chn) {};

        virtual ~MediaAi() {};

        /**
         * @brief 设置参数
         * @param param [in] 参数数据
         * @return ErrCodeE
         */
        virtual ErrCodeE SetParam(const Param &param) = 0;

        /**
         * @brief 获取参数
         * @param param [out] 参数数据
         * @return ErrCodeE
         */
        virtual ErrCodeE GetParam(Param &param) = 0;

        /**
         * @brief 获取参数取值范围
         * @param range [out] 参数取值范围
         * @return ErrCodeE
         */
        virtual ErrCodeE GetParamRange(Json::Value &range) = 0;

        /**
         * @brief 判断参数是否改变
         * @param paramA [in] 参数A
         * @param paramB [in] 参数B
         * @retval true:改变
         * @retval false:未改变
         */
        static bool ParamChanged(const Param &paramA, const Param &paramB) {
            return memcmp(&paramA, &paramB, sizeof(Param));
        }

        /**
         * @brief 获取通道号
         * @return 通道号
         */
        int32_t GetChn() { return m_chn; }

    protected:
        int32_t m_chn;
    };

}
#endif //EMX_MediaAi_HPP

