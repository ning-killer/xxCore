#ifndef RECORD_FILE_H_INCLUDED
#define RECORD_FILE_H_INCLUDED

#include <ts_rne_c_api.h>
#include <ts_rne_type.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

TS_S32 RNE_RecordFile(TS_S32 frameIndex, TS_S32 frameCount, RNE_BLOBS_S *debug, TS_CHAR *debugLayerName,
                                 TS_CHAR *outputPath, const TS_CHAR *type);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TS_RNE_RECORD_FILE_H__ */
