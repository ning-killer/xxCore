#ifndef __TS_RNE_CRC_H__
#define __TS_RNE_CRC_H__

#include <stdint.h>
#include <ts_rne_type.h>
#include <ts_rne_device.h>

#ifdef __cplusplus
extern "C" {
#endif

uint16_t TS_MPI_TRP_RNE_CRC16CCITT(TS_U16 crc, void *ptr, int len);
TS_U16 TS_MPI_TRP_RNE_GetBlobsCRC16CCITT(RNE_BLOBS_S *blobs);

#ifdef __cplusplus
}
#endif

#endif