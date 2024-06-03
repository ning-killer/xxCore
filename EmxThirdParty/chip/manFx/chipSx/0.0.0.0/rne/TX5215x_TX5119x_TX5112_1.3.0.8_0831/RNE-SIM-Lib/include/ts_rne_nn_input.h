#ifndef __TS_RNE_NN_INPUT_H__
#define __TS_RNE_NN_INPUT_H__

#include "ts_rne_c_api.h"

BEGIN_DECLS

/**
 * @brief fill input bolbs of NN net
 *
 * @param net NN net pointer
 * @param core_idx core index of NPU, default set 0.
 * @param input the input data of NN net
 *
 * @return 0: successful  others: fails
 */
TS_S32 TS_MPI_TRP_RNE_FillInputBlobs(RNE_NET_S *net, TS_S32 core_idx, TS_VOID *input);

END_DECLS

#endif /* __TS_RNE_NN_INPUT_H__ */
