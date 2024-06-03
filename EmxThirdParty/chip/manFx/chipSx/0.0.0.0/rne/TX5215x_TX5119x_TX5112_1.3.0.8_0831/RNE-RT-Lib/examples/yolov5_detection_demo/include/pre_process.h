#ifndef _PRE_PROCESS_H_
#define _PRE_PROCESS_H_

#include "common.h"

#define IMAGE_SIZE 640
#define MODEL_INPUT_REQUIRED_W 672
#define MODEL_INPUT_REQUIRED_H 448 

int TS_MPI_TRP_RNE_PreProcess(cv::Mat &src_img, cv::Mat &dst_img, cv::Mat &crop_img, bool use_int8_hwc);

#endif