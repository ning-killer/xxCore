#include "../include/post_process.h"

void TS_MPI_TRP_RNE_DetectionPostProcess(RNE_BLOBS_S *debug,
        std::vector<cv::Rect2i>& boxes,
        std::vector<float>& confidences,
        std::vector<int>& classIds)
{
    for (TS_U32 idx = 0; idx < debug->u32NBlob; idx++) {
        TS_S32 h = debug->stpBlob[idx].s32H;
        TS_S32 w = debug->stpBlob[idx].s32W;
        TS_S32 c = debug->stpBlob[idx].s32C;
        bool is_unsign = debug->stpBlob[idx].bIsUnsigned;
        TS_S32 cStride = TS_MPI_TRP_RNE_CStride(c, debug->stpBlob[idx].s32BitNum,
                                                debug->stpBlob[idx].bIsJoined);
        TS_S32 uSize = (debug->stpBlob[idx].s32BitNum / CHAR_BIT);
        TS_U8 *out = (TS_U8 *)(debug->stpBlob[idx].vpAddr);
        float coeff = *(debug->stpBlob[idx].fCoeff);
        TS_S32 lenStride = c /
                           ANCHORS_NUMBER; // yolov5 demo模型c为255，c / ANCHORS_NUMBER = 85(cx, cy, cwidth, cheight, score + 80个class)
        for(TS_S32 n = 0; n < BATCH_SIZE; n++) {
            for (TS_S32 i = 0; i < h; i++) {
                for (TS_S32 j = 0; j < w; j++) {
                    for(TS_S32 l = 0; l < ANCHORS_NUMBER; l++) {
                        cv::Rect2i rect;
                        float cx = 0.0, cy=0.0, cwidth=0.0, cheight=0.0;
                        int max_class_id = 0;
                        float max_class_score = 0.0;
                        TS_S32 *d = (TS_S32 *)(out + (((j + i * w) * cStride + l*lenStride+4)
                                                      *uSize)); // 跳过cx，cy，cwidth，cheight 4个位置，解析score
                        // 解析结果数据类型(有无符号，位宽)
                        TS_S32 data = *d & MASK_BITS(debug->stpBlob[idx].s32BitNum);
                        TS_S32 bitNum = (debug->stpBlob[idx].s32BitNum - 1);
                        TS_S32 bitValue = MASK_BITS(bitNum);
                        if (!is_unsign && data > bitValue) {
                            bitNum = (debug->stpBlob[idx].s32BitNum);
                            bitValue = MASK_BITS(bitNum);
                            data = data - (bitValue + 1);
                        }

                        // Score反量化为浮点
                        float conf = TS_MPI_TRP_RNE_Sigmoid(data * coeff);
                        if(conf > SCORE_THRESHOLD_FOR_BOX_REGRESSION) {
                            for (TS_S32 k = 0; k < lenStride; k++) {
                                // 解析cx, cy, cwidth, cheight
                                TS_S32 *d = (TS_S32 *)(out + (((j + i * w) * cStride + l*lenStride+k) *uSize));
                                TS_S32 data = *d & MASK_BITS(debug->stpBlob[idx].s32BitNum);
                                TS_S32 bitNum = (debug->stpBlob[idx].s32BitNum - 1);
                                TS_S32 bitValue = MASK_BITS(bitNum);
                                if (!is_unsign && data > bitValue) {
                                    bitNum = (debug->stpBlob[idx].s32BitNum);
                                    bitValue = MASK_BITS(bitNum);
                                    data = data - (bitValue + 1);
                                }

                                // cx, cy, cwidth, cheight 反量化为浮点
                                float f_data =  data * coeff;
                                f_data = TS_MPI_TRP_RNE_Sigmoid(f_data);
                                // yolov5模型边界框回归，不同检测模型计算公式不同，可能需要修改
                                if(k==0) { // cx
                                    cx = ((f_data * 2.0f - 0.5f) + j) * strides[idx];
                                    continue;
                                } else if(k==1) { // cy
                                    cy = ((f_data * 2.0f - 0.5f) + i) * strides[idx];
                                    continue;
                                } else if(k==2) { // cwidth
                                    cwidth = (float) (f_data *f_data* 4.0f * anchors[std::to_string(idx)][l][0]);
                                    continue;
                                } else if(k==3) { // cheight
                                    cheight = (float) (f_data *f_data* 4.0f * anchors[std::to_string(idx)][l][1]);
                                    continue;
                                } else if(k==4) {
                                    conf= f_data;
                                    continue;
                                } else {
                                    if (max_class_score<=f_data) {
                                        max_class_score=f_data;
                                        max_class_id = k;
                                    }
                                }
                            }
                            // 中心点坐标转化成矩形框坐标(左上角，右下角)
                            rect.x = cx - cwidth / 2.0f;
                            rect.y = cy - cheight / 2.0f;
                            rect.width = cx + cwidth / 2.0f;;
                            rect.height = cy + cheight / 2.0f;;
                            boxes.emplace_back(rect);
                            confidences.emplace_back(max_class_score * conf);
                            classIds.emplace_back(max_class_id-5); // 求出分类结果id = max_class_id-5
                        }
                    }
                }
            }
            out += h * w * cStride * uSize;
        }
    }
}

int TS_MPI_TRP_RNE_PostProcess(RNE_BLOBS_S *outputBlobs, cv::Mat &dst_img,
                               std::string outFilePath)
{
    std::vector<cv::Rect2i> boxes;
    std::vector<float> confidences;
    std::vector<int> classIds;
    TS_MPI_TRP_RNE_DetectionPostProcess(outputBlobs,boxes,confidences,classIds);
    std::vector<int> keepIndices;

#ifdef DEBUG
    printf("before nms, The boxes number is %ld\n", boxes.size());
#endif

    cv::dnn::NMSBoxes(boxes, confidences, NMS_SCORE_THRESHOLD, NMS_THRESHOLD,
                      keepIndices);

#ifdef DEBUG
    printf("after nms, The boxes number is %ld\n", keepIndices.size());
#endif

    for (size_t i = 0; i < keepIndices.size(); i++) {
        int index = keepIndices.at(i);
        int X = boxes.at(index).x;
        int Y = boxes.at(index).y;
        int H = boxes.at(index).height - Y;
        int W = boxes.at(index).width - X;
        cv::Rect rect(X, Y, W, H);
        cv::rectangle(dst_img, rect, cv::Scalar(0, 255, 0),
                      5); // 参数3:框的颜色，参数4:框的粗细

        // 将score转化为字符串，保留4位小数并标注到图片
        std::string str = std::to_string(confidences.at(index));
        size_t pos = str.find('.');
        if (pos != std::string::npos && pos + 4 < str.length()) {
            str.erase(pos + 4, std::string::npos);
        }

        std::string confidenceText = class_name[classIds.at(index)] + ":" + str;
#ifdef DEBUG
        printf("%s\n", confidenceText.c_str());
#endif
        cv::Point textPosition(X, Y - 5); // 设置标注内容距离框5个像素
        int fontFace = cv::FONT_HERSHEY_SIMPLEX;
        double fontScale = 0.5; // 字体大小
        cv::Scalar fontColor(0, 0, 255);
        int thickness = 2; // 字体粗细
        cv::putText(dst_img, confidenceText, textPosition, fontFace, fontScale,
                    fontColor, thickness);
    }
    cv::imwrite(outFilePath, dst_img);
    printf("write file: %s\n", outFilePath.c_str());
    return 0;
}
