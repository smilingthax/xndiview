/*
 *  Copyright 2015 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <string.h>  // For memset/memcpy

#include "libyuv/scale.h"
#include "libyuv/scale_row.h"

#include "libyuv/basic_types.h"

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

// Fixed scale down.
// Mask may be non-power of 2, so use MOD
#define SDANY(NAMEANY, SCALEROWDOWN_SIMD, SCALEROWDOWN_C, FACTOR, BPP, MASK)   \
  void NAMEANY(const uint8_t* src_ptr, ptrdiff_t src_stride, uint8_t* dst_ptr, \
               int dst_width) {                                                \
    int r = (int)((unsigned int)dst_width % (MASK + 1)); /* NOLINT */          \
    int n = dst_width - r;                                                     \
    if (n > 0) {                                                               \
      SCALEROWDOWN_SIMD(src_ptr, src_stride, dst_ptr, n);                      \
    }                                                                          \
    SCALEROWDOWN_C(src_ptr + (n * FACTOR) * BPP, src_stride,                   \
                   dst_ptr + n * BPP, r);                                      \
  }

// Fixed scale down for odd source width.  Used by I420Blend subsampling.
// Since dst_width is (width + 1) / 2, this function scales one less pixel
// and copies the last pixel.
#define SDODD(NAMEANY, SCALEROWDOWN_SIMD, SCALEROWDOWN_C, FACTOR, BPP, MASK)   \
  void NAMEANY(const uint8_t* src_ptr, ptrdiff_t src_stride, uint8_t* dst_ptr, \
               int dst_width) {                                                \
    int r = (int)((unsigned int)(dst_width - 1) % (MASK + 1)); /* NOLINT */    \
    int n = (dst_width - 1) - r;                                               \
    if (n > 0) {                                                               \
      SCALEROWDOWN_SIMD(src_ptr, src_stride, dst_ptr, n);                      \
    }                                                                          \
    SCALEROWDOWN_C(src_ptr + (n * FACTOR) * BPP, src_stride,                   \
                   dst_ptr + n * BPP, r + 1);                                  \
  }

#ifdef HAS_SCALEARGBROWDOWN2_SSE2
SDANY(ScaleARGBRowDown2_Any_SSE2,
      ScaleARGBRowDown2_SSE2,
      ScaleARGBRowDown2_C,
      2,
      4,
      3)
SDANY(ScaleARGBRowDown2Linear_Any_SSE2,
      ScaleARGBRowDown2Linear_SSE2,
      ScaleARGBRowDown2Linear_C,
      2,
      4,
      3)
SDANY(ScaleARGBRowDown2Box_Any_SSE2,
      ScaleARGBRowDown2Box_SSE2,
      ScaleARGBRowDown2Box_C,
      2,
      4,
      3)
#endif
#ifdef HAS_SCALEARGBROWDOWN2_NEON
SDANY(ScaleARGBRowDown2_Any_NEON,
      ScaleARGBRowDown2_NEON,
      ScaleARGBRowDown2_C,
      2,
      4,
      7)
SDANY(ScaleARGBRowDown2Linear_Any_NEON,
      ScaleARGBRowDown2Linear_NEON,
      ScaleARGBRowDown2Linear_C,
      2,
      4,
      7)
SDANY(ScaleARGBRowDown2Box_Any_NEON,
      ScaleARGBRowDown2Box_NEON,
      ScaleARGBRowDown2Box_C,
      2,
      4,
      7)
#endif
#ifdef HAS_SCALEARGBROWDOWN2_MSA
SDANY(ScaleARGBRowDown2_Any_MSA,
      ScaleARGBRowDown2_MSA,
      ScaleARGBRowDown2_C,
      2,
      4,
      3)
SDANY(ScaleARGBRowDown2Linear_Any_MSA,
      ScaleARGBRowDown2Linear_MSA,
      ScaleARGBRowDown2Linear_C,
      2,
      4,
      3)
SDANY(ScaleARGBRowDown2Box_Any_MSA,
      ScaleARGBRowDown2Box_MSA,
      ScaleARGBRowDown2Box_C,
      2,
      4,
      3)
#endif
#ifdef HAS_SCALEARGBROWDOWN2_MMI
SDANY(ScaleARGBRowDown2_Any_MMI,
      ScaleARGBRowDown2_MMI,
      ScaleARGBRowDown2_C,
      2,
      4,
      1)
SDANY(ScaleARGBRowDown2Linear_Any_MMI,
      ScaleARGBRowDown2Linear_MMI,
      ScaleARGBRowDown2Linear_C,
      2,
      4,
      1)
SDANY(ScaleARGBRowDown2Box_Any_MMI,
      ScaleARGBRowDown2Box_MMI,
      ScaleARGBRowDown2Box_C,
      2,
      4,
      1)
