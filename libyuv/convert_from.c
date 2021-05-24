/*
 *  Copyright 2012 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "libyuv/basic_types.h"
#include "libyuv/cpu_id.h"
#include "libyuv/planar_functions.h"
#include "libyuv/row.h"

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

LIBYUV_API
int I422ToUYVY(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_u,
               int src_stride_u,
               const uint8_t* src_v,
               int src_stride_v,
               uint8_t* dst_uyvy,
               int dst_stride_uyvy,
               int width,
               int height) {
  int y;
  void (*I422ToUYVYRow)(const uint8_t* src_y, const uint8_t* src_u,
                        const uint8_t* src_v, uint8_t* dst_uyvy, int width) =
      I422ToUYVYRow_C;
  if (!src_y || !src_u || !src_v || !dst_uyvy || width <= 0 || height == 0) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    dst_uyvy = dst_uyvy + (height - 1) * dst_stride_uyvy;
    dst_stride_uyvy = -dst_stride_uyvy;
  }
  // Coalesce rows.
  if (src_stride_y == width && src_stride_u * 2 == width &&
      src_stride_v * 2 == width && dst_stride_uyvy == width * 2) {
    width *= height;
    height = 1;
    src_stride_y = src_stride_u = src_stride_v = dst_stride_uyvy = 0;
  }
#if defined(HAS_I422TOUYVYROW_SSE2)
  if (TestCpuFlag(kCpuHasSSE2)) {
    I422ToUYVYRow = I422ToUYVYRow_Any_SSE2;
    if (IS_ALIGNED(width, 16)) {
      I422ToUYVYRow = I422ToUYVYRow_SSE2;
    }
  }
#endif
#if defined(HAS_I422TOUYVYROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    I422ToUYVYRow = I422ToUYVYRow_Any_AVX2;
    if (IS_ALIGNED(width, 32)) {
      I422ToUYVYRow = I422ToUYVYRow_AVX2;
    }
  }
#endif
#if defined(HAS_I422TOUYVYROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    I422ToUYVYRow = I422ToUYVYRow_Any_NEON;
    if (IS_ALIGNED(width, 16)) {
      I422ToUYVYRow = I422ToUYVYRow_NEON;
    }
  }
#endif
#if defined(HAS_I422TOUYVYROW_MMI)
  if (TestCpuFlag(kCpuHasMMI)) {
    I422ToUYVYRow = I422ToUYVYRow_Any_MMI;
    if (IS_ALIGNED(width, 8)) {
      I422ToUYVYRow = I422ToUYVYRow_MMI;
    }
  }
#endif
#if defined(HAS_I422TOUYVYROW_MSA)
  if (TestCpuFlag(kCpuHasMSA)) {
    I422ToUYVYRow = I422ToUYVYRow_Any_MSA;
    if (IS_ALIGNED(width, 32)) {
      I422ToUYVYRow = I422ToUYVYRow_MSA;
    }
  }
#endif

  for (y = 0; y < height; ++y) {
    I422ToUYVYRow(src_y, src_u, src_v, dst_uyvy, width);
    src_y += src_stride_y;
    src_u += src_stride_u;
    src_v += src_stride_v;
    dst_uyvy += dst_stride_uyvy;
  }
  return 0;
}

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif
