/*
 *  Copyright 2011 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef INCLUDE_LIBYUV_SCALE_H_
#define INCLUDE_LIBYUV_SCALE_H_

#include "libyuv/basic_types.h"

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

// Supported filtering.
typedef enum FilterMode {
  kFilterNone = 0,      // Point sample; Fastest.
  kFilterLinear = 1,    // Filter horizontally only.
  kFilterBilinear = 2,  // Faster than box, but lower quality scaling down.
  kFilterBox = 3        // Highest quality.
} FilterModeEnum;

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif

#endif  // INCLUDE_LIBYUV_SCALE_H_
