#include "../include/common.h"

std::map<int, std::string> class_name = {{0, "person"}, {1, "bicycle"}, {2, "car"}, {3, "motorcycle"}, {4, "airplane"}, {5, "bus"}, {6, "train"}, {7, "truck"}, {8, "boat"}, {9, "traffic light"}, {10, "fire hydrant"}, {11, "stop sign"}, {12, "parking meter"}, {13, "bench"}, {14, "bird"}, {15, "cat"}, {16, "dog"}, {17, "horse"}, {18, "sheep"}, {19, "cow"}, {20, "elephant"}, {21, "bear"}, {22, "zebra"}, {23, "giraffe"}, {24, "backpack"}, {25, "umbrella"}, {26, "handbag"}, {27, "tie"}, {28, "suitcase"}, {29, "frisbee"}, {30, "skis"}, {31, "snowboard"}, {32, "sports ball"}, {33, "kite"}, {34, "baseball bat"}, {35, "baseball glove"}, {36, "skateboard"}, {37, "surfboard"}, {38, "tennis racket"}, {39, "bottle"}, {40, "wine glass"}, {41, "cup"}, {42, "fork"}, {43, "knife"}, {44, "spoon"}, {45, "bowl"}, {46, "banana"}, {47, "apple"}, {48, "sandwich"}, {49, "orange"}, {50, "broccoli"}, {51, "carrot"}, {52, "hot dog"}, {53, "pizza"}, {54, "donut"}, {55, "cake"}, {56, "chair"}, {57, "couch"}, {58, "potted plant"}, {59, "bed"}, {60, "dining table"}, {61, "toilet"}, {62, "tv"}, {63, "laptop"}, {64, "mouse"}, {65, "remote"}, {66, "keyboard"}, {67, "cell phone"}, {68, "microwave"}, {69, "oven"}, {70, "toaster"}, {71, "sink"}, {72, "refrigerator"}, {73, "book"}, {74, "clock"}, {75, "vase"}, {76, "scissors"}, {77, "teddy bear"}, {78, "hair drier"}, {79, "toothbrush"}};

int strides[3] = {8,16,32};
std::unordered_map<std::string, std::vector<std::vector<int>>> 	anchors = {
    {"0", {{10, 13}, {16, 30}, {33, 23}}},
    {"1", {{30, 61}, {62, 45}, {59, 119}}},
    {"2", {{116, 90}, {156, 198}, {373, 326}}}
};

float TS_MPI_TRP_RNE_Sigmoid(float x)
{
    return (1 / (1 + std::exp(-x)));
}

int TS_MPI_TRP_RNE_HWC_TO_CHW(cv::Mat &src_img, cv::Mat &dst_img)
{
    uint8_t *src = src_img.data;
    uint8_t *dst = dst_img.data;
    for (int c = 0; c < dst_img.channels(); c++) {
        for (int h = 0; h < dst_img.rows; h++) {
            for (int w = 0; w < dst_img.cols; w++) {
                int src_idex = h * dst_img.cols * dst_img.channels() + w * dst_img.channels() +
                               c;
                int dst_idex = c * dst_img.rows * dst_img.cols + h * dst_img.cols + w;
                dst[dst_idex] = src[src_idex];
            }
        }
    }
    return 0;
}

int TS_MPI_TRP_RNE_BGR_OR_RGB(cv::Mat &src_img, cv::Mat &dst_img)
{
    uint32_t len = dst_img.rows * dst_img.cols;
    memcpy(dst_img.data, src_img.data + len * 2, len);
    memcpy(dst_img.data + len, src_img.data + len, len);
    memcpy(dst_img.data + len * 2, src_img.data, len);
    return 0;
}

int TS_MPI_TRP_RNE_BGR_OR_RGB_INT8(cv::Mat &src_img, cv::Mat &dst_img)
{
    for (int i = 0; i < src_img.rows; ++i) {
        for (int j = 0; j < src_img.cols; ++j) {
            *(dst_img.data + i * dst_img.cols + j * dst_img.channels()) = *
                    (src_img.data + i * src_img.cols + j * src_img.channels() + 2);
            *(dst_img.data + i * dst_img.cols + j * dst_img.channels() + 1) = *
                    (src_img.data + i * src_img.cols + j * src_img.channels() + 1);
            *(dst_img.data + i * dst_img.cols + j * dst_img.channels() + 2) = *
                    (src_img.data + i * src_img.cols + j * src_img.channels());;
        }
    }
    return 0;
}

long TS_MPI_TRP_RNE_GetFileSize(FILE *stream)
{
    long file_size = -1;
    long cur_offset = ftell(stream);

    if (cur_offset == -1) {
        printf("ftell failed :%s\n", strerror(errno));
        return -1;
    }

    if (fseek(stream, 0, SEEK_END) != 0) {
        printf("fseek failed: %s\n", strerror(errno));
        return -1;
    }

    file_size = ftell(stream);
    if (file_size == -1) {
        printf("ftell failed :%s\n", strerror(errno));
    }

    if (fseek(stream, cur_offset, SEEK_SET) != 0) {
        printf("fseek failed: %s\n", strerror(errno));
        return -1;
    }

    return file_size;
}
