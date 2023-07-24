#ifndef _CVI_COMM_IVS_H_
#define _CVI_COMM_IVS_H_

#include "cvi_type.h"

typedef enum {
  IVS_MD_MODEL_BGS = 0, /* base on Background Subtraction */
  IVS_MD_MODEL_SGM = 1, /* base on Single (Simple) Gaussian Model */
  IVS_MD_MODEL_GMM = 2, /* base on Gaussian Mixture Model */
  IVS_MD_MODEL_BUTT
} IVS_MD_MODEL_E;

typedef struct {
  bool do_morphology;
} IVS_MD_BGS_CONFIG_S;

typedef struct {
  float update_rate;
  int update_interval;
  uint8_t diff_init;
  uint8_t diff_min;
  uint16_t sad_threshold;
  bool do_morphology;
} IVS_MD_SGM_CONFIG_S;

typedef struct {
  IVS_MD_BGS_CONFIG_S bgs;
  IVS_MD_SGM_CONFIG_S sgm;
} IVS_MD_CONFIG_S;

typedef struct {
  int x1;
  int y1;
  int x2;
  int y2;
} MD_BBOX_S;

typedef struct {
  uint32_t size;
  MD_BBOX_S *bbox;
} MD_BBOX_INFO_S;

typedef struct {
  float x1;
  float y1;
  float x2;
  float y2;
} bbox_t;

/** @enum trk_state_type_t
 * @ingroup core_cviaicore
 * @brief Enum describing the tracking state.
 */
typedef enum {
  CVI_TRACKER_NEW = 0,
  CVI_TRACKER_UNSTABLE,
  CVI_TRACKER_STABLE,
} trk_state_type_t;

/** @struct tracker_info_t
 * @ingroup core_cviaicore
 * @brief Tracking info of a object.
 *
 * @var tracker_info_t:id:
 * The tracker ID number.
 * @var tracker_info_t::state
 * The tracking state of the object (or face).
 * @var tracker_info_t::bbox
 * The MOT algorithm computed bbox.
 */
typedef struct {
  uint64_t id;
  trk_state_type_t state;
  bbox_t bbox;
} tracker_info_t;

/** @struct tracker_t
 * @ingroup core_cviaicore
 * @brief Tracking meta.
 *
 * @var tracker_t::size
 * The size of the info.
 * @var tracker_t::info
 * The object tracking array.
 */
typedef struct {
  uint32_t size;
  tracker_info_t *info;
} tracker_t;

#endif
