#ifndef __TS_RNE_NN_INPUT_H__
#define __TS_RNE_NN_INPUT_H__

#include "ts_rne_type.h"
#include "ts_rne_c_api.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


/**
 * @brief fill input bolbs of NN net
 *
 * @param net NN net pointer
 * @param blobs the input blobs of NN net
 * @param core_idx core index of NPU, default set 0.
 * @param input the input data of NN net
 *
 * @return 0: successful  others: fails
 */
TS_S32 TS_MPI_TRP_RNE_FillInputBlobs(RNE_NET_S *net, RNE_BLOBS_S *blobs, TS_S32 core_idx, TS_VOID *input);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TS_RNE_NN_INPUT_H__ */
