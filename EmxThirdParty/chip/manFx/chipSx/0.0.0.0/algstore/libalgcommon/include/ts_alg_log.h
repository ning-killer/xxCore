/*
 * Copyright (c) Beijing Tsing Micro Co.,Ltd. All rights reserved.
 * Description: base type definitions of alg
 * Author: Tsing Micro solution-application group
 * Create: 2022/08/18
 */
#ifndef _TS_ALG_LOG_H_
#define _TS_ALG_LOG_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include <stdbool.h>

enum { ALG_LOG_TRACE, ALG_LOG_DEBUG, ALG_LOG_INFO, ALG_LOG_WARN, ALG_LOG_ERROR, ALG_LOG_FATAL };

#define ALG_LOGT(...) log_print(ALG_LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define ALG_LOGD(...) log_print(ALG_LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define ALG_LOGI(...) log_print(ALG_LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define ALG_LOGW(...) log_print(ALG_LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define ALG_LOGE(...) log_print(ALG_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define ALG_LOGF(...) log_print(ALG_LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

void log_set_level(int level);

void log_set_quiet(bool enable);

void log_print(int level, const char *file, int line, const char *fmt, ...);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _TS_ALG_AMR__H_ */

