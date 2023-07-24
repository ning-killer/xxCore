/*
 * @Author: xiong
 * @Date: 2022/10/25
*/

#include "AlarmPerson.hpp"
#include "cmath"

using namespace Emx;

ErrCodeE AlarmPerson::Create() {
    m_cacheIdx = 0;
    memset(m_cache, 0, sizeof(m_cache));

    // 创建一个timer用于检测到移动侦测触发后，才认为人行有效
    m_ai = new MediaClientAiDataAsync(m_ctx->loop);
    m_ai->Start(nullptr, [this](MediaAi::AiInfoHeader &header) {
        if (!m_ctx->env.person.ena)
            return;
        if (header.type != MediaAi::AiTypeE::PersonDetection)
            return;
        if (m_ai->GetDataByInfo(header) != ErrCodeE::Success)
            return;

        auto *data = (MediaAi::AiInfoPersonDetectionData *) header.data;
        int maxArea = 0;
        int maxAreaIdx = 0;
        for (int i = 0; i < header.size / (int) sizeof(MediaAi::AiInfoPersonDetectionData); i++) {
            if (AlarmInZone(header.srcPicSize, data[i], m_ctx->env.person.zone)) {
                if (data[i].Area() > maxArea) {
                    maxArea = data[i].Area();
                    maxAreaIdx = i;
                }
            }
        }
        int targetArea;
        if (m_ctx->env.person.sensitivity <= 20) {
            targetArea = m_ctx->env.person.map[0];
        } else if (m_ctx->env.person.sensitivity <= 40) {
            targetArea = m_ctx->env.person.map[1];
        } else if (m_ctx->env.person.sensitivity <= 60) {
            targetArea = m_ctx->env.person.map[2];
        } else if (m_ctx->env.person.sensitivity <= 80) {
            targetArea = m_ctx->env.person.map[3];
        } else {
            targetArea = m_ctx->env.person.map[4];
        }
        if (maxArea * 1000 / header.srcPicSize.w * header.srcPicSize.h > targetArea) {
            //判断此人行是否静态人行，如果是静态人行则忽略掉
            if (!IsStaticPerson(header.srcPicSize, data[maxAreaIdx])) {
                AlarmTrigger();
            }
        }
    });
    return ErrCodeE::Success;
}

void AlarmPerson::Destroy() {
    m_ai->Stop();
    delete m_ai;
}

//896*512 16*16 pd_filter
bool AlarmPerson::IsStaticPerson(Size &size, MediaAi::AiInfoPersonDetectionData &data) {
    //判断cache里的框与当前data最近的中心距离是否小于静态阈值
    bool isStatic = false;
    float minDist = (float) size.w / 100;
    for (auto &d : m_cache) {
        int cx = abs((data.x + data.w / 2) - (d.x + d.w / 2));
        int cy = abs((data.y + data.y / 2) - (d.y + d.w / 2));
        auto dist = (float) sqrt(cx * cx + cy * cy);
        if (dist < minDist) {
            isStatic = true;
            break;
        }
    }
    //缓存最近的5个最大面积的区域，用于静态人行判断
    m_cache[m_cacheIdx] = data;
    m_cacheIdx++;
    if (m_cacheIdx == sizeof(m_cache) / sizeof(data))
        m_cacheIdx = 0;
    return isStatic;
}
