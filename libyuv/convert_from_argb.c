/*
 *  Copyright 2012 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "libyuv/convert_from_argb.h"

#include "libyuv/basic_types.h"
#include "libyuv/cpu_id.h"
#include "libyuv/planar_functions.h"
#include "libyuv/row.h"

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

// ARGB little endian (bgra in memory) to I422
LIBYUV_API
int ARGBToI422(const uint8_t* src_argb,
               int src_stride_argb,
               uint8_t* dst_y,
               int dst_stride_y,
               uint8_t* dst_u,
               int dst_stride_u,
               uint8_t* dst_v,
               int dst_stride_v,
               int width,
               int height) {
  int y;
  void (*ARGBToUVRow)(const uint8_t* src_argb0, int src_stride_argb,
                      uint8_t* dst_u, uint8_t* dst_v, int width) =
      ARGBToUVRow_C;
  void (*ARGBToYRow)(const uint8_t* src_argb, uint8_t* dst_y, int width) =
      ARGBToYRow_C;
  if (!src_argb || !dst_y || !dst_u || !dst_v || width <= 0 || height == 0) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    src_argb = src_argb + (height - 1) * src_stride_argb;
    src_stride_argb = -src_stride_argb;
  }
  // Coalesce rows.
  if (src_stride_argb == width * 4 && dst_stride_y == width &&
      dst_stride_u * 2 == width && dst_stride_v * 2 == width) {
    width *= height;
    height = 1;
    src_stride_argb = dst_stride_y = dst_stride_u = dst_stride_v = 0;
  }
#if defined(HAS_ARGBTOYROW_SSSE3) && defined(HAS_ARGBTOUVROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3)) {
    ARGBToUVRow = ARGBToUVRow_Any_SSSE3;
    ARGBToYRow = ARGBToYRow_Any_SSSE3;
    if (IS_ALIGNED(width, 16)) {
      ARGBToUVRow = ARGBToUVRow_SSSE3;
      ARGBToYRow = ARGBToYRow_SSSE3;
    }
  }
#endif
#if defined(HAS_ARGBTOYROW_AVX2) && defined(HAS_ARGBTOUVROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    ARGBToUVRow = ARGBToUVRow_Any_AVX2;
    ARGBToYRow = ARGBToYRow_Any_AVX2;
    if (IS_ALIGNED(width, 32)) {
      ARGBToUVRow = ARGBToUVRow_AVX2;
      ARGBToYRow = ARGBToYRow_AVX2;
    }
  }
#endif
#if defined(HAS_ARGBTOYROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    ARGBToYRow = ARGBToYRow_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      ARGBToYRow = ARGBToYRow_NEON;
    }
  }
#endif
#if defined(HAS_ARGBTOUVROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    ARGBToUVRow = ARGBToUVRow_Any_NEON;
    if (IS_ALIGNED(width, 16)) {
      ARGBToUVRow = ARGBToUVRow_NEON;
    }
  }
#endif

#if defined(HAS_ARGBTOYROW_MMI) && defined(HAS_ARGBTOUVROW_MMI)
  if (TestCpuFlag(kCpuHasMMI)) {
    ARGBToYRow = ARGBToYRow_Any_MMI;
    ARGBToUVRow = ARGBToUVRow_Any_MMI;
    if (IS_ALIGNED(width, 8)) {
      ARGBToYRow = ARGBToYRow_MMI;
    }
    if (IS_ALIGNED(width, 16)) {
      ARGBToUVRow = ARGBToUVRow_MMI;
    }
  }
#endif

#if defined(HAS_ARGBTOYROW_MSA) && defined(HAS_ARGBTOUVROW_MSA)
  if (TestCpuFlag(kCpuHasMSA)) {
    ARGBToYRow = ARGBToYRow_Any_MSA;
    ARGBToUVRow = ARGBToUVRow_Any_MSA;
    if (IS_ALIGNED(width, 16)) {
      ARGBToYRow = ARGBToYRow_MSA;
    }
    if (IS_ALIGNED(width, 32)) {
      ARGBToUVRow = ARGBToUVRow_MSA;
    }
  }
#endif

  for (y = 0; y < height; ++y) {
    ARGBToUVRow(src_argb, 0, dst_u, dst_v, width);
    ARGBToYRow(src_argb, dst_y, width);
    src_argb += src_stride_argb;
    dst_y += dst_stride_y;
    dst_u += dst_stride_u;
    dst_v += dst_stride_v;
  }
  return 0;
}

// Convert ARGB to UYVY.
LIBYUV_API
int ARGBToUYVY(const uint8_t* src_argb,
               int src_stride_argb,
               uint8_t* dst_uyvy,
               int dst_stride_uyvy,
               int width,
               int height) {
  int y;
  void (*ARGBToUVRow)(const uint8_t* src_argb, int src_stride_argb,
                      uint8_t* dst_u, uint8_t* dst_v, int width) =
      ARGBToUVRow_C;
  void (*ARGBToYRow)(const uint8_t* src_argb, uint8_t* dst_y, int width) =
      ARGBToYRow_C;
  void (*I422ToUYVYRow)(const uint8_t* src_y, const uint8_t* src_u,
                        const uint8_t* src_v, uint8_t* dst_uyvy, int width) =
      I422ToUYVYRow_C;

  if (!src_argb || !dst_uyvy || width <= 0 || height == 0) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    dst_uyvy = dst_uyvy + (height - 1) * dst_stride_uyvy;
    dst_stride_uyvy = -dst_stride_uyvy;
  }
  // Coalesce rows.
  if (src_stride_argb == width * 4 && dst_stride_uyvy == width * 2) {
    width *= height;
    height = 1;
    src_stride_argb = dst_stride_uyvy = 0;
  }
#if defined(HAS_ARGBTOYROW_SSSE3) && defined(HAS_ARGBTOUVROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3)) {
    ARGBToUVRow = ARGBToUVRow_Any_SSSE3;
    ARGBToYRow = ARGBToYRow_Any_SSSE3;
    if (IS_ALIGNED(width, 16)) {
      ARGBToUVRow = ARGBToUVRow_SSSE3;
      ARGBToYRow = ARGBToYRow_SSSE3;
    }
  }
#endif
#if defined(HAS_ARGBTOYROW_AVX2) && defined(HAS_ARGBTOUVROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    ARGBToUVRow = ARGBToUVRow_Any_AVX2;
    ARGBToYRow = ARGBToYRow_Any_AVX2;
    if (IS_ALIGNED(width, 32)) {
      ARGBToUVRow = ARGBToUVRow_AVX2;
      ARGBToYRow = ARGBToYRow_AVX2;
    }
  }
#endif
#if defined(HAS_ARGBTOYROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    ARGBToYRow = ARGBToYRow_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      ARGBToYRow = ARGBToYRow_NEON;
    }
  }
#endif
#if defined(HAS_ARGBTOUVROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    ARGBToUVRow = ARGBToUVRow_Any_NEON;
    if (IS_ALIGNED(width, 16)) {
      ARGBToUVRow = ARGBToUVRow_NEON;
    }
  }
#endif
#if defined(HAS_ARGBTOYROW_MMI) && defined(HAS_ARGBTOUVROW_MMI)
  if (TestCpuFlag(kCpuHasMMI)) {
    ARGBToYRow = ARGBToYRow_Any_MMI;
    ARGBToUVRow = ARGBToUVRow_Any_MMI;
    if (IS_ALIGNED(width, 8)) {
      ARGBToYRow = ARGBToYRow_MMI;
    }
    if (IS_ALIGNED(width, 16)) {
      ARGBToUVRow = ARGBToUVRow_MMI;
    }
  }
#endif
#if defined(HAS_ARGBTOYROW_MSA) && defined(HAS_ARGBTOUVROW_MSA)
  if (TestCpuFlag(kCpuHasMSA)) {
    ARGBToYRow = ARGBToYRow_Any_MSA;
    ARGBToUVRow = ARGBToUVRow_Any_MSA;
    if (IS_ALIGNED(width, 16)) {
      ARGBToYRow = ARGBToYRow_MSA;
    }
    if (IS_ALIGNED(width, 32)) {
      ARGBToUVRow = ARGBToUVRow_MSA;
    }
  }
#endif
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

  {
    // Allocate a rows of yuv.
    align_buffer_64(row_y, ((width + 63) & ~63) * 2);
    uint8_t* row_u = row_y + ((width + 63) & ~63);
    uint8_t* row_v = row_u + ((width + 63) & ~63) / 2;

    for (y = 0; y < height; ++y) {
      ARGBToUVRow(src_argb, 0, row_u, row_v, width);
      ARGBToYRow(src_argb, row_y, width);
      I422ToUYVYRow(row_y, row_u, row_v, dst_uyvy, width);
      src_argb += src_stride_argb;
      dst_uyvy += dst_stride_uyvy;
    }

    free_aligned_buffer_64(row_y);
  }
  return 0;
}

// Shuffle table for converting ARGB to RGBA.
static const uvec8 kShuffleMaskARGBToRGBA = {
    3u, 0u, 1u, 2u, 7u, 4u, 5u, 6u, 11u, 8u, 9u, 10u, 15u, 12u, 13u, 14u};

// Convert ARGB to RGBA.
LIBYUV_API
int ARGBToRGBA(const uint8_t* src_argb,
               int src_stride_argb,
               uint8_t* dst_rgba,
               int dst_stride_rgba,
               int width,
               int height) {
  return ARGBShuffle(src_argb, src_stride_argb, dst_rgba, dst_stride_rgba,
                     (const uint8_t*)(&kShuffleMaskARGBToRGBA), width, height);
}

// Convert ARGB To RGB24.
LIBYUV_API
int ARGBToRGB24(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_rgb24,
                int dst_stride_rgb24,
                int width,
                int height) {
  int y;
  void (*ARGBToRGB24Row)(const uint8_t* src_argb, uint8_t* dst_rgb, int width) =
      ARGBToRGB24Row_C;
  if (!src_argb || !dst_rgb24 || width <= 0 || height == 0) {
    return -1;
  }
  if (height < 0) {
    height = -height;
    src_argb = src_argb + (height - 1) * src_stride_argb;
    src_stride_argb = -src_stride_argb;
  }
  // Coalesce rows.
  if (src_stride_argb == width * 4 && dst_stride_rgb24 == width * 3) {
    width *= height;
    height = 1;
    src_stride_argb = dst_stride_rgb24 = 0;
  }
#if defined(HAS_ARGBTORGB24ROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3)) {
    ARGBToRGB24Row = ARGBToRGB24Row_Any_SSSE3;
    if (IS_ALIGNED(width, 16)) {
      ARGBToRGB24Row = ARGBToRGB24Row_SSSE3;
    }
  }
#endif
#if defined(HAS_ARGBTORGB24ROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    ARGBToRGB24Row = ARGBToRGB24Row_Any_AVX2;
    if (IS_ALIGNED(width, 32)) {
      ARGBToRGB24Row = ARGBToRGB24Row_AVX2;
    }
  }
#endif
#if defined(HAS_ARGBTORGB24ROW_AVX512VBMI)
  if (TestCpuFlag(kCpuHasAVX512VBMI)) {
    ARGBToRGB24Row = ARGBToRGB24Row_Any_AVX512VBMI;
    if (IS_ALIGNED(width, 32)) {
      ARGBToRGB24Row = ARGBToRGB24Row_AVX512VBMI;
    }
  }
#endif
#if defined(HAS_ARGBTORGB24ROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    ARGBToRGB24Row = ARGBToRGB24Row_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      ARGBToRGB24Row = ARGBToRGB24Row_NEON;
    }
  }
#endif
#if defined(HAS_ARGBTORGB24ROW_MMI)
  if (TestCpuFlag(kCpuHasMMI)) {
    ARGBToRGB24Row = ARGBToRGB24Row_Any_MMI;
    if (IS_ALIGNED(width, 4)) {
      ARGBToRGB24Row = ARGBToRGB24Row_MMI;
    }
  }
#endif
#if defined(HAS_ARGBTORGB24ROW_MSA)
  if (TestCpuFlag(kCpuHasMSA)) {
    ARGBToRGB24Row = ARGBToRGB24Row_Any_MSA;
    if (IS_ALIGNED(width, 16)) {
      ARGBToRGB24Row = ARGBToRGB24Row_MSA;
    }
  }
#endif

  for (y = 0; y < height; ++y) {
    ARGBToRGB24Row(src_argb, dst_rgb24, width);
    src_argb += src_stride_argb;
    dst_rgb24 += dst_stride_rgb24;
  }
  return 0;
}

// Convert ARGB To RAW.
LIBYUV_API
int ARGBToRAW(const uint8_t* src_argb,
              int src_stride_argb,
              uint8_t* dst_raw,
              int dst_stride_raw,
              int width,
              int height) {
  int y;
  void (*ARGBToRAWRow)(const uint8_t* src_argb, uint8_t* dst_rgb, int width) =
      ARGBToRAWRow_C;
  if (!src_argb || !dst_raw || width <= 0 || height == 0) {
    return -1;
  }
  if (height < 0) {
    height = -height;
    src_argb = src_argb + (height - 1) * src_stride_argb;
    src_stride_argb = -src_stride_argb;
  }
  // Coalesce rows.
  if (src_stride_argb == width * 4 && dst_stride_raw == width * 3) {
    width *= height;
    height = 1;
    src_stride_argb = dst_stride_raw = 0;
  }
#if defined(HAS_ARGBTORAWROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3)) {
    ARGBToRAWRow = ARGBToRAWRow_Any_SSSE3;
    if (IS_ALIGNED(width, 16)) {
      ARGBToRAWRow = ARGBToRAWRow_SSSE3;
    }
  }
#endif
#if defined(HAS_ARGBTORAWROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    ARGBToRAWRow = ARGBToRAWRow_Any_AVX2;
    if (IS_ALIGNED(width, 32)) {
      ARGBToRAWRow = ARGBToRAWRow_AVX2;
    }
  }
#endif
#if defined(HAS_ARGBTORAWROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    ARGBToRAWRow = ARGBToRAWRow_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      ARGBToRAWRow = ARGBToRAWRow_NEON;
    }
  }
#endif
#if defined(HAS_ARGBTORAWROW_MMI)
  if (TestCpuFlag(kCpuHasMMI)) {
    ARGBToRAWRow = ARGBToRAWRow_Any_MMI;
    if (IS_ALIGNED(width, 4)) {
      ARGBToRAWRow = ARGBToRAWRow_MMI;
    }
  }
#endif
#if defined(HAS_ARGBTORAWROW_MSA)
  if (TestCpuFlag(kCpuHasMSA)) {
    ARGBToRAWRow = ARGBToRAWRow_Any_MSA;
    if (IS_ALIGNED(width, 16)) {
      ARGBToRAWRow = ARGBToRAWRow_MSA;
    }
  }
#endif

  for (y = 0; y < height; ++y) {
    ARGBToRAWRow(src_argb, dst_raw, width);
    src_argb += src_stride_argb;
    dst_raw += dst_stride_raw;
  }
  return 0;
}

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif
