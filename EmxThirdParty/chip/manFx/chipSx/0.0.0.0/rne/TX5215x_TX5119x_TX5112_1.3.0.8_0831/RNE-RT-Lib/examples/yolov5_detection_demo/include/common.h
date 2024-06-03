#ifndef _COMMON_H_
#define _COMMON_H_

#include <map>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>

extern std::map<int, std::string> class_name;
extern int strides[3];
extern std::unordered_map<std::string, std::vector<std::vector<int>>> anchors;

float TS_MPI_TRP_RNE_Sigmoid(float x);
long TS_MPI_TRP_RNE_GetFileSize(FILE *stream);
int TS_MPI_TRP_RNE_HWC_TO_CHW(cv::Mat &src_img, cv::Mat &dst_img);
int TS_MPI_TRP_RNE_BGR_OR_RGB(cv::Mat &src_img, cv::Mat &dst_img);
int TS_MPI_TRP_RNE_BGR_OR_RGB_INT8(cv::Mat &src_img, cv::Mat &dst_img);

#endif