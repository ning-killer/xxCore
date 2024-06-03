#include "../include/pre_process.h"

int TS_MPI_TRP_RNE_PreProcess(cv::Mat &src_img, cv::Mat &dst_img,
                              cv::Mat &crop_img, bool use_int8_hwc)
{
    cv::Mat &img = src_img;
    int h0 = img.rows;
    int w0 = img.cols;
    float ratio = IMAGE_SIZE / std::max(h0, w0);
    cv::Mat resizedImage;
    int interp = ratio > 1 ?  cv::INTER_LINEAR : cv::INTER_AREA;
    cv::Size targetSize(MODEL_INPUT_REQUIRED_W, MODEL_INPUT_REQUIRED_H);
    cv::resize(img, resizedImage, targetSize, 0, 0, interp);

    cv::Mat chwImage = resizedImage.clone();
    if (!use_int8_hwc) {
        TS_MPI_TRP_RNE_HWC_TO_CHW(resizedImage, chwImage);
    }

    if (!use_int8_hwc) {
        cv::Mat processedImage = chwImage.clone();
        TS_MPI_TRP_RNE_BGR_OR_RGB(chwImage, processedImage);
        cv::Mat normalizedImage;
        // 对像素值归一化到(0,1)的浮点
        processedImage.convertTo(normalizedImage, CV_32F, 1.0 / 255.0);
        dst_img = normalizedImage;
    } else {
        dst_img = resizedImage;
    }
    crop_img = resizedImage;
    return 0;
}