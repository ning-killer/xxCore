#ifndef __TS_RNE_VERSION_H__
#define __TS_RNE_VERSION_H__

#include "ts_rne_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum tsRNE_TYPE_NAME_E {
    RNE_TYPE_NAME_TX5105C     = 0,
    RNE_TYPE_NAME_TX5103C     = 1,
    RNE_TYPE_NAME_TX5105CV100 = 2,
    RNE_TYPE_NAME_TX5103CV100 = 3,
    RNE_TYPE_NAME_TX5101C     = 4,
    RNE_TYPE_NAME_TX5101D     = 5,
    RNE_TYPE_NAME_TX5101E     = 6,
    RNE_TYPE_NAME_TX2311R     = 7,
    RNE_TYPE_NAME_TX2311S     = 8,
    RNE_TYPE_NAME_TX232       = 9,
    RNE_TYPE_NAME_TX5368AV200 = 10,
    RNE_TYPE_NAME_TX5215CV200 = 11,
    RNE_TYPE_NAME_TX5215DV200 = 12,
    RNE_TYPE_NAME_TX5215DV300 = 13,
    RNE_TYPE_NAME_TX5239DV200 = 14,
    RNE_TYPE_NAME_TX5112CV200 = 15,
    RNE_TYPE_NAME_TX5112DV200 = 16,
    RNE_TYPE_NAME_TX5119DV200 = 17,
    RNE_TYPE_NAME_TX5339AV200 = 18,
    RNE_TYPE_NAME_MAX
} RNE_TYPE_NAME_E;

/**
 * @brief Initialize resources based on product type
 *
 * @param type product type
 *
 * @return 0: successful  others: fails
 */
TS_S32 TS_MPI_TRP_RNE_InitResourceByType(RNE_TYPE_NAME_E type);

/**
 * @brief the interface of get sdk version
 *
 * @param none
 *
 * @return sdk version
 */
const TS_CHAR *TS_MPI_TRP_RNE_GetSdkVersion(TS_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TS_RNE_VERSION_H__ */
