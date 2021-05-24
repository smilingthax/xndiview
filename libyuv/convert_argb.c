/*
 *  Copyright 2011 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "libyuv/convert_argb.h"

#include "libyuv/cpu_id.h"
#include "libyuv/planar_functions.h"  // For CopyPlane and ARGBShuffle.
#include "libyuv/row.h"

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

// Copy ARGB with optional flipping
LIBYUV_API
int ARGBCopy(const uint8_t* src_argb,
             int src_stride_argb,
             uint8_t* dst_argb,
             int dst_stride_argb,
             int width,
             int height) {
  if (!src_argb || !dst_argb || width <= 0 || height == 0) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    src_argb = src_argb + (height - 1) * src_stride_argb;
    src_stride_argb = -src_stride_argb;
  }

  CopyPlane(src_argb, src_stride_argb, dst_argb, dst_stride_argb, width * 4,
            height);
  return 0;
}

// Convert I422 to ARGB with matrix.
LIBYUV_API
int I422ToARGBMatrix(const uint8_t* src_y,
                     int src_stride_y,
                     const uint8_t* src_u,
                     int src_stride_u,
                     const uint8_t* src_v,
                     int src_stride_v,
                     uint8_t* dst_argb,
                     int dst_stride_argb,
                     const struct YuvConstants* yuvconstants,
                     int width,
                     int height) {
  int y;
  void (*I422ToARGBRow)(const uint8_t* y_buf, const uint8_t* u_buf,
                        const uint8_t* v_buf, uint8_t* rgb_buf,
                        const struct YuvConstants* yuvconstants, int width) =
      I422ToARGBRow_C;
  if (!src_y || !src_u || !src_v || !dst_argb || width <= 0 || height == 0) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    dst_argb = dst_argb + (height - 1) * dst_stride_argb;
    dst_stride_argb = -dst_stride_argb;
  }
  // Coalesce rows.
  if (src_stride_y == width && src_stride_u * 2 == width &&
      src_stride_v * 2 == width && dst_stride_argb == width * 4) {
    width *= height;
    height = 1;
    src_stride_y = src_stride_u = src_stride_v = dst_stride_argb = 0;
  }
#if defined(HAS_I422TOARGBROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3)) {
    I422ToARGBRow = I422ToARGBRow_Any_SSSE3;
    if (IS_ALIGNED(width, 8)) {
      I422ToARGBRow = I422ToARGBRow_SSSE3;
    }
  }
#endif
#if defined(HAS_I422TOARGBROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    I422ToARGBRow = I422ToARGBRow_Any_AVX2;
    if (IS_ALIGNED(width, 16)) {
      I422ToARGBRow = I422ToARGBRow_AVX2;
    }
  }
#endif
#if defined(HAS_I422TOARGBROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    I422ToARGBRow = I422ToARGBRow_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      I422ToARGBRow = I422ToARGBRow_NEON;
    }
  }
#endif
#if defined(HAS_I422TOARGBROW_MMI)
  if (TestCpuFlag(kCpuHasMMI)) {
    I422ToARGBRow = I422ToARGBRow_Any_MMI;
    if (IS_ALIGNED(width, 4)) {
      I422ToARGBRow = I422ToARGBRow_MMI;
    }
  }
#endif
#if defined(HAS_I422TOARGBROW_MSA)
  if (TestCpuFlag(kCpuHasMSA)) {
    I422ToARGBRow = I422ToARGBRow_Any_MSA;
    if (IS_ALIGNED(width, 8)) {
      I422ToARGBRow = I422ToARGBRow_MSA;
    }
  }
#endif

  for (y = 0; y < height; ++y) {
    I422ToARGBRow(src_y, src_u, src_v, dst_argb, yuvconstants, width);
    dst_argb += dst_stride_argb;
    src_y += src_stride_y;
    src_u += src_stride_u;
    src_v += src_stride_v;
  }
  return 0;
}

// Convert I422 with Alpha to preattenuated ARGB with matrix.
LIBYUV_API
int I422AlphaToARGBMatrix(const uint8_t* src_y,
                          int src_stride_y,
                          const uint8_t* src_u,
                          int src_stride_u,
                          const uint8_t* src_v,
                          int src_stride_v,
                          const uint8_t* src_a,
                          int src_stride_a,
                          uint8_t* dst_argb,
                          int dst_stride_argb,
                          const struct YuvConstants* yuvconstants,
                          int width,
                          int height,
                          int attenuate) {
  int y;
  void (*I422AlphaToARGBRow)(const uint8_t* y_buf, const uint8_t* u_buf,
                             const uint8_t* v_buf, const uint8_t* a_buf,
                             uint8_t* dst_argb,
                             const struct YuvConstants* yuvconstants,
                             int width) = I422AlphaToARGBRow_C;
  void (*ARGBAttenuateRow)(const uint8_t* src_argb, uint8_t* dst_argb,
                           int width) = ARGBAttenuateRow_C;
  if (!src_y || !src_u || !src_v || !dst_argb || width <= 0 || height == 0) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    dst_argb = dst_argb + (height - 1) * dst_stride_argb;
    dst_stride_argb = -dst_stride_argb;
  }
#if defined(HAS_I422ALPHATOARGBROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3)) {
    I422AlphaToARGBRow = I422AlphaToARGBRow_Any_SSSE3;
    if (IS_ALIGNED(width, 8)) {
      I422AlphaToARGBRow = I422AlphaToARGBRow_SSSE3;
    }
  }
#endif
#if defined(HAS_I422ALPHATOARGBROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    I422AlphaToARGBRow = I422AlphaToARGBRow_Any_AVX2;
    if (IS_ALIGNED(width, 16)) {
      I422AlphaToARGBRow = I422AlphaToARGBRow_AVX2;
    }
  }
#endif
#if defined(HAS_I422ALPHATOARGBROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    I422AlphaToARGBRow = I422AlphaToARGBRow_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      I422AlphaToARGBRow = I422AlphaToARGBRow_NEON;
    }
  }
#endif
#if defined(HAS_I422ALPHATOARGBROW_MMI)
  if (TestCpuFlag(kCpuHasMMI)) {
    I422AlphaToARGBRow = I422AlphaToARGBRow_Any_MMI;
    if (IS_ALIGNED(width, 4)) {
      I422AlphaToARGBRow = I422AlphaToARGBRow_MMI;
    }
  }
#endif
#if defined(HAS_I422ALPHATOARGBROW_MSA)
  if (TestCpuFlag(kCpuHasMSA)) {
    I422AlphaToARGBRow = I422AlphaToARGBRow_Any_MSA;
    if (IS_ALIGNED(width, 8)) {
      I422AlphaToARGBRow = I422AlphaToARGBRow_MSA;
    }
  }
#endif
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
    I422AlphaToARGBRow(src_y, src_u, src_v, src_a, dst_argb, yuvconstants,
                       width);
    if (attenuate) {
      ARGBAttenuateRow(dst_argb, dst_argb, width);
    }
    dst_argb += dst_stride_argb;
    src_a += src_stride_a;
    src_y += src_stride_y;
    src_u += src_stride_u;
    src_v += src_stride_v;
  }
  return 0;
}

// Shuffle table for converting BGRA to ARGB.
static const uvec8 kShuffleMaskBGRAToARGB = {
    3u, 2u, 1u, 0u, 7u, 6u, 5u, 4u, 11u, 10u, 9u, 8u, 15u, 14u, 13u, 12u};

// Shuffle table for converting ABGR to ARGB.
static const uvec8 kShuffleMaskABGRToARGB = {
    2u, 1u, 0u, 3u, 6u, 5u, 4u, 7u, 10u, 9u, 8u, 11u, 14u, 13u, 12u, 15u};

// Shuffle table for converting RGBA to ARGB.
static const uvec8 kShuffleMaskRGBAToARGB = {
    1u, 2u, 3u, 0u, 5u, 6u, 7u, 4u, 9u, 10u, 11u, 8u, 13u, 14u, 15u, 12u};

// Convert BGRA to ARGB.
LIBYUV_API
int BGRAToARGB(const uint8_t* src_bgra,
               int src_stride_bgra,
               uint8_t* dst_argb,
               int dst_stride_argb,
               int width,
               int height) {
  return ARGBShuffle(src_bgra, src_stride_bgra, dst_argb, dst_stride_argb,
                     (const uint8_t*)&kShuffleMaskBGRAToARGB, width, height);
}

// Convert ABGR to ARGB.
LIBYUV_API
int ABGRToARGB(const uint8_t* src_abgr,
               int src_stride_abgr,
               uint8_t* dst_argb,
               int dst_stride_argb,
               int width,
               int height) {
  return ARGBShuffle(src_abgr, src_stride_abgr, dst_argb, dst_stride_argb,
                     (const uint8_t*)&kShuffleMaskABGRToARGB, width, height);
}

// Convert RGBA to ARGB.
LIBYUV_API
int RGBAToARGB(const uint8_t* src_rgba,
               int src_stride_rgba,
               uint8_t* dst_argb,
               int dst_stride_argb,
               int width,
               int height) {
  return ARGBShuffle(src_rgba, src_stride_rgba, dst_argb, dst_stride_argb,
                     (const uint8_t*)&kShuffleMaskRGBAToARGB, width, height);
}

// Convert RGB24 to ARGB.
LIBYUV_API
int RGB24ToARGB(const uint8_t* src_rgb24,
                int src_stride_rgb24,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height) {
  int y;
  void (*RGB24ToARGBRow)(const uint8_t* src_rgb, uint8_t* dst_argb, int width) =
      RGB24ToARGBRow_C;
  if (!src_rgb24 || !dst_argb || width <= 0 || height == 0) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    src_rgb24 = src_rgb24 + (height - 1) * src_stride_rgb24;
    src_stride_rgb24 = -src_stride_rgb24;
  }
  // Coalesce rows.
  if (src_stride_rgb24 == width * 3 && dst_stride_argb == width * 4) {
    width *= height;
    height = 1;
    src_stride_rgb24 = dst_stride_argb = 0;
  }
#if defined(HAS_RGB24TOARGBROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3)) {
    RGB24ToARGBRow = RGB24ToARGBRow_Any_SSSE3;
    if (IS_ALIGNED(width, 16)) {
      RGB24ToARGBRow = RGB24ToARGBRow_SSSE3;
    }
  }
#endif
#if defined(HAS_RGB24TOARGBROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    RGB24ToARGBRow = RGB24ToARGBRow_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      RGB24ToARGBRow = RGB24ToARGBRow_NEON;
    }
  }
#endif
#if defined(HAS_RGB24TOARGBROW_MMI)
  if (TestCpuFlag(kCpuHasMMI)) {
    RGB24ToARGBRow = RGB24ToARGBRow_Any_MMI;
    if (IS_ALIGNED(width, 4)) {
      RGB24ToARGBRow = RGB24ToARGBRow_MMI;
    }
  }
#endif
#if defined(HAS_RGB24TOARGBROW_MSA)
  if (TestCpuFlag(kCpuHasMSA)) {
    RGB24ToARGBRow = RGB24ToARGBRow_Any_MSA;
    if (IS_ALIGNED(width, 16)) {
      RGB24ToARGBRow = RGB24ToARGBRow_MSA;
    }
  }
#endif

  for (y = 0; y < height; ++y) {
    RGB24ToARGBRow(src_rgb24, dst_argb, width);
    src_rgb24 += src_stride_rgb24;
    dst_argb += dst_stride_argb;
  }
  return 0;
}

// Convert RAW to ARGB.
LIBYUV_API
int RAWToARGB(const uint8_t* src_raw,
              int src_stride_raw,
              uint8_t* dst_argb,
              int dst_stride_argb,
              int width,
              int height) {
  int y;
  void (*RAWToARGBRow)(const uint8_t* src_rgb, uint8_t* dst_argb, int width) =
      RAWToARGBRow_C;
  if (!src_raw || !dst_argb || width <= 0 || height == 0) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    src_raw = src_raw + (height - 1) * src_stride_raw;
    src_stride_raw = -src_stride_raw;
  }
  // Coalesce rows.
  if (src_stride_raw == width * 3 && dst_stride_argb == width * 4) {
    width *= height;
    height = 1;
    src_stride_raw = dst_stride_argb = 0;
  }
#if defined(HAS_RAWTOARGBROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3)) {
    RAWToARGBRow = RAWToARGBRow_Any_SSSE3;
    if (IS_ALIGNED(width, 16)) {
      RAWToARGBRow = RAWToARGBRow_SSSE3;
    }
  }
#endif
#if defined(HAS_RAWTOARGBROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    RAWToARGBRow = RAWToARGBRow_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      RAWToARGBRow = RAWToARGBRow_NEON;
    }
  }
#endif
#if defined(HAS_RAWTOARGBROW_MMI)
  if (TestCpuFlag(kCpuHasMMI)) {
    RAWToARGBRow = RAWToARGBRow_Any_MMI;
    if (IS_ALIGNED(width, 4)) {
      RAWToARGBRow = RAWToARGBRow_MMI;
    }
  }
#endif
#if defined(HAS_RAWTOARGBROW_MSA)
  if (TestCpuFlag(kCpuHasMSA)) {
    RAWToARGBRow = RAWToARGBRow_Any_MSA;
    if (IS_ALIGNED(width, 16)) {
      RAWToARGBRow = RAWToARGBRow_MSA;
    }
  }
#endif

  for (y = 0; y < height; ++y) {
    RAWToARGBRow(src_raw, dst_argb, width);
    src_raw += src_stride_raw;
    dst_argb += dst_stride_argb;
  }
  return 0;
}

// Convert RAW to RGBA.
LIBYUV_API
int RAWToRGBA(const uint8_t* src_raw,
              int src_stride_raw,
              uint8_t* dst_rgba,
              int dst_stride_rgba,
              int width,
              int height) {
  int y;
  void (*RAWToRGBARow)(const uint8_t* src_rgb, uint8_t* dst_rgba, int width) =
      RAWToRGBARow_C;
  if (!src_raw || !dst_rgba || width <= 0 || height == 0) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    src_raw = src_raw + (height - 1) * src_stride_raw;
    src_stride_raw = -src_stride_raw;
  }
  // Coalesce rows.
  if (src_stride_raw == width * 3 && dst_stride_rgba == width * 4) {
    width *= height;
    height = 1;
    src_stride_raw = dst_stride_rgba = 0;
  }
#if defined(HAS_RAWTORGBAROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3)) {
    RAWToRGBARow = RAWToRGBARow_Any_SSSE3;
    if (IS_ALIGNED(width, 16)) {
      RAWToRGBARow = RAWToRGBARow_SSSE3;
    }
  }
#endif
#if defined(HAS_RAWTORGBAROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    RAWToRGBARow = RAWToRGBARow_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      RAWToRGBARow = RAWToRGBARow_NEON;
    }
  }
#endif

  for (y = 0; y < height; ++y) {
    RAWToRGBARow(src_raw, dst_rgba, width);
    src_raw += src_stride_raw;
    dst_rgba += dst_stride_rgba;
  }
  return 0;
}

// Convert UYVY to ARGB with matrix.
LIBYUV_API
int UYVYToARGBMatrix(const uint8_t* src_uyvy,
                     int src_stride_uyvy,
                     uint8_t* dst_argb,
                     int dst_stride_argb,
                     const struct YuvConstants* yuvconstants,
                     int width,
                     int height) {
  int y;
  void (*UYVYToARGBRow)(const uint8_t* src_uyvy, uint8_t* dst_argb,
                        const struct YuvConstants* yuvconstants, int width) =
      UYVYToARGBRow_C;
  if (!src_uyvy || !dst_argb || width <= 0 || height == 0) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    src_uyvy = src_uyvy + (height - 1) * src_stride_uyvy;
    src_stride_uyvy = -src_stride_uyvy;
  }
  // Coalesce rows.
  if (src_stride_uyvy == width * 2 && dst_stride_argb == width * 4) {
    width *= height;
    height = 1;
    src_stride_uyvy = dst_stride_argb = 0;
  }
#if defined(HAS_UYVYTOARGBROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3)) {
    UYVYToARGBRow = UYVYToARGBRow_Any_SSSE3;
    if (IS_ALIGNED(width, 16)) {
      UYVYToARGBRow = UYVYToARGBRow_SSSE3;
    }
  }
#endif
#if defined(HAS_UYVYTOARGBROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    UYVYToARGBRow = UYVYToARGBRow_Any_AVX2;
    if (IS_ALIGNED(width, 32)) {
      UYVYToARGBRow = UYVYToARGBRow_AVX2;
    }
  }
#endif
#if defined(HAS_UYVYTOARGBROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    UYVYToARGBRow = UYVYToARGBRow_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      UYVYToARGBRow = UYVYToARGBRow_NEON;
    }
  }
#endif
#if defined(HAS_UYVYTOARGBROW_MMI)
  if (TestCpuFlag(kCpuHasMMI)) {
    UYVYToARGBRow = UYVYToARGBRow_Any_MMI;
    if (IS_ALIGNED(width, 4)) {
      UYVYToARGBRow = UYVYToARGBRow_MMI;
    }
  }
#endif
#if defined(HAS_UYVYTOARGBROW_MSA)
  if (TestCpuFlag(kCpuHasMSA)) {
    UYVYToARGBRow = UYVYToARGBRow_Any_MSA;
    if (IS_ALIGNED(width, 8)) {
      UYVYToARGBRow = UYVYToARGBRow_MSA;
    }
  }
#endif
  for (y = 0; y < height; ++y) {
    UYVYToARGBRow(src_uyvy, dst_argb, yuvconstants, width);
    src_uyvy += src_stride_uyvy;
    dst_argb += dst_stride_argb;
  }
  return 0;
}

// Convert I422 to RGBA with matrix.
LIBYUV_API
int I422ToRGBAMatrix(const uint8_t* src_y,
                     int src_stride_y,
                     const uint8_t* src_u,
                     int src_stride_u,
                     const uint8_t* src_v,
                     int src_stride_v,
                     uint8_t* dst_rgba,
                     int dst_stride_rgba,
                     const struct YuvConstants* yuvconstants,
                     int width,
                     int height) {
  int y;
  void (*I422ToRGBARow)(const uint8_t* y_buf, const uint8_t* u_buf,
                        const uint8_t* v_buf, uint8_t* rgb_buf,
                        const struct YuvConstants* yuvconstants, int width) =
      I422ToRGBARow_C;
  if (!src_y || !src_u || !src_v || !dst_rgba || width <= 0 || height == 0) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    dst_rgba = dst_rgba + (height - 1) * dst_stride_rgba;
    dst_stride_rgba = -dst_stride_rgba;
  }
#if defined(HAS_I422TORGBAROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3)) {
    I422ToRGBARow = I422ToRGBARow_Any_SSSE3;
    if (IS_ALIGNED(width, 8)) {
      I422ToRGBARow = I422ToRGBARow_SSSE3;
    }
  }
#endif
#if defined(HAS_I422TORGBAROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    I422ToRGBARow = I422ToRGBARow_Any_AVX2;
    if (IS_ALIGNED(width, 16)) {
      I422ToRGBARow = I422ToRGBARow_AVX2;
    }
  }
#endif
#if defined(HAS_I422TORGBAROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    I422ToRGBARow = I422ToRGBARow_Any_NEON;
    if (IS_ALIGNED(width, 8)) {
      I422ToRGBARow = I422ToRGBARow_NEON;
    }
  }
#endif
#if defined(HAS_I422TORGBAROW_MMI)
  if (TestCpuFlag(kCpuHasMMI)) {
    I422ToRGBARow = I422ToRGBARow_Any_MMI;
    if (IS_ALIGNED(width, 4)) {
      I422ToRGBARow = I422ToRGBARow_MMI;
    }
  }
#endif
#if defined(HAS_I422TORGBAROW_MSA)
  if (TestCpuFlag(kCpuHasMSA)) {
    I422ToRGBARow = I422ToRGBARow_Any_MSA;
    if (IS_ALIGNED(width, 8)) {
      I422ToRGBARow = I422ToRGBARow_MSA;
    }
  }
#endif

  for (y = 0; y < height; ++y) {
    I422ToRGBARow(src_y, src_u, src_v, dst_rgba, yuvconstants, width);
    dst_rgba += dst_stride_rgba;
    src_y += src_stride_y;
    src_u += src_stride_u;
    src_v += src_stride_v;
  }
  return 0;
}

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif
