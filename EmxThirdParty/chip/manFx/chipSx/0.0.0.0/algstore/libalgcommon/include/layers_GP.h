#ifndef LAYERS_GP_H_INCLUDED
#define LAYERS_GP_H_INCLUDED
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum LayerType_Id {
    SLICE_GP_LAYER = 1024,
    CONCAT_GP_LAYER,
    PERMUTE_GP_LAYER,
    SOFTMAX_GP_LAYER,
    DETECT_OUTPUT_GP_LAYER,
    FLATTEN_GP_LAYER,
    RESHAPE_GP_LAYER,
    CONCAT_GP_H_LAYER,
    CONCAT_GP_W_LAYER,
    RCEMFCC_LAYER,
    NORMALIZE_LAYER,
    PRIORBOXES_LAYER,
    DECONV_LAYER,
    LUT_LAYER,
    POOL_LAYER,
    REDUCTION_LAYER,
    REPEAT_LAYER,
    NORMALIZEL_LAYER,
    REORG_LAYER,
    EMBEDDING_LAYER,
    PRIORBOXE_RENTINA_LAYER,
    DETECTION_OUTPUT_RENTINA_LAYER,
    MAX_LAYER_TYPE
} LayerType_Id;

typedef struct WeightOfGP {
    int32_t id;
    uint32_t addr[2];
    uint32_t bits_of_1_w;
} WeightOfGP;

int rne_register_gp_layers(void);

int ts_shape_count(int *shape, int start_idx, int end_idx);

#ifdef __cplusplus
}
#endif

#endif