#endif
#undef SDANY

// Scale down by even scale factor.
#define SDAANY(NAMEANY, SCALEROWDOWN_SIMD, SCALEROWDOWN_C, BPP, MASK)       \
  void NAMEANY(const uint8_t* src_ptr, ptrdiff_t src_stride, int src_stepx, \
               uint8_t* dst_ptr, int dst_width) {                           \
    int r = dst_width & MASK;                                               \
    int n = dst_width & ~MASK;                                              \
    if (n > 0) {                                                            \
      SCALEROWDOWN_SIMD(src_ptr, src_stride, src_stepx, dst_ptr, n);        \
    }                                                                       \
    SCALEROWDOWN_C(src_ptr + (n * src_stepx) * BPP, src_stride, src_stepx,  \
                   dst_ptr + n * BPP, r);                                   \
  }

#ifdef HAS_SCALEARGBROWDOWNEVEN_SSE2
SDAANY(ScaleARGBRowDownEven_Any_SSE2,
       ScaleARGBRowDownEven_SSE2,
       ScaleARGBRowDownEven_C,
       4,
       3)
SDAANY(ScaleARGBRowDownEvenBox_Any_SSE2,
       ScaleARGBRowDownEvenBox_SSE2,
       ScaleARGBRowDownEvenBox_C,
       4,
       3)
#endif
#ifdef HAS_SCALEARGBROWDOWNEVEN_NEON
SDAANY(ScaleARGBRowDownEven_Any_NEON,
       ScaleARGBRowDownEven_NEON,
       ScaleARGBRowDownEven_C,
       4,
       3)
SDAANY(ScaleARGBRowDownEvenBox_Any_NEON,
       ScaleARGBRowDownEvenBox_NEON,
       ScaleARGBRowDownEvenBox_C,
       4,
       3)
#endif
#ifdef HAS_SCALEARGBROWDOWNEVEN_MSA
SDAANY(ScaleARGBRowDownEven_Any_MSA,
       ScaleARGBRowDownEven_MSA,
       ScaleARGBRowDownEven_C,
       4,
       3)
SDAANY(ScaleARGBRowDownEvenBox_Any_MSA,
       ScaleARGBRowDownEvenBox_MSA,
       ScaleARGBRowDownEvenBox_C,
       4,
       3)
#endif
#ifdef HAS_SCALEARGBROWDOWNEVEN_MMI
SDAANY(ScaleARGBRowDownEven_Any_MMI,
       ScaleARGBRowDownEven_MMI,
       ScaleARGBRowDownEven_C,
       4,
       1)
SDAANY(ScaleARGBRowDownEvenBox_Any_MMI,
       ScaleARGBRowDownEvenBox_MMI,
       ScaleARGBRowDownEvenBox_C,
       4,
       1)
#endif

// Definition for ScaleFilterCols, ScaleARGBCols and ScaleARGBFilterCols
#define CANY(NAMEANY, TERP_SIMD, TERP_C, BPP, MASK)                            \
  void NAMEANY(uint8_t* dst_ptr, const uint8_t* src_ptr, int dst_width, int x, \
               int dx) {                                                       \
    int r = dst_width & MASK;                                                  \
    int n = dst_width & ~MASK;                                                 \
    if (n > 0) {                                                               \
      TERP_SIMD(dst_ptr, src_ptr, n, x, dx);                                   \
    }                                                                          \
    TERP_C(dst_ptr + n * BPP, src_ptr, r, x + n * dx, dx);                     \
  }

#ifdef HAS_SCALEARGBCOLS_NEON
CANY(ScaleARGBCols_Any_NEON, ScaleARGBCols_NEON, ScaleARGBCols_C, 4, 7)
#endif
#ifdef HAS_SCALEARGBCOLS_MSA
CANY(ScaleARGBCols_Any_MSA, ScaleARGBCols_MSA, ScaleARGBCols_C, 4, 3)
#endif
#ifdef HAS_SCALEARGBCOLS_MMI
CANY(ScaleARGBCols_Any_MMI, ScaleARGBCols_MMI, ScaleARGBCols_C, 4, 0)
#endif
#ifdef HAS_SCALEARGBFILTERCOLS_NEON
CANY(ScaleARGBFilterCols_Any_NEON,
     ScaleARGBFilterCols_NEON,
     ScaleARGBFilterCols_C,
     4,
     3)
#endif
#ifdef HAS_SCALEARGBFILTERCOLS_MSA
CANY(ScaleARGBFilterCols_Any_MSA,
     ScaleARGBFilterCols_MSA,
     ScaleARGBFilterCols_C,
     4,
     7)
#endif
#undef CANY

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif
