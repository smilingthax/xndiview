/*
 *  Copyright 2012 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef INCLUDE_LIBYUV_CONVERT_FROM_ARGB_H_
#define INCLUDE_LIBYUV_CONVERT_FROM_ARGB_H_

#include "libyuv/basic_types.h"

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

#if 0
  // NOTE(thobi): already in convert_argb.h (and planar_functions.h)
// Copy ARGB to ARGB.
#define ARGBToARGB ARGBCopy
LIBYUV_API
int ARGBCopy(const uint8_t* src_argb,
             int src_stride_argb,
             uint8_t* dst_argb,
             int dst_stride_argb,
             int width,
             int height);

  // NOTE(thobi): alias of BGRAToARGB in convert_argb.h
// Convert ARGB To BGRA.
LIBYUV_API
int ARGBToBGRA(const uint8_t* src_argb,
               int src_stride_argb,
               uint8_t* dst_bgra,
               int dst_stride_bgra,
               int width,
               int height);

  // NOTE(thobi): alias of ABGRToARGB in convert_argb.h
// Convert ARGB To ABGR.
LIBYUV_API
int ARGBToABGR(const uint8_t* src_argb,
               int src_stride_argb,
               uint8_t* dst_abgr,
               int dst_stride_abgr,
               int width,
               int height);
#endif // thobi

// Convert ARGB To RGBA.
LIBYUV_API
int ARGBToRGBA(const uint8_t* src_argb,
               int src_stride_argb,
               uint8_t* dst_rgba,
               int dst_stride_rgba,
               int width,
               int height);

// Aliases
#define ABGRToRGB24 ARGBToRAW
#define ABGRToRAW ARGBToRGB24

// Convert ARGB To RGB24.
LIBYUV_API
int ARGBToRGB24(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_rgb24,
                int dst_stride_rgb24,
                int width,
                int height);

// Convert ARGB To RAW.
LIBYUV_API
int ARGBToRAW(const uint8_t* src_argb,
              int src_stride_argb,
              uint8_t* dst_raw,
              int dst_stride_raw,
              int width,
              int height);

// Convert ARGB To I422.
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
               int height);

// Convert ARGB To UYVY.  -- NOTE(thobi): fixed I601 (via ARGBTo{UV,Y}Row)
LIBYUV_API
int ARGBToUYVY(const uint8_t* src_argb,
               int src_stride_argb,
               uint8_t* dst_uyvy,
               int dst_stride_uyvy,
               int width,
               int height);

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif

#endif  // INCLUDE_LIBYUV_CONVERT_FROM_ARGB_H_
