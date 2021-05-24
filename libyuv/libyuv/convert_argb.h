/*
 *  Copyright 2012 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef INCLUDE_LIBYUV_CONVERT_ARGB_H_
#define INCLUDE_LIBYUV_CONVERT_ARGB_H_

#include "libyuv/basic_types.h"

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

// Conversion matrix for YUV to RGB
LIBYUV_API extern const struct YuvConstants kYuvI601Constants;   // BT.601
LIBYUV_API extern const struct YuvConstants kYuvJPEGConstants;   // BT.601 full
LIBYUV_API extern const struct YuvConstants kYuvH709Constants;   // BT.709
LIBYUV_API extern const struct YuvConstants kYuvF709Constants;   // BT.709 full
LIBYUV_API extern const struct YuvConstants kYuv2020Constants;   // BT.2020
LIBYUV_API extern const struct YuvConstants kYuvV2020Constants;  // BT.2020 full

// Conversion matrix for YVU to BGR
LIBYUV_API extern const struct YuvConstants kYvuI601Constants;   // BT.601
LIBYUV_API extern const struct YuvConstants kYvuJPEGConstants;   // BT.601 full
LIBYUV_API extern const struct YuvConstants kYvuH709Constants;   // BT.709
LIBYUV_API extern const struct YuvConstants kYvuF709Constants;   // BT.709 full
LIBYUV_API extern const struct YuvConstants kYvu2020Constants;   // BT.2020
LIBYUV_API extern const struct YuvConstants kYvuV2020Constants;  // BT.2020 full

// Macros for end swapped destination Matrix conversions.
// Swap UV and pass mirrored kYvuJPEGConstants matrix.
#define kYuvI601ConstantsVU kYvuI601Constants
#define kYuvJPEGConstantsVU kYvuJPEGConstants
#define kYuvH709ConstantsVU kYvuH709Constants
#define kYuvF709ConstantsVU kYvuF709Constants
#define kYuv2020ConstantsVU kYvu2020Constants
#define kYuvV2020ConstantsVU kYvuV2020Constants

#define I422AlphaToABGRMatrix(a, b, c, d, e, f, g, h, i, j, k, l, m, n) \
  I422AlphaToARGBMatrix(a, b, e, f, c, d, g, h, i, j, k##VU, l, m, n)

  // NOTE(thobi): really in planar_functions.h, but impl in convert_argb.c
// Copy ARGB to ARGB.
#define ARGBToARGB ARGBCopy
LIBYUV_API
int ARGBCopy(const uint8_t* src_argb,
             int src_stride_argb,
             uint8_t* dst_argb,
             int dst_stride_argb,
             int width,
             int height);

// I422ToARGB  ->  kYuvI601Constants
// I422ToABGR  ->  kYvuI601Constants  // Use Yvu matrix

// J422ToARGB  ->  kYuvJPEGConstants
// J422ToABGR  ->  kYvuJPEGConstants

// H422ToARGB  ->  kYuvH709Constants
// H422ToABGR  ->  kYvuH709Constants

// F422, V422 ?    [cf. docs/formats.md]

// U422ToARGB  ->  kYuv2020Constants
// U422ToABGR  ->  kYvu2020Constants

// NOTE(thobi): original libyuv only has UYVYToARGB with fixed kYuvI601Constants ...
// TODO?(thobi): UYVY + Alpha -> preattenuated ARGB with matrix ?

// Convert UYVY to ARGB with matrix.
LIBYUV_API
int UYVYToARGBMatrix(const uint8_t* src_uyvy,
                     int src_stride_uyvy,
                     uint8_t* dst_argb,
                     int dst_stride_argb,
                     const struct YuvConstants* yuvconstants,
                     int width,
                     int height);

// BGRA little endian (argb in memory) to ARGB.
LIBYUV_API
int BGRAToARGB(const uint8_t* src_bgra,
               int src_stride_bgra,
               uint8_t* dst_argb,
               int dst_stride_argb,
               int width,
               int height);

// ARGB to BGRA (same as BGRAToARGB).
#define ARGBToBGRA BGRAToARGB

// ABGR little endian (rgba in memory) to ARGB.
LIBYUV_API
int ABGRToARGB(const uint8_t* src_abgr,
               int src_stride_abgr,
               uint8_t* dst_argb,
               int dst_stride_argb,
               int width,
               int height);

// ARGB to ABGR (same as ABGRToARGB).
#define ARGBToABGR ABGRToARGB

// RGBA little endian (abgr in memory) to ARGB.
LIBYUV_API
int RGBAToARGB(const uint8_t* src_rgba,
               int src_stride_rgba,
               uint8_t* dst_argb,
               int dst_stride_argb,
               int width,
               int height);

// RGB little endian (bgr in memory) to ARGB.
LIBYUV_API
int RGB24ToARGB(const uint8_t* src_rgb24,
                int src_stride_rgb24,
                uint8_t* dst_argb,
                int dst_stride_argb,
                int width,
                int height);

// RGB big endian (rgb in memory) to ARGB.
LIBYUV_API
int RAWToARGB(const uint8_t* src_raw,
              int src_stride_raw,
              uint8_t* dst_argb,
              int dst_stride_argb,
              int width,
              int height);

// RGB big endian (rgb in memory) to RGBA.
LIBYUV_API
int RAWToRGBA(const uint8_t* src_raw,
              int src_stride_raw,
              uint8_t* dst_rgba,
              int dst_stride_rgba,
              int width,
              int height);

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
                     int height);

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
                          int attenuate);

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
                     int height);

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif

#endif  // INCLUDE_LIBYUV_CONVERT_ARGB_H_
