/*
 *  Copyright 2011 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "libyuv/planar_functions.h"

#include <assert.h>
#include <string.h>  // for memset()

#include "libyuv/cpu_id.h"
#ifdef HAVE_JPEG
#include "libyuv/mjpeg_decoder.h"
#endif
#include "libyuv/row.h"
#include "libyuv/scale_row.h"  // for ScaleRowDown2

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

// Copy a plane of data
LIBYUV_API
void CopyPlane(const uint8_t* src_y,
               int src_stride_y,
               uint8_t* dst_y,
               int dst_stride_y,
               int width,
               int height) {
  int y;
  void (*CopyRow)(const uint8_t* src, uint8_t* dst, int width) = CopyRow_C;
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    dst_y = dst_y + (height - 1) * dst_stride_y;
    dst_stride_y = -dst_stride_y;
  }
  // Coalesce rows.
  if (src_stride_y == width && dst_stride_y == width) {
    width *= height;
    height = 1;
    src_stride_y = dst_stride_y = 0;
  }
  // Nothing to do.
  if (src_y == dst_y && src_stride_y == dst_stride_y) {
    return;
  }

#if defined(HAS_COPYROW_SSE2)
  if (TestCpuFlag(kCpuHasSSE2)) {
    CopyRow = IS_ALIGNED(width, 32) ? CopyRow_SSE2 : CopyRow_Any_SSE2;
  }
#endif
#if defined(HAS_COPYROW_AVX)
  if (TestCpuFlag(kCpuHasAVX)) {
    CopyRow = IS_ALIGNED(width, 64) ? CopyRow_AVX : CopyRow_Any_AVX;
  }
#endif
#if defined(HAS_COPYROW_ERMS)
  if (TestCpuFlag(kCpuHasERMS)) {
    CopyRow = CopyRow_ERMS;
  }
#endif
#if defined(HAS_COPYROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    CopyRow = IS_ALIGNED(width, 32) ? CopyRow_NEON : CopyRow_Any_NEON;
  }
#endif

  // Copy plane
  for (y = 0; y < height; ++y) {
    CopyRow(src_y, dst_y, width);
    src_y += src_stride_y;
    dst_y += dst_stride_y;
  }
}

// Convert UYVY to I422.
LIBYUV_API
int UYVYToI422(const uint8_t* src_uyvy,
               int src_stride_uyvy,
               uint8_t* dst_y,
               int dst_stride_y,
               uint8_t* dst_u,
               int dst_stride_u,
               uint8_t* dst_v,
               int dst_stride_v,
               int width,
               int height) {
  int y;
  void (*UYVYToUV422Row)(const uint8_t* src_uyvy, uint8_t* dst_u,
                         uint8_t* dst_v, int width) = UYVYToUV422Row_C;
  void (*UYVYToYRow)(const uint8_t* src_uyvy, uint8_t* dst_y, int width) =
      UYVYToYRow_C;
  if (!src_uyvy || !dst_y || !dst_u || !dst_v || width <= 0 || height == 0) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    src_uyvy = src_uyvy + (height - 1) * src_stride_uyvy;
    src_stride_uyvy = -src_stride_uyvy;
  }
  // Coalesce rows.
  if (src_stride_uyvy == width * 2 && dst_stride_y == width &&
      dst_stride_u * 2 == width && dst_stride_v * 2 == width &&
      width * height <= 32768) {
    width *= height;
    height = 1;
    src_stride_uyvy = dst_stride_y = dst_stride_u = dst_stride_v = 0;
  }
#if defined(HAS_UYVYTOYROW_SSE2)
  if (TestCpuFlag(kCpuHasSSE2)) {
    UYVYToUV422Row = UYVYToUV422Row_Any_SSE2;
    UYVYToYRow = UYVYToYRow_Any_SSE2;
    if (IS_ALIGNED(width, 16)) {
      UYVYToUV422Row = UYVYToUV422Row_SSE2;
      UYVYToYRow = UYVYToYRow_SSE2;
    }
  }
#endif
#if defined(HAS_UYVYTOYROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    UYVYToUV422Row = UYVYToUV422Row_Any_AVX2;
    UYVYToYRow = UYVYToYRow_Any_AVX2;
    if (IS_ALIGNED(width, 32)) {
      UYVYToUV422Row = UYVYToUV422Row_AVX2;
      UYVYToYRow = UYVYToYRow_AVX2;
    }
  }
#endif
#if defined(HAS_UYVYTOYROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    UYVYToYRow = UYVYToYRow_Any_NEON;
    UYVYToUV422Row = UYVYToUV422Row_Any_NEON;
    if (IS_ALIGNED(width, 16)) {
      UYVYToYRow = UYVYToYRow_NEON;
      UYVYToUV422Row = UYVYToUV422Row_NEON;
    }
  }
#endif
#if defined(HAS_UYVYTOYROW_MMI) && defined(HAS_UYVYTOUV422ROW_MMI)
  if (TestCpuFlag(kCpuHasMMI)) {
    UYVYToYRow = UYVYToYRow_Any_MMI;
    UYVYToUV422Row = UYVYToUV422Row_Any_MMI;
    if (IS_ALIGNED(width, 16)) {
      UYVYToYRow = UYVYToYRow_MMI;
      UYVYToUV422Row = UYVYToUV422Row_MMI;
    }
  }
#endif
#if defined(HAS_UYVYTOYROW_MSA) && defined(HAS_UYVYTOUV422ROW_MSA)
  if (TestCpuFlag(kCpuHasMSA)) {
    UYVYToYRow = UYVYToYRow_Any_MSA;
    UYVYToUV422Row = UYVYToUV422Row_Any_MSA;
    if (IS_ALIGNED(width, 32)) {
      UYVYToYRow = UYVYToYRow_MSA;
      UYVYToUV422Row = UYVYToUV422Row_MSA;
    }
  }
#endif

  for (y = 0; y < height; ++y) {
    UYVYToUV422Row(src_uyvy, dst_u, dst_v, width);
    UYVYToYRow(src_uyvy, dst_y, width);
    src_uyvy += src_stride_uyvy;
    dst_y += dst_stride_y;
    dst_u += dst_stride_u;
    dst_v += dst_stride_v;
  }
  return 0;
}

// Convert unattentuated ARGB to preattenuated ARGB.
// An unattenutated ARGB alpha blend uses the formula
// p = a * f + (1 - a) * b
// where
//   p is output pixel
//   f is foreground pixel
//   b is background pixel
//   a is alpha value from foreground pixel
// An preattenutated ARGB alpha blend uses the formula
// p = f + (1 - a) * b
// where
//   f is foreground pixel premultiplied by alpha

LIBYUV_API
int ARGBAttenuate(const uint8_t* src_argb,
                  int src_stride_argb,
                  uint8_t* dst_argb,
                  int dst_stride_argb,
                  int width,
                  int height) {
  int y;
  void (*ARGBAttenuateRow)(const uint8_t* src_argb, uint8_t* dst_argb,
                           int width) = ARGBAttenuateRow_C;
  if (!src_argb || !dst_argb || width <= 0 || height == 0) {
    return -1;
  }
  if (height < 0) {
    height = -height;
    src_argb = src_argb + (height - 1) * src_stride_argb;
    src_stride_argb = -src_stride_argb;
  }
  // Coalesce rows.
  if (src_stride_argb == width * 4 && dst_stride_argb == width * 4) {
    width *= height;
    height = 1;
    src_stride_argb = dst_stride_argb = 0;
  }
#if defined(HAS_ARGBATTENUATEROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3)) {
    ARGBAttenuateRow = ARGBAttenuateRow_Any_SSSE3;
    if (IS_ALIGNED(width, 4)) {
      ARGBAttenuateRow = ARGBAttenuateRow_SSSE3;
    }
  }
#endif
#if defined(HAS_ARGBATTENUATEROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    ARGBAttenuateRow = ARGBAttenuateRow_Any_AVX2;
    if (IS_ALIGNED(width, 8)) {
      ARGBAttenuateRow = ARGBAttenuateRow_AVX2;
    }
  }
#endif
#if defined(HAS_ARGBATTENUATEROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    ARGBAttenuateRow = ARGBAttenuateRow_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      ARGBAttenuateRow = ARGBAttenuateRow_NEON;
    }
  }
#endif
#if defined(HAS_ARGBATTENUATEROW_MMI)
  if (TestCpuFlag(kCpuHasMMI)) {
    ARGBAttenuateRow = ARGBAttenuateRow_Any_MMI;
    if (IS_ALIGNED(width, 2)) {
      ARGBAttenuateRow = ARGBAttenuateRow_MMI;
    }
  }
#endif
#if defined(HAS_ARGBATTENUATEROW_MSA)
  if (TestCpuFlag(kCpuHasMSA)) {
    ARGBAttenuateRow = ARGBAttenuateRow_Any_MSA;
    if (IS_ALIGNED(width, 8)) {
      ARGBAttenuateRow = ARGBAttenuateRow_MSA;
    }
  }
#endif

  for (y = 0; y < height; ++y) {
    ARGBAttenuateRow(src_argb, dst_argb, width);
    src_argb += src_stride_argb;
    dst_argb += dst_stride_argb;
  }
  return 0;
}

// Convert preattentuated ARGB to unattenuated ARGB.
LIBYUV_API
int ARGBUnattenuate(const uint8_t* src_argb,
                    int src_stride_argb,
                    uint8_t* dst_argb,
                    int dst_stride_argb,
                    int width,
                    int height) {
  int y;
  void (*ARGBUnattenuateRow)(const uint8_t* src_argb, uint8_t* dst_argb,
                             int width) = ARGBUnattenuateRow_C;
  if (!src_argb || !dst_argb || width <= 0 || height == 0) {
    return -1;
  }
  if (height < 0) {
    height = -height;
    src_argb = src_argb + (height - 1) * src_stride_argb;
    src_stride_argb = -src_stride_argb;
  }
  // Coalesce rows.
  if (src_stride_argb == width * 4 && dst_stride_argb == width * 4) {
    width *= height;
    height = 1;
    src_stride_argb = dst_stride_argb = 0;
  }
#if defined(HAS_ARGBUNATTENUATEROW_SSE2)
  if (TestCpuFlag(kCpuHasSSE2)) {
    ARGBUnattenuateRow = ARGBUnattenuateRow_Any_SSE2;
    if (IS_ALIGNED(width, 4)) {
      ARGBUnattenuateRow = ARGBUnattenuateRow_SSE2;
    }
  }
#endif
#if defined(HAS_ARGBUNATTENUATEROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    ARGBUnattenuateRow = ARGBUnattenuateRow_Any_AVX2;
    if (IS_ALIGNED(width, 8)) {
      ARGBUnattenuateRow = ARGBUnattenuateRow_AVX2;
    }
  }
#endif
  // TODO(fbarchard): Neon version.

  for (y = 0; y < height; ++y) {
    ARGBUnattenuateRow(src_argb, dst_argb, width);
    src_argb += src_stride_argb;
    dst_argb += dst_stride_argb;
  }
  return 0;
}

// Shuffle ARGB channel order.  e.g. BGRA to ARGB.
LIBYUV_API
int ARGBShuffle(const uint8_t* src_bgra,
                int src_stride_bgra,
                uint8_t* dst_argb,
                int dst_stride_argb,
                const uint8_t* shuffler,
                int width,
                int height) {
  int y;
  void (*ARGBShuffleRow)(const uint8_t* src_bgra, uint8_t* dst_argb,
                         const uint8_t* shuffler, int width) = ARGBShuffleRow_C;
  if (!src_bgra || !dst_argb || width <= 0 || height == 0) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    src_bgra = src_bgra + (height - 1) * src_stride_bgra;
    src_stride_bgra = -src_stride_bgra;
  }
  // Coalesce rows.
  if (src_stride_bgra == width * 4 && dst_stride_argb == width * 4) {
    width *= height;
    height = 1;
    src_stride_bgra = dst_stride_argb = 0;
  }
#if defined(HAS_ARGBSHUFFLEROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3)) {
    ARGBShuffleRow = ARGBShuffleRow_Any_SSSE3;
    if (IS_ALIGNED(width, 8)) {
      ARGBShuffleRow = ARGBShuffleRow_SSSE3;
    }
  }
#endif
#if defined(HAS_ARGBSHUFFLEROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    ARGBShuffleRow = ARGBShuffleRow_Any_AVX2;
    if (IS_ALIGNED(width, 16)) {
      ARGBShuffleRow = ARGBShuffleRow_AVX2;
    }
  }
#endif
#if defined(HAS_ARGBSHUFFLEROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    ARGBShuffleRow = ARGBShuffleRow_Any_NEON;
    if (IS_ALIGNED(width, 4)) {
      ARGBShuffleRow = ARGBShuffleRow_NEON;
    }
  }
#endif
#if defined(HAS_ARGBSHUFFLEROW_MMI)
  if (TestCpuFlag(kCpuHasMMI)) {
    ARGBShuffleRow = ARGBShuffleRow_Any_MMI;
    if (IS_ALIGNED(width, 2)) {
      ARGBShuffleRow = ARGBShuffleRow_MMI;
    }
  }
#endif
#if defined(HAS_ARGBSHUFFLEROW_MSA)
  if (TestCpuFlag(kCpuHasMSA)) {
    ARGBShuffleRow = ARGBShuffleRow_Any_MSA;
    if (IS_ALIGNED(width, 8)) {
      ARGBShuffleRow = ARGBShuffleRow_MSA;
    }
  }
#endif

  for (y = 0; y < height; ++y) {
    ARGBShuffleRow(src_bgra, dst_argb, shuffler, width);
    src_bgra += src_stride_bgra;
    dst_argb += dst_stride_argb;
  }
  return 0;
}

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif
