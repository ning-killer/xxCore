#ifndef _POST_PROCESS_H_
#define _POST_PROCESS_H_

#include "common.h"
#include "ts_rne_c_api.h"

#define SCORE_THRESHOLD_FOR_BOX_REGRESSION 0.25 // 为加速边界框回归，低于该值的边界框会被舍弃
#define NMS_SCORE_THRESHOLD 0.25                // NMS置信度阈值，低于该值的边界框被舍弃
#define NMS_THRESHOLD 0.45                      // NMS阈值，IOU重叠度高于该阈值的边界框会被抑制
#define BATCH_SIZE 1                            // 模型的N维度大小
#define ANCHORS_NUMBER 3                        // YOLOV5每个特征点的先验框个数

int TS_MPI_TRP_RNE_PostProcess(RNE_BLOBS_S *outputBlobs, cv::Mat &dst_img, std::string outFilePath);
#endif