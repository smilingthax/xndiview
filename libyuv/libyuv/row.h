/*
 *  Copyright 2011 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef INCLUDE_LIBYUV_ROW_H_
#define INCLUDE_LIBYUV_ROW_H_

#include <stdlib.h>  // For malloc.

#include "libyuv/basic_types.h"

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

#if defined(__pnacl__) || defined(__CLR_VER) ||            \
    (defined(__native_client__) && defined(__x86_64__)) || \
    (defined(__i386__) && !defined(__SSE__) && !defined(__clang__))
#define LIBYUV_DISABLE_X86
#endif
#if defined(__native_client__)
#define LIBYUV_DISABLE_NEON
#endif
// MemorySanitizer does not support assembly code yet. http://crbug.com/344505
#if defined(__has_feature)
#if __has_feature(memory_sanitizer)
#define LIBYUV_DISABLE_X86
#endif
#endif
// clang >= 3.5.0 required for Arm64.
#if defined(__clang__) && defined(__aarch64__) && !defined(LIBYUV_DISABLE_NEON)
#if (__clang_major__ < 3) || (__clang_major__ == 3 && (__clang_minor__ < 5))
#define LIBYUV_DISABLE_NEON
#endif  // clang >= 3.5
#endif  // __clang__

// GCC >= 4.7.0 required for AVX2.
#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#if (__GNUC__ > 4) || (__GNUC__ == 4 && (__GNUC_MINOR__ >= 7))
#define GCC_HAS_AVX2 1
#endif  // GNUC >= 4.7
#endif  // __GNUC__

// clang >= 3.4.0 required for AVX2.
#if defined(__clang__) && (defined(__x86_64__) || defined(__i386__))
#if (__clang_major__ > 3) || (__clang_major__ == 3 && (__clang_minor__ >= 4))
#define CLANG_HAS_AVX2 1
#endif  // clang >= 3.4
#endif  // __clang__

// clang >= 6.0.0 required for AVX512.
#if defined(__clang__) && (defined(__x86_64__) || defined(__i386__))
// clang in xcode follows a different versioning scheme.
// TODO(fbarchard): fix xcode 9 ios b/789.
#if (__clang_major__ >= 7) && !defined(__APPLE__)
#define CLANG_HAS_AVX512 1
#endif  // clang >= 7
#endif  // __clang__

// Visual C 2012 required for AVX2.
#if defined(_M_IX86) && !defined(__clang__) && defined(_MSC_VER) && \
    _MSC_VER >= 1700
#define VISUALC_HAS_AVX2 1
#endif  // VisualStudio >= 2012

// The following are available on all x86 platforms:
#if !defined(LIBYUV_DISABLE_X86) && \
    (defined(_M_IX86) || defined(__x86_64__) || defined(__i386__))
// Conversions:
#define HAS_ABGRTOUVROW_SSSE3
#define HAS_ABGRTOYROW_SSSE3
#define HAS_ARGB1555TOARGBROW_SSE2
#define HAS_ARGB4444TOARGBROW_SSE2
#define HAS_ARGBEXTRACTALPHAROW_SSE2
#define HAS_ARGBSETROW_X86
#define HAS_ARGBSHUFFLEROW_SSSE3
#define HAS_ARGBTOARGB1555ROW_SSE2
#define HAS_ARGBTOARGB4444ROW_SSE2
#define HAS_ARGBTORAWROW_SSSE3
#define HAS_ARGBTORGB24ROW_SSSE3
#define HAS_ARGBTORGB565DITHERROW_SSE2
#define HAS_ARGBTORGB565ROW_SSE2
#define HAS_ARGBTOUV444ROW_SSSE3
#define HAS_ARGBTOUVJROW_SSSE3
#define HAS_ARGBTOUVROW_SSSE3
#define HAS_ARGBTOYJROW_SSSE3
#define HAS_ARGBTOYROW_SSSE3
#define HAS_BGRATOUVROW_SSSE3
#define HAS_BGRATOYROW_SSSE3
#define HAS_COPYROW_ERMS
#define HAS_COPYROW_SSE2
#define HAS_H422TOARGBROW_SSSE3
#define HAS_HALFFLOATROW_SSE2
#define HAS_I422TOARGB1555ROW_SSSE3
#define HAS_I422TOARGB4444ROW_SSSE3
#define HAS_I422TOARGBROW_SSSE3
#define HAS_I422TORGB24ROW_SSSE3
#define HAS_I422TORGB565ROW_SSSE3
#define HAS_I422TORGBAROW_SSSE3
#define HAS_I422TOUYVYROW_SSE2
#define HAS_I422TOYUY2ROW_SSE2
#define HAS_I444TOARGBROW_SSSE3
#define HAS_J400TOARGBROW_SSE2
#define HAS_J422TOARGBROW_SSSE3
#define HAS_MERGEUVROW_SSE2
#define HAS_MIRRORROW_SSSE3
#define HAS_MIRRORSPLITUVROW_SSSE3
#define HAS_NV12TOARGBROW_SSSE3
#define HAS_NV12TORGB24ROW_SSSE3
#define HAS_NV12TORGB565ROW_SSSE3
#define HAS_NV21TOARGBROW_SSSE3
#define HAS_NV21TORGB24ROW_SSSE3
#define HAS_RAWTOARGBROW_SSSE3
#define HAS_RAWTORGB24ROW_SSSE3
#define HAS_RAWTOYROW_SSSE3
#define HAS_RGB24TOARGBROW_SSSE3
#define HAS_RGB24TOYROW_SSSE3
#define HAS_RGB24TOYJROW_SSSE3
#define HAS_RAWTOYJROW_SSSE3
#define HAS_RGB565TOARGBROW_SSE2
#define HAS_RGBATOUVROW_SSSE3
#define HAS_RGBATOYROW_SSSE3
#define HAS_SETROW_ERMS
#define HAS_SETROW_X86
#define HAS_SPLITUVROW_SSE2
#define HAS_UYVYTOARGBROW_SSSE3
#define HAS_UYVYTOUV422ROW_SSE2
#define HAS_UYVYTOUVROW_SSE2
#define HAS_UYVYTOYROW_SSE2
#define HAS_YUY2TOARGBROW_SSSE3
#define HAS_YUY2TOUV422ROW_SSE2
#define HAS_YUY2TOUVROW_SSE2
#define HAS_YUY2TOYROW_SSE2

// Effects:
#define HAS_ARGBADDROW_SSE2
#define HAS_ARGBAFFINEROW_SSE2
#define HAS_ARGBATTENUATEROW_SSSE3
#define HAS_ARGBBLENDROW_SSSE3
#define HAS_ARGBCOLORMATRIXROW_SSSE3
#define HAS_ARGBCOLORTABLEROW_X86
#define HAS_ARGBCOPYALPHAROW_SSE2
#define HAS_ARGBCOPYYTOALPHAROW_SSE2
#define HAS_ARGBGRAYROW_SSSE3
#define HAS_ARGBLUMACOLORTABLEROW_SSSE3
#define HAS_ARGBMIRRORROW_SSE2
#define HAS_ARGBMULTIPLYROW_SSE2
#define HAS_ARGBPOLYNOMIALROW_SSE2
#define HAS_ARGBQUANTIZEROW_SSE2
#define HAS_ARGBSEPIAROW_SSSE3
#define HAS_ARGBSHADEROW_SSE2
#define HAS_ARGBSUBTRACTROW_SSE2
#define HAS_ARGBUNATTENUATEROW_SSE2
#define HAS_BLENDPLANEROW_SSSE3
#define HAS_COMPUTECUMULATIVESUMROW_SSE2
#define HAS_CUMULATIVESUMTOAVERAGEROW_SSE2
#define HAS_INTERPOLATEROW_SSSE3
#define HAS_RGBCOLORTABLEROW_X86
#define HAS_SOBELROW_SSE2
#define HAS_SOBELTOPLANEROW_SSE2
#define HAS_SOBELXROW_SSE2
#define HAS_SOBELXYROW_SSE2
#define HAS_SOBELYROW_SSE2

// The following functions fail on gcc/clang 32 bit with fpic and framepointer.
// caveat: clangcl uses row_win.cc which works.
#if defined(__x86_64__) || !defined(__pic__) || defined(__clang__) || \
    defined(_MSC_VER)
// TODO(fbarchard): fix build error on android_full_debug=1
// https://code.google.com/p/libyuv/issues/detail?id=517
#define HAS_I422ALPHATOARGBROW_SSSE3
#define HAS_I444ALPHATOARGBROW_SSSE3
#endif
#endif

// The following are available on all x86 platforms, but
// require VS2012, clang 3.4 or gcc 4.7.
#if !defined(LIBYUV_DISABLE_X86) &&                          \
    (defined(VISUALC_HAS_AVX2) || defined(CLANG_HAS_AVX2) || \
     defined(GCC_HAS_AVX2))
#define HAS_ARGBCOPYALPHAROW_AVX2
#define HAS_ARGBCOPYYTOALPHAROW_AVX2
#define HAS_ARGBEXTRACTALPHAROW_AVX2
#define HAS_ARGBMIRRORROW_AVX2
#define HAS_ARGBPOLYNOMIALROW_AVX2
#define HAS_ARGBSHUFFLEROW_AVX2
#define HAS_ARGBTORGB565DITHERROW_AVX2
#define HAS_ARGBTOUVJROW_AVX2
#define HAS_ARGBTOUVROW_AVX2
#define HAS_ARGBTOYJROW_AVX2
#define HAS_ARGBTOYROW_AVX2
#define HAS_RGB24TOYJROW_AVX2
#define HAS_RAWTOYJROW_AVX2
#define HAS_COPYROW_AVX
#define HAS_H422TOARGBROW_AVX2
#define HAS_HALFFLOATROW_AVX2
//  #define HAS_HALFFLOATROW_F16C  // Enable to test halffloat cast
#define HAS_I422TOARGB1555ROW_AVX2
#define HAS_I422TOARGB4444ROW_AVX2
#define HAS_I422TOARGBROW_AVX2
#define HAS_I422TORGB24ROW_AVX2
#define HAS_I422TORGB565ROW_AVX2
#define HAS_I422TORGBAROW_AVX2
#define HAS_I444TOARGBROW_AVX2
#define HAS_INTERPOLATEROW_AVX2
#define HAS_J422TOARGBROW_AVX2
#define HAS_MERGEUVROW_AVX2
#define HAS_MIRRORROW_AVX2
#define HAS_NV12TOARGBROW_AVX2
#define HAS_NV12TORGB24ROW_AVX2
#define HAS_NV12TORGB565ROW_AVX2
#define HAS_NV21TOARGBROW_AVX2
#define HAS_NV21TORGB24ROW_AVX2
#define HAS_SPLITUVROW_AVX2
#define HAS_UYVYTOARGBROW_AVX2
#define HAS_UYVYTOUV422ROW_AVX2
#define HAS_UYVYTOUVROW_AVX2
#define HAS_UYVYTOYROW_AVX2
#define HAS_YUY2TOARGBROW_AVX2
#define HAS_YUY2TOUV422ROW_AVX2
#define HAS_YUY2TOUVROW_AVX2
#define HAS_YUY2TOYROW_AVX2

// Effects:
#define HAS_ARGBADDROW_AVX2
#define HAS_ARGBATTENUATEROW_AVX2
#define HAS_ARGBMULTIPLYROW_AVX2
#define HAS_ARGBSUBTRACTROW_AVX2
#define HAS_ARGBUNATTENUATEROW_AVX2
#define HAS_BLENDPLANEROW_AVX2

#if defined(__x86_64__) || !defined(__pic__) || defined(__clang__) || \
    defined(_MSC_VER)
// TODO(fbarchard): fix build error on android_full_debug=1
// https://code.google.com/p/libyuv/issues/detail?id=517
#define HAS_I422ALPHATOARGBROW_AVX2
#define HAS_I444ALPHATOARGBROW_AVX2
#endif
#endif

// The following are available for AVX2 Visual C 32 bit:
// TODO(fbarchard): Port to gcc.
#if !defined(LIBYUV_DISABLE_X86) && defined(_M_IX86) && defined(_MSC_VER) && \
    !defined(__clang__) && defined(VISUALC_HAS_AVX2)
#define HAS_ARGB1555TOARGBROW_AVX2
#define HAS_ARGB4444TOARGBROW_AVX2
#define HAS_ARGBTOARGB1555ROW_AVX2
#define HAS_ARGBTOARGB4444ROW_AVX2
#define HAS_ARGBTORGB565ROW_AVX2
#define HAS_J400TOARGBROW_AVX2
#define HAS_RGB565TOARGBROW_AVX2
#endif

// The following are also available on x64 Visual C.
#if !defined(LIBYUV_DISABLE_X86) && defined(_MSC_VER) && defined(_M_X64) && \
    (!defined(__clang__) || defined(__SSSE3__))
#define HAS_I444ALPHATOARGBROW_SSSE3
#define HAS_I444TOARGBROW_SSSE3
#define HAS_I422ALPHATOARGBROW_SSSE3
#define HAS_I422TOARGBROW_SSSE3
#endif

// The following are available for gcc/clang x86 platforms:
// TODO(fbarchard): Port to Visual C
#if !defined(LIBYUV_DISABLE_X86) && (defined(__x86_64__) || defined(__i386__))
#define HAS_ABGRTOAR30ROW_SSSE3
#define HAS_ARGBTOAR30ROW_SSSE3
#define HAS_ARGBTOAR64ROW_SSSE3
#define HAS_ARGBTOAB64ROW_SSSE3
#define HAS_AR64TOARGBROW_SSSE3
#define HAS_AB64TOARGBROW_SSSE3
#define HAS_CONVERT16TO8ROW_SSSE3
#define HAS_CONVERT8TO16ROW_SSE2
#define HAS_HALFMERGEUVROW_SSSE3
#define HAS_I210TOAR30ROW_SSSE3
#define HAS_I210TOARGBROW_SSSE3
#define HAS_I212TOAR30ROW_SSSE3
#define HAS_I212TOARGBROW_SSSE3
#define HAS_I400TOARGBROW_SSE2
#define HAS_I422TOAR30ROW_SSSE3
#define HAS_I410TOAR30ROW_SSSE3
#define HAS_I410TOARGBROW_SSSE3
#define HAS_MERGEARGBROW_SSE2
#define HAS_MERGEXRGBROW_SSE2
#define HAS_MERGERGBROW_SSSE3
#define HAS_MIRRORUVROW_SSSE3
#define HAS_P210TOAR30ROW_SSSE3
#define HAS_P210TOARGBROW_SSSE3
#define HAS_P410TOAR30ROW_SSSE3
#define HAS_P410TOARGBROW_SSSE3
#define HAS_RAWTORGBAROW_SSSE3
#define HAS_RGB24MIRRORROW_SSSE3
#define HAS_RGBATOYJROW_SSSE3
#define HAS_SPLITARGBROW_SSE2
#define HAS_SPLITARGBROW_SSSE3
#define HAS_SPLITXRGBROW_SSE2
#define HAS_SPLITXRGBROW_SSSE3
#define HAS_SPLITRGBROW_SSSE3
#define HAS_SWAPUVROW_SSSE3

#if defined(__x86_64__) || !defined(__pic__)
// TODO(fbarchard): fix build error on android_full_debug=1
// https://code.google.com/p/libyuv/issues/detail?id=517
#define HAS_I210ALPHATOARGBROW_SSSE3
#define HAS_I410ALPHATOARGBROW_SSSE3
#endif
#endif

// The following are available for AVX2 gcc/clang x86 platforms:
// TODO(fbarchard): Port to Visual C
#if !defined(LIBYUV_DISABLE_X86) &&               \
    (defined(__x86_64__) || defined(__i386__)) && \
    (defined(CLANG_HAS_AVX2) || defined(GCC_HAS_AVX2))
#define HAS_ABGRTOAR30ROW_AVX2
#define HAS_ABGRTOUVROW_AVX2
#define HAS_ABGRTOYROW_AVX2
#define HAS_ARGBTOAR30ROW_AVX2
#define HAS_ARGBTORAWROW_AVX2
#define HAS_ARGBTORGB24ROW_AVX2
#define HAS_ARGBTOAR64ROW_AVX2
#define HAS_ARGBTOAB64ROW_AVX2
#define HAS_AR64TOARGBROW_AVX2
#define HAS_AB64TOARGBROW_AVX2
#define HAS_CONVERT16TO8ROW_AVX2
#define HAS_CONVERT8TO16ROW_AVX2
#define HAS_DIVIDEROW_16_AVX2
#define HAS_HALFMERGEUVROW_AVX2
#define HAS_MERGEAR64ROW_AVX2
#define HAS_MERGEARGB16TO8ROW_AVX2
#define HAS_MERGEARGBROW_AVX2
#define HAS_MERGEXR30ROW_AVX2
#define HAS_MERGEXR64ROW_AVX2
#define HAS_MERGEXRGB16TO8ROW_AVX2
#define HAS_MERGEXRGBROW_AVX2
#define HAS_I210TOAR30ROW_AVX2
#define HAS_I210TOARGBROW_AVX2
#define HAS_I212TOAR30ROW_AVX2
#define HAS_I212TOARGBROW_AVX2
#define HAS_I400TOARGBROW_AVX2
#define HAS_I410TOAR30ROW_AVX2
#define HAS_I410TOARGBROW_AVX2
#define HAS_P210TOAR30ROW_AVX2
#define HAS_P210TOARGBROW_AVX2
#define HAS_P410TOAR30ROW_AVX2
#define HAS_P410TOARGBROW_AVX2
#define HAS_I422TOAR30ROW_AVX2
#define HAS_I422TOUYVYROW_AVX2
#define HAS_I422TOYUY2ROW_AVX2
#define HAS_MERGEUVROW_16_AVX2
#define HAS_MIRRORUVROW_AVX2
#define HAS_MULTIPLYROW_16_AVX2
#define HAS_RGBATOYJROW_AVX2
#define HAS_SPLITARGBROW_AVX2
#define HAS_SPLITXRGBROW_AVX2
#define HAS_SPLITUVROW_16_AVX2
#define HAS_SWAPUVROW_AVX2
// TODO(fbarchard): Fix AVX2 version of YUV24
// #define HAS_NV21TOYUV24ROW_AVX2

#if defined(__x86_64__) || !defined(__pic__)
// TODO(fbarchard): fix build error on android_full_debug=1
// https://code.google.com/p/libyuv/issues/detail?id=517
#define HAS_I210ALPHATOARGBROW_AVX2
#define HAS_I410ALPHATOARGBROW_AVX2
#endif
#endif

// The following are available for AVX512 clang x86 platforms:
// TODO(fbarchard): Port to GCC and Visual C
// TODO(fbarchard): re-enable HAS_ARGBTORGB24ROW_AVX512VBMI. Issue libyuv:789
#if !defined(LIBYUV_DISABLE_X86) && \
    (defined(__x86_64__) || defined(__i386__)) && (defined(CLANG_HAS_AVX512))
#define HAS_ARGBTORGB24ROW_AVX512VBMI
#endif

// The following are available on Neon platforms:
#if !defined(LIBYUV_DISABLE_NEON) && \
    (defined(__aarch64__) || defined(__ARM_NEON__) || defined(LIBYUV_NEON))
#define HAS_ABGRTOUVROW_NEON
#define HAS_ABGRTOYROW_NEON
#define HAS_ARGB1555TOARGBROW_NEON
#define HAS_ARGB1555TOUVROW_NEON
#define HAS_ARGB1555TOYROW_NEON
#define HAS_ARGB4444TOARGBROW_NEON
#define HAS_ARGB4444TOUVROW_NEON
#define HAS_ARGB4444TOYROW_NEON
#define HAS_ARGBEXTRACTALPHAROW_NEON
#define HAS_ARGBSETROW_NEON
#define HAS_ARGBTOARGB1555ROW_NEON
#define HAS_ARGBTOARGB4444ROW_NEON
#define HAS_ARGBTORAWROW_NEON
#define HAS_ARGBTORGB24ROW_NEON
#define HAS_ARGBTORGB565DITHERROW_NEON
#define HAS_ARGBTORGB565ROW_NEON
#define HAS_ARGBTOAR64ROW_NEON
#define HAS_ARGBTOAB64ROW_NEON
#define HAS_AR64TOARGBROW_NEON
#define HAS_AB64TOARGBROW_NEON
#define HAS_ARGBTOUV444ROW_NEON
#define HAS_ARGBTOUVJROW_NEON
#define HAS_ARGBTOUVROW_NEON
#define HAS_ARGBTOYJROW_NEON
#define HAS_ARGBTOYROW_NEON
#define HAS_AYUVTOUVROW_NEON
#define HAS_AYUVTOVUROW_NEON
#define HAS_AYUVTOYROW_NEON
#define HAS_BGRATOUVROW_NEON
#define HAS_BGRATOYROW_NEON
#define HAS_BYTETOFLOATROW_NEON
#define HAS_COPYROW_NEON
#define HAS_DIVIDEROW_16_NEON
#define HAS_HALFFLOATROW_NEON
#define HAS_HALFMERGEUVROW_NEON
#define HAS_I400TOARGBROW_NEON
#define HAS_I444ALPHATOARGBROW_NEON
#define HAS_I422ALPHATOARGBROW_NEON
#define HAS_I422TOARGB1555ROW_NEON
#define HAS_I422TOARGB4444ROW_NEON
#define HAS_I422TOARGBROW_NEON
#define HAS_I422TORGB24ROW_NEON
#define HAS_I422TORGB565ROW_NEON
#define HAS_I422TORGBAROW_NEON
#define HAS_I422TOUYVYROW_NEON
#define HAS_I422TOYUY2ROW_NEON
#define HAS_I444TOARGBROW_NEON
#define HAS_J400TOARGBROW_NEON
#define HAS_MERGEAR64ROW_NEON
#define HAS_MERGEARGB16TO8ROW_NEON
#define HAS_MERGEARGBROW_NEON
#define HAS_MERGEXR30ROW_NEON
#define HAS_MERGEXR64ROW_NEON
#define HAS_MERGEXRGB16TO8ROW_NEON
#define HAS_MERGEXRGBROW_NEON
#define HAS_MERGEUVROW_NEON
#define HAS_MERGEUVROW_16_NEON
#define HAS_MIRRORROW_NEON
#define HAS_MIRRORUVROW_NEON
#define HAS_MIRRORSPLITUVROW_NEON
#define HAS_MULTIPLYROW_16_NEON
#define HAS_NV12TOARGBROW_NEON
#define HAS_NV12TORGB24ROW_NEON
#define HAS_NV12TORGB565ROW_NEON
#define HAS_NV21TOARGBROW_NEON
#define HAS_NV21TORGB24ROW_NEON
#define HAS_NV21TOYUV24ROW_NEON
#define HAS_RAWTOARGBROW_NEON
#define HAS_RAWTORGB24ROW_NEON
#define HAS_RAWTORGBAROW_NEON
#define HAS_RAWTOUVROW_NEON
#define HAS_RAWTOYJROW_NEON
#define HAS_RAWTOYROW_NEON
#define HAS_RGB24TOARGBROW_NEON
#define HAS_RGB24TOUVROW_NEON
#define HAS_RGB24TOYJROW_NEON
#define HAS_RGB24TOYROW_NEON
#define HAS_RGB565TOARGBROW_NEON
#define HAS_RGB565TOUVROW_NEON
#define HAS_RGB565TOYROW_NEON
#define HAS_RGBATOUVROW_NEON
#define HAS_RGBATOYJROW_NEON
#define HAS_RGBATOYROW_NEON
#define HAS_SETROW_NEON
#define HAS_SPLITARGBROW_NEON
#define HAS_SPLITXRGBROW_NEON
#define HAS_SPLITRGBROW_NEON
#define HAS_SPLITUVROW_NEON
#define HAS_SPLITUVROW_16_NEON
#define HAS_SWAPUVROW_NEON
#define HAS_UYVYTOARGBROW_NEON
#define HAS_UYVYTOUV422ROW_NEON
#define HAS_UYVYTOUVROW_NEON
#define HAS_UYVYTOYROW_NEON
#define HAS_YUY2TOARGBROW_NEON
#define HAS_YUY2TOUV422ROW_NEON
#define HAS_YUY2TOUVROW_NEON
#define HAS_YUY2TOYROW_NEON

// Effects:
#define HAS_ARGBADDROW_NEON
#define HAS_ARGBATTENUATEROW_NEON
#define HAS_ARGBBLENDROW_NEON
#define HAS_ARGBCOLORMATRIXROW_NEON
#define HAS_ARGBGRAYROW_NEON
#define HAS_ARGBMIRRORROW_NEON
#define HAS_RGB24MIRRORROW_NEON
#define HAS_ARGBMULTIPLYROW_NEON
#define HAS_ARGBQUANTIZEROW_NEON
#define HAS_ARGBSEPIAROW_NEON
#define HAS_ARGBSHADEROW_NEON
#define HAS_ARGBSHUFFLEROW_NEON
#define HAS_ARGBSUBTRACTROW_NEON
#define HAS_INTERPOLATEROW_NEON
#define HAS_SOBELROW_NEON
#define HAS_SOBELTOPLANEROW_NEON
#define HAS_SOBELXROW_NEON
#define HAS_SOBELXYROW_NEON
#define HAS_SOBELYROW_NEON
#endif

// The following are available on AArch64 platforms:
#if !defined(LIBYUV_DISABLE_NEON) && defined(__aarch64__)
#define HAS_SCALESUMSAMPLES_NEON
#define HAS_GAUSSROW_F32_NEON
#define HAS_GAUSSCOL_F32_NEON

#endif
#if !defined(LIBYUV_DISABLE_MSA) && defined(__mips_msa)
#define HAS_ABGRTOUVROW_MSA
#define HAS_ABGRTOYROW_MSA
#define HAS_ARGB1555TOARGBROW_MSA
#define HAS_ARGB1555TOUVROW_MSA
#define HAS_ARGB1555TOYROW_MSA
#define HAS_ARGB4444TOARGBROW_MSA
#define HAS_ARGBADDROW_MSA
#define HAS_ARGBATTENUATEROW_MSA
#define HAS_ARGBBLENDROW_MSA
#define HAS_ARGBCOLORMATRIXROW_MSA
#define HAS_ARGBEXTRACTALPHAROW_MSA
#define HAS_ARGBGRAYROW_MSA
#define HAS_ARGBMIRRORROW_MSA
#define HAS_ARGBMULTIPLYROW_MSA
#define HAS_ARGBQUANTIZEROW_MSA
#define HAS_ARGBSEPIAROW_MSA
#define HAS_ARGBSETROW_MSA
#define HAS_ARGBSHADEROW_MSA
#define HAS_ARGBSHUFFLEROW_MSA
#define HAS_ARGBSUBTRACTROW_MSA
#define HAS_ARGBTOARGB1555ROW_MSA
#define HAS_ARGBTOARGB4444ROW_MSA
#define HAS_ARGBTORAWROW_MSA
#define HAS_ARGBTORGB24ROW_MSA
#define HAS_ARGBTORGB565DITHERROW_MSA
#define HAS_ARGBTORGB565ROW_MSA
#define HAS_ARGBTOUV444ROW_MSA
#define HAS_ARGBTOUVJROW_MSA
#define HAS_ARGBTOUVROW_MSA
#define HAS_ARGBTOYJROW_MSA
#define HAS_ARGBTOYROW_MSA
#define HAS_BGRATOUVROW_MSA
#define HAS_BGRATOYROW_MSA
#define HAS_HALFFLOATROW_MSA
#define HAS_I400TOARGBROW_MSA
#define HAS_I422TOUYVYROW_MSA
#define HAS_I422TOYUY2ROW_MSA
#define HAS_INTERPOLATEROW_MSA
#define HAS_J400TOARGBROW_MSA
#define HAS_MERGEUVROW_MSA
#define HAS_MIRRORROW_MSA
#define HAS_MIRRORUVROW_MSA
#define HAS_MIRRORSPLITUVROW_MSA
#define HAS_RAWTOARGBROW_MSA
#define HAS_RAWTORGB24ROW_MSA
#define HAS_RAWTOUVROW_MSA
#define HAS_RAWTOYROW_MSA
#define HAS_RGB24TOARGBROW_MSA
#define HAS_RGB24TOUVROW_MSA
#define HAS_RGB24TOYROW_MSA
#define HAS_RGB565TOARGBROW_MSA
#define HAS_RGB565TOUVROW_MSA
#define HAS_RGB565TOYROW_MSA
#define HAS_RGBATOUVROW_MSA
#define HAS_RGBATOYROW_MSA
#define HAS_SETROW_MSA
#define HAS_SOBELROW_MSA
#define HAS_SOBELTOPLANEROW_MSA
#define HAS_SOBELXROW_MSA
#define HAS_SOBELXYROW_MSA
#define HAS_SOBELYROW_MSA
#define HAS_SPLITUVROW_MSA
#define HAS_UYVYTOUVROW_MSA
#define HAS_UYVYTOYROW_MSA
#define HAS_YUY2TOUV422ROW_MSA
#define HAS_YUY2TOUVROW_MSA
#define HAS_YUY2TOYROW_MSA
#endif

#if !defined(LIBYUV_DISABLE_MMI) && defined(_MIPS_ARCH_LOONGSON3A)
#define HAS_ABGRTOUVROW_MMI
#define HAS_ABGRTOYROW_MMI
#define HAS_ARGB1555TOARGBROW_MMI
#define HAS_ARGB1555TOUVROW_MMI
#define HAS_ARGB1555TOYROW_MMI
#define HAS_ARGB4444TOARGBROW_MMI
#define HAS_ARGB4444TOUVROW_MMI
#define HAS_ARGB4444TOYROW_MMI
#define HAS_ARGBADDROW_MMI
#define HAS_ARGBATTENUATEROW_MMI
#define HAS_ARGBBLENDROW_MMI
#define HAS_ARGBCOLORMATRIXROW_MMI
#define HAS_ARGBCOPYALPHAROW_MMI
#define HAS_ARGBCOPYYTOALPHAROW_MMI
#define HAS_ARGBEXTRACTALPHAROW_MMI
#define HAS_ARGBGRAYROW_MMI
#define HAS_ARGBMIRRORROW_MMI
#define HAS_ARGBMULTIPLYROW_MMI
#define HAS_ARGBSEPIAROW_MMI
#define HAS_ARGBSETROW_MMI
#define HAS_ARGBSHADEROW_MMI
#define HAS_ARGBSHUFFLEROW_MMI
#define HAS_ARGBSUBTRACTROW_MMI
#define HAS_ARGBTOARGB1555ROW_MMI
#define HAS_ARGBTOARGB4444ROW_MMI
#define HAS_ARGBTORAWROW_MMI
#define HAS_ARGBTORGB24ROW_MMI
#define HAS_ARGBTORGB565DITHERROW_MMI
#define HAS_ARGBTORGB565ROW_MMI
#define HAS_ARGBTOUV444ROW_MMI
#define HAS_ARGBTOUVJROW_MMI
#define HAS_ARGBTOUVROW_MMI
#define HAS_ARGBTOYJROW_MMI
#define HAS_ARGBTOYROW_MMI
#define HAS_BGRATOUVROW_MMI
#define HAS_BGRATOYROW_MMI
#define HAS_BLENDPLANEROW_MMI
#define HAS_COMPUTECUMULATIVESUMROW_MMI
#define HAS_CUMULATIVESUMTOAVERAGEROW_MMI
#define HAS_HALFFLOATROW_MMI
#define HAS_I400TOARGBROW_MMI
#define HAS_I422TOUYVYROW_MMI
#define HAS_I422TOYUY2ROW_MMI
#define HAS_INTERPOLATEROW_MMI
#define HAS_J400TOARGBROW_MMI
#define HAS_MERGERGBROW_MMI
#define HAS_MERGEUVROW_MMI
#define HAS_MIRRORROW_MMI
#define HAS_MIRRORSPLITUVROW_MMI
#define HAS_RAWTOARGBROW_MMI
#define HAS_RAWTORGB24ROW_MMI
#define HAS_RAWTOUVROW_MMI
#define HAS_RAWTOYROW_MMI
#define HAS_RGB24TOARGBROW_MMI
#define HAS_RGB24TOUVROW_MMI
#define HAS_RGB24TOYROW_MMI
#define HAS_RGB565TOARGBROW_MMI
#define HAS_RGB565TOUVROW_MMI
#define HAS_RGB565TOYROW_MMI
#define HAS_RGBATOUVROW_MMI
#define HAS_RGBATOYROW_MMI
#define HAS_SOBELROW_MMI
#define HAS_SOBELTOPLANEROW_MMI
#define HAS_SOBELXROW_MMI
#define HAS_SOBELXYROW_MMI
#define HAS_SOBELYROW_MMI
#define HAS_SPLITRGBROW_MMI
#define HAS_SPLITUVROW_MMI
#define HAS_UYVYTOUVROW_MMI
#define HAS_UYVYTOYROW_MMI
#define HAS_YUY2TOUV422ROW_MMI
#define HAS_YUY2TOUVROW_MMI
#define HAS_YUY2TOYROW_MMI
#endif

#if defined(_MSC_VER) && !defined(__CLR_VER) && !defined(__clang__)
#if defined(VISUALC_HAS_AVX2)
#define SIMD_ALIGNED(var) __declspec(align(32)) var
#else
#define SIMD_ALIGNED(var) __declspec(align(16)) var
#endif
#define LIBYUV_NOINLINE __declspec(noinline)
typedef __declspec(align(16)) int16_t vec16[8];
typedef __declspec(align(16)) int32_t vec32[4];
typedef __declspec(align(16)) float vecf32[4];
typedef __declspec(align(16)) int8_t vec8[16];
typedef __declspec(align(16)) uint16_t uvec16[8];
typedef __declspec(align(16)) uint32_t uvec32[4];
typedef __declspec(align(16)) uint8_t uvec8[16];
typedef __declspec(align(32)) int16_t lvec16[16];
typedef __declspec(align(32)) int32_t lvec32[8];
typedef __declspec(align(32)) int8_t lvec8[32];
typedef __declspec(align(32)) uint16_t ulvec16[16];
typedef __declspec(align(32)) uint32_t ulvec32[8];
typedef __declspec(align(32)) uint8_t ulvec8[32];
#elif !defined(__pnacl__) && (defined(__GNUC__) || defined(__clang__))
// Caveat GCC 4.2 to 4.7 have a known issue using vectors with const.
#if defined(CLANG_HAS_AVX2) || defined(GCC_HAS_AVX2)
#define SIMD_ALIGNED(var) var __attribute__((aligned(32)))
#else
#define SIMD_ALIGNED(var) var __attribute__((aligned(16)))
#endif
#define LIBYUV_NOINLINE __attribute__((noinline))
typedef int16_t __attribute__((vector_size(16))) vec16;
typedef int32_t __attribute__((vector_size(16))) vec32;
typedef float __attribute__((vector_size(16))) vecf32;
typedef int8_t __attribute__((vector_size(16))) vec8;
typedef uint16_t __attribute__((vector_size(16))) uvec16;
typedef uint32_t __attribute__((vector_size(16))) uvec32;
typedef uint8_t __attribute__((vector_size(16))) uvec8;
typedef int16_t __attribute__((vector_size(32))) lvec16;
typedef int32_t __attribute__((vector_size(32))) lvec32;
typedef int8_t __attribute__((vector_size(32))) lvec8;
typedef uint16_t __attribute__((vector_size(32))) ulvec16;
typedef uint32_t __attribute__((vector_size(32))) ulvec32;
typedef uint8_t __attribute__((vector_size(32))) ulvec8;
#else
#define SIMD_ALIGNED(var) var
#define LIBYUV_NOINLINE
typedef int16_t vec16[8];
typedef int32_t vec32[4];
typedef float vecf32[4];
typedef int8_t vec8[16];
typedef uint16_t uvec16[8];
typedef uint32_t uvec32[4];
typedef uint8_t uvec8[16];
typedef int16_t lvec16[16];
typedef int32_t lvec32[8];
typedef int8_t lvec8[32];
typedef uint16_t ulvec16[16];
typedef uint32_t ulvec32[8];
typedef uint8_t ulvec8[32];
#endif

#if defined(__aarch64__) || defined(__arm__)
// This struct is for ARM color conversion.
struct YuvConstants {
  uvec8 kUVCoeff;
  vec16 kRGBCoeffBias;
};
#else
// This struct is for Intel color conversion.
struct YuvConstants {
  uint8_t kUVToB[32];
  uint8_t kUVToG[32];
  uint8_t kUVToR[32];
  int16_t kYToRgb[16];
  int16_t kYBiasToRgb[16];
};

// Offsets into YuvConstants structure
#define KUVTOB 0
#define KUVTOG 32
#define KUVTOR 64
#define KYTORGB 96
#define KYBIASTORGB 128

#endif

#define IS_ALIGNED(p, a) (!((uintptr_t)(p) & ((a)-1)))

#define align_buffer_64(var, size)                                           \
  uint8_t* var##_mem = (uint8_t*)(malloc((size) + 63));         /* NOLINT */ \
  uint8_t* var = (uint8_t*)(((intptr_t)(var##_mem) + 63) & ~63) /* NOLINT */

#define free_aligned_buffer_64(var) \
  free(var##_mem);                  \
  var = 0

#if defined(__APPLE__) || defined(__x86_64__) || defined(__llvm__)
#define OMITFP
#else
#define OMITFP __attribute__((optimize("omit-frame-pointer")))
#endif

// NaCL macros for GCC x86 and x64.
#if defined(__native_client__)
#define LABELALIGN ".p2align 5\n"
#else
#define LABELALIGN
#endif

// Intel Code Analizer markers.  Insert IACA_START IACA_END around code to be
// measured and then run with iaca -64 libyuv_unittest.
// IACA_ASM_START amd IACA_ASM_END are equivalents that can be used within
// inline assembly blocks.
// example of iaca:
// ~/iaca-lin64/bin/iaca.sh -64 -analysis LATENCY out/Release/libyuv_unittest

#if defined(__x86_64__) || defined(__i386__)

#define IACA_ASM_START  \
  ".byte 0x0F, 0x0B\n"  \
  " movl $111, %%ebx\n" \
  ".byte 0x64, 0x67, 0x90\n"

#define IACA_ASM_END         \
  " movl $222, %%ebx\n"      \
  ".byte 0x64, 0x67, 0x90\n" \
  ".byte 0x0F, 0x0B\n"

#define IACA_SSC_MARK(MARK_ID)                        \
  __asm__ __volatile__("\n\t  movl $" #MARK_ID        \
                       ", %%ebx"                      \
                       "\n\t  .byte 0x64, 0x67, 0x90" \
                       :                              \
                       :                              \
                       : "memory");

#define IACA_UD_BYTES __asm__ __volatile__("\n\t .byte 0x0F, 0x0B");

#else /* Visual C */
#define IACA_UD_BYTES \
  { __asm _emit 0x0F __asm _emit 0x0B }

#define IACA_SSC_MARK(x) \
  { __asm mov ebx, x __asm _emit 0x64 __asm _emit 0x67 __asm _emit 0x90 }

#define IACA_VC64_START __writegsbyte(111, 111);
#define IACA_VC64_END __writegsbyte(222, 222);
#endif

#define IACA_START     \
  {                    \
    IACA_UD_BYTES      \
    IACA_SSC_MARK(111) \
  }
#define IACA_END       \
  {                    \
    IACA_SSC_MARK(222) \
    IACA_UD_BYTES      \
  }

void I422ToARGBRow_NEON(const uint8_t* src_y,
                        const uint8_t* src_u,
                        const uint8_t* src_v,
                        uint8_t* dst_argb,
                        const struct YuvConstants* yuvconstants,
                        int width);
void I422AlphaToARGBRow_NEON(const uint8_t* src_y,
                             const uint8_t* src_u,
                             const uint8_t* src_v,
                             const uint8_t* src_a,
                             uint8_t* dst_argb,
                             const struct YuvConstants* yuvconstants,
                             int width);
void I422ToRGBARow_NEON(const uint8_t* src_y,
                        const uint8_t* src_u,
                        const uint8_t* src_v,
                        uint8_t* dst_rgba,
                        const struct YuvConstants* yuvconstants,
                        int width);
void UYVYToARGBRow_NEON(const uint8_t* src_uyvy,
                        uint8_t* dst_argb,
                        const struct YuvConstants* yuvconstants,
                        int width);

void I422ToARGBRow_MSA(const uint8_t* src_y,
                       const uint8_t* src_u,
                       const uint8_t* src_v,
                       uint8_t* dst_argb,
                       const struct YuvConstants* yuvconstants,
                       int width);
void I422ToRGBARow_MSA(const uint8_t* src_y,
                       const uint8_t* src_u,
                       const uint8_t* src_v,
                       uint8_t* dst_argb,
                       const struct YuvConstants* yuvconstants,
                       int width);
void I422ToARGBRow_MMI(const uint8_t* src_y,
                       const uint8_t* src_u,
                       const uint8_t* src_v,
                       uint8_t* dst_argb,
                       const struct YuvConstants* yuvconstants,
                       int width);
void I422AlphaToARGBRow_MSA(const uint8_t* src_y,
                            const uint8_t* src_u,
                            const uint8_t* src_v,
                            const uint8_t* src_a,
                            uint8_t* dst_argb,
                            const struct YuvConstants* yuvconstants,
                            int width);
void UYVYToARGBRow_MSA(const uint8_t* src_uyvy,
                       uint8_t* dst_argb,
                       const struct YuvConstants* yuvconstants,
                       int width);

void ARGBToYRow_AVX2(const uint8_t* src_argb, uint8_t* dst_y, int width);
void ARGBToYRow_Any_AVX2(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void ABGRToYRow_AVX2(const uint8_t* src_abgr, uint8_t* dst_y, int width);
void ABGRToYRow_Any_AVX2(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void ARGBToYRow_SSSE3(const uint8_t* src_argb, uint8_t* dst_y, int width);
void BGRAToYRow_SSSE3(const uint8_t* src_bgra, uint8_t* dst_y, int width);
void ABGRToYRow_SSSE3(const uint8_t* src_abgr, uint8_t* dst_y, int width);
void RGBAToYRow_SSSE3(const uint8_t* src_rgba, uint8_t* dst_y, int width);
void RGB24ToYRow_SSSE3(const uint8_t* src_rgb24, uint8_t* dst_y, int width);
void ARGBToYRow_NEON(const uint8_t* src_argb, uint8_t* dst_y, int width);
void ARGBToYRow_MSA(const uint8_t* src_argb0, uint8_t* dst_y, int width);
void ARGBToYRow_MMI(const uint8_t* src_argb0, uint8_t* dst_y, int width);
void ARGBToUVRow_NEON(const uint8_t* src_argb,
                      int src_stride_argb,
                      uint8_t* dst_u,
                      uint8_t* dst_v,
                      int width);
void ARGBToUVRow_MSA(const uint8_t* src_argb,
                     int src_stride_argb,
                     uint8_t* dst_u,
                     uint8_t* dst_v,
                     int width);
void ARGBToUVRow_MMI(const uint8_t* src_argb,
                     int src_stride_argb,
                     uint8_t* dst_u,
                     uint8_t* dst_v,
                     int width);
void BGRAToUVRow_NEON(const uint8_t* src_bgra,
                      int src_stride_bgra,
                      uint8_t* dst_u,
                      uint8_t* dst_v,
                      int width);
void ABGRToUVRow_NEON(const uint8_t* src_abgr,
                      int src_stride_abgr,
                      uint8_t* dst_u,
                      uint8_t* dst_v,
                      int width);
void RGBAToUVRow_NEON(const uint8_t* src_rgba,
                      int src_stride_rgba,
                      uint8_t* dst_u,
                      uint8_t* dst_v,
                      int width);
void BGRAToUVRow_MSA(const uint8_t* src_rgb,
                     int src_stride_rgb,
                     uint8_t* dst_u,
                     uint8_t* dst_v,
                     int width);
void ABGRToUVRow_MSA(const uint8_t* src_rgb,
                     int src_stride_rgb,
                     uint8_t* dst_u,
                     uint8_t* dst_v,
                     int width);
void RGBAToUVRow_MSA(const uint8_t* src_rgb,
                     int src_stride_rgb,
                     uint8_t* dst_u,
                     uint8_t* dst_v,
                     int width);
void BGRAToUVRow_MMI(const uint8_t* src_rgb,
                     int src_stride_rgb,
                     uint8_t* dst_u,
                     uint8_t* dst_v,
                     int width);
void ABGRToUVRow_MMI(const uint8_t* src_rgb,
                     int src_stride_rgb,
                     uint8_t* dst_u,
                     uint8_t* dst_v,
                     int width);
void RGBAToUVRow_MMI(const uint8_t* src_rgb,
                     int src_stride_rgb,
                     uint8_t* dst_u,
                     uint8_t* dst_v,
                     int width);
void BGRAToYRow_NEON(const uint8_t* src_bgra, uint8_t* dst_y, int width);
void ABGRToYRow_NEON(const uint8_t* src_abgr, uint8_t* dst_y, int width);
void RGBAToYRow_NEON(const uint8_t* src_rgba, uint8_t* dst_y, int width);
void RAWToYRow_NEON(const uint8_t* src_raw, uint8_t* dst_y, int width);
void BGRAToYRow_MSA(const uint8_t* src_argb, uint8_t* dst_y, int width);
void ABGRToYRow_MSA(const uint8_t* src_argb, uint8_t* dst_y, int width);
void RGBAToYRow_MSA(const uint8_t* src_argb, uint8_t* dst_y, int width);
void RAWToYRow_MSA(const uint8_t* src_argb, uint8_t* dst_y, int width);
void BGRAToYRow_MMI(const uint8_t* src_argb, uint8_t* dst_y, int width);
void ABGRToYRow_MMI(const uint8_t* src_argb, uint8_t* dst_y, int width);
void RGBAToYRow_MMI(const uint8_t* src_argb, uint8_t* dst_y, int width);
void RAWToYRow_MMI(const uint8_t* src_argb, uint8_t* dst_y, int width);

void ARGBToYRow_C(const uint8_t* src_rgb, uint8_t* dst_y, int width);
void ARGBToYJRow_C(const uint8_t* src_rgb, uint8_t* dst_y, int width);
void RGBAToYJRow_C(const uint8_t* src_rgb, uint8_t* dst_y, int width);
void BGRAToYRow_C(const uint8_t* src_rgb, uint8_t* dst_y, int width);
void ABGRToYRow_C(const uint8_t* src_rgb, uint8_t* dst_y, int width);
void RGBAToYRow_C(const uint8_t* src_rgb, uint8_t* dst_y, int width);
void RAWToYRow_C(const uint8_t* src_rgb, uint8_t* dst_y, int width);
void ARGBToYRow_Any_SSSE3(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void BGRAToYRow_Any_SSSE3(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void ABGRToYRow_Any_SSSE3(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void RGBAToYRow_Any_SSSE3(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void RGB24ToYRow_Any_SSSE3(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void RAWToYRow_Any_SSSE3(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void ARGBToYRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void BGRAToYRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void ABGRToYRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void RGBAToYRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void RGB24ToYRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void RAWToYRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void BGRAToYRow_Any_MSA(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void ABGRToYRow_Any_MSA(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void RGBAToYRow_Any_MSA(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void ARGBToYRow_Any_MSA(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void BGRAToYRow_Any_MMI(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void ABGRToYRow_Any_MMI(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void RGBAToYRow_Any_MMI(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void ARGBToYRow_Any_MMI(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);

void ARGBToUVRow_AVX2(const uint8_t* src_argb,
                      int src_stride_argb,
                      uint8_t* dst_u,
                      uint8_t* dst_v,
                      int width);
void ABGRToUVRow_AVX2(const uint8_t* src_abgr,
                      int src_stride_abgr,
                      uint8_t* dst_u,
                      uint8_t* dst_v,
                      int width);
void ARGBToUVRow_SSSE3(const uint8_t* src_argb,
                       int src_stride_argb,
                       uint8_t* dst_u,
                       uint8_t* dst_v,
                       int width);
void BGRAToUVRow_SSSE3(const uint8_t* src_bgra,
                       int src_stride_bgra,
                       uint8_t* dst_u,
                       uint8_t* dst_v,
                       int width);
void ABGRToUVRow_SSSE3(const uint8_t* src_abgr,
                       int src_stride_abgr,
                       uint8_t* dst_u,
                       uint8_t* dst_v,
                       int width);
void RGBAToUVRow_SSSE3(const uint8_t* src_rgba,
                       int src_stride_rgba,
                       uint8_t* dst_u,
                       uint8_t* dst_v,
                       int width);
void ARGBToUVRow_Any_AVX2(const uint8_t* src_ptr,
                          int src_stride,
                          uint8_t* dst_u,
                          uint8_t* dst_v,
                          int width);
void ABGRToUVRow_Any_AVX2(const uint8_t* src_ptr,
                          int src_stride,
                          uint8_t* dst_u,
                          uint8_t* dst_v,
                          int width);
void ARGBToUVRow_Any_SSSE3(const uint8_t* src_ptr,
                           int src_stride,
                           uint8_t* dst_u,
                           uint8_t* dst_v,
                           int width);
void BGRAToUVRow_Any_SSSE3(const uint8_t* src_ptr,
                           int src_stride,
                           uint8_t* dst_u,
                           uint8_t* dst_v,
                           int width);
void ABGRToUVRow_Any_SSSE3(const uint8_t* src_ptr,
                           int src_stride,
                           uint8_t* dst_u,
                           uint8_t* dst_v,
                           int width);
void RGBAToUVRow_Any_SSSE3(const uint8_t* src_ptr,
                           int src_stride,
                           uint8_t* dst_u,
                           uint8_t* dst_v,
                           int width);
void ARGBToUVRow_Any_NEON(const uint8_t* src_ptr,
                          int src_stride,
                          uint8_t* dst_u,
                          uint8_t* dst_v,
                          int width);
void ARGBToUVRow_Any_MSA(const uint8_t* src_ptr,
                         int src_stride_ptr,
                         uint8_t* dst_u,
                         uint8_t* dst_v,
                         int width);
void ARGBToUVRow_Any_MMI(const uint8_t* src_ptr,
                         int src_stride_ptr,
                         uint8_t* dst_u,
                         uint8_t* dst_v,
                         int width);
void BGRAToUVRow_Any_NEON(const uint8_t* src_ptr,
                          int src_stride,
                          uint8_t* dst_u,
                          uint8_t* dst_v,
                          int width);
void ABGRToUVRow_Any_NEON(const uint8_t* src_ptr,
                          int src_stride,
                          uint8_t* dst_u,
                          uint8_t* dst_v,
                          int width);
void RGBAToUVRow_Any_NEON(const uint8_t* src_ptr,
                          int src_stride,
                          uint8_t* dst_u,
                          uint8_t* dst_v,
                          int width);
void BGRAToUVRow_Any_MSA(const uint8_t* src_ptr,
                         int src_stride_ptr,
                         uint8_t* dst_u,
                         uint8_t* dst_v,
                         int width);
void ABGRToUVRow_Any_MSA(const uint8_t* src_ptr,
                         int src_stride_ptr,
                         uint8_t* dst_u,
                         uint8_t* dst_v,
                         int width);
void RGBAToUVRow_Any_MSA(const uint8_t* src_ptr,
                         int src_stride_ptr,
                         uint8_t* dst_u,
                         uint8_t* dst_v,
                         int width);
void BGRAToUVRow_Any_MMI(const uint8_t* src_ptr,
                         int src_stride_ptr,
                         uint8_t* dst_u,
                         uint8_t* dst_v,
                         int width);
void ABGRToUVRow_Any_MMI(const uint8_t* src_ptr,
                         int src_stride_ptr,
                         uint8_t* dst_u,
                         uint8_t* dst_v,
                         int width);
void RGBAToUVRow_Any_MMI(const uint8_t* src_ptr,
                         int src_stride_ptr,
                         uint8_t* dst_u,
                         uint8_t* dst_v,
                         int width);
void ARGBToUVRow_C(const uint8_t* src_rgb,
                   int src_stride_rgb,
                   uint8_t* dst_u,
                   uint8_t* dst_v,
                   int width);
void ARGBToUVRow_C(const uint8_t* src_rgb,
                   int src_stride_rgb,
                   uint8_t* dst_u,
                   uint8_t* dst_v,
                   int width);
void BGRAToUVRow_C(const uint8_t* src_rgb,
                   int src_stride_rgb,
                   uint8_t* dst_u,
                   uint8_t* dst_v,
                   int width);
void ABGRToUVRow_C(const uint8_t* src_rgb,
                   int src_stride_rgb,
                   uint8_t* dst_u,
                   uint8_t* dst_v,
                   int width);
void RGBAToUVRow_C(const uint8_t* src_rgb,
                   int src_stride_rgb,
                   uint8_t* dst_u,
                   uint8_t* dst_v,
                   int width);

void CopyRow_SSE2(const uint8_t* src, uint8_t* dst, int width);
void CopyRow_AVX(const uint8_t* src, uint8_t* dst, int width);
void CopyRow_ERMS(const uint8_t* src, uint8_t* dst, int width);
void CopyRow_NEON(const uint8_t* src, uint8_t* dst, int width);
void CopyRow_MIPS(const uint8_t* src, uint8_t* dst, int count);
void CopyRow_C(const uint8_t* src, uint8_t* dst, int count);
void CopyRow_Any_SSE2(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void CopyRow_Any_AVX(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void CopyRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);

void CopyRow_16_C(const uint16_t* src, uint16_t* dst, int count);

void ARGBCopyAlphaRow_C(const uint8_t* src, uint8_t* dst, int width);
void ARGBCopyAlphaRow_SSE2(const uint8_t* src, uint8_t* dst, int width);
void ARGBCopyAlphaRow_AVX2(const uint8_t* src, uint8_t* dst, int width);
void ARGBCopyAlphaRow_MMI(const uint8_t* src, uint8_t* dst, int width);
void ARGBCopyAlphaRow_Any_SSE2(const uint8_t* src_ptr,
                               uint8_t* dst_ptr,
                               int width);
void ARGBCopyAlphaRow_Any_AVX2(const uint8_t* src_ptr,
                               uint8_t* dst_ptr,
                               int width);
void ARGBCopyAlphaRow_Any_MMI(const uint8_t* src_ptr,
                              uint8_t* dst_ptr,
                              int width);

void ARGBExtractAlphaRow_C(const uint8_t* src_argb, uint8_t* dst_a, int width);
void ARGBExtractAlphaRow_SSE2(const uint8_t* src_argb,
                              uint8_t* dst_a,
                              int width);
void ARGBExtractAlphaRow_AVX2(const uint8_t* src_argb,
                              uint8_t* dst_a,
                              int width);
void ARGBExtractAlphaRow_NEON(const uint8_t* src_argb,
                              uint8_t* dst_a,
                              int width);
void ARGBExtractAlphaRow_MSA(const uint8_t* src_argb,
                             uint8_t* dst_a,
                             int width);
void ARGBExtractAlphaRow_MMI(const uint8_t* src_argb,
                             uint8_t* dst_a,
                             int width);
void ARGBExtractAlphaRow_Any_SSE2(const uint8_t* src_ptr,
                                  uint8_t* dst_ptr,
                                  int width);
void ARGBExtractAlphaRow_Any_AVX2(const uint8_t* src_ptr,
                                  uint8_t* dst_ptr,
                                  int width);
void ARGBExtractAlphaRow_Any_NEON(const uint8_t* src_ptr,
                                  uint8_t* dst_ptr,
                                  int width);
void ARGBExtractAlphaRow_Any_MSA(const uint8_t* src_ptr,
                                 uint8_t* dst_ptr,
                                 int width);
void ARGBExtractAlphaRow_Any_MMI(const uint8_t* src_ptr,
                                 uint8_t* dst_ptr,
                                 int width);

void ARGBCopyYToAlphaRow_C(const uint8_t* src, uint8_t* dst, int width);
void ARGBCopyYToAlphaRow_SSE2(const uint8_t* src, uint8_t* dst, int width);
void ARGBCopyYToAlphaRow_AVX2(const uint8_t* src, uint8_t* dst, int width);
void ARGBCopyYToAlphaRow_MMI(const uint8_t* src, uint8_t* dst, int width);
void ARGBCopyYToAlphaRow_Any_SSE2(const uint8_t* src_ptr,
                                  uint8_t* dst_ptr,
                                  int width);
void ARGBCopyYToAlphaRow_Any_AVX2(const uint8_t* src_ptr,
                                  uint8_t* dst_ptr,
                                  int width);
void ARGBCopyYToAlphaRow_Any_MMI(const uint8_t* src_ptr,
                                 uint8_t* dst_ptr,
                                 int width);

void SetRow_C(uint8_t* dst, uint8_t v8, int width);
void SetRow_MSA(uint8_t* dst, uint8_t v8, int width);
void SetRow_X86(uint8_t* dst, uint8_t v8, int width);
void SetRow_ERMS(uint8_t* dst, uint8_t v8, int width);
void SetRow_NEON(uint8_t* dst, uint8_t v8, int width);
void SetRow_Any_X86(uint8_t* dst_ptr, uint8_t v32, int width);
void SetRow_Any_NEON(uint8_t* dst_ptr, uint8_t v32, int width);

void ARGBSetRow_C(uint8_t* dst_argb, uint32_t v32, int width);
void ARGBSetRow_X86(uint8_t* dst_argb, uint32_t v32, int width);
void ARGBSetRow_NEON(uint8_t* dst, uint32_t v32, int width);
void ARGBSetRow_Any_NEON(uint8_t* dst_ptr, uint32_t v32, int width);
void ARGBSetRow_MSA(uint8_t* dst_argb, uint32_t v32, int width);
void ARGBSetRow_Any_MSA(uint8_t* dst_ptr, uint32_t v32, int width);
void ARGBSetRow_MMI(uint8_t* dst_argb, uint32_t v32, int width);
void ARGBSetRow_Any_MMI(uint8_t* dst_ptr, uint32_t v32, int width);

// ARGBShufflers for BGRAToARGB etc.
void ARGBShuffleRow_C(const uint8_t* src_argb,
                      uint8_t* dst_argb,
                      const uint8_t* shuffler,
                      int width);
void ARGBShuffleRow_SSSE3(const uint8_t* src_argb,
                          uint8_t* dst_argb,
                          const uint8_t* shuffler,
                          int width);
void ARGBShuffleRow_AVX2(const uint8_t* src_argb,
                         uint8_t* dst_argb,
                         const uint8_t* shuffler,
                         int width);
void ARGBShuffleRow_NEON(const uint8_t* src_argb,
                         uint8_t* dst_argb,
                         const uint8_t* shuffler,
                         int width);
void ARGBShuffleRow_MSA(const uint8_t* src_argb,
                        uint8_t* dst_argb,
                        const uint8_t* shuffler,
                        int width);
void ARGBShuffleRow_MMI(const uint8_t* src_argb,
                        uint8_t* dst_argb,
                        const uint8_t* shuffler,
                        int width);
void ARGBShuffleRow_Any_SSSE3(const uint8_t* src_ptr,
                              uint8_t* dst_ptr,
                              const uint8_t* param,
                              int width);
void ARGBShuffleRow_Any_AVX2(const uint8_t* src_ptr,
                             uint8_t* dst_ptr,
                             const uint8_t* param,
                             int width);
void ARGBShuffleRow_Any_NEON(const uint8_t* src_ptr,
                             uint8_t* dst_ptr,
                             const uint8_t* param,
                             int width);
void ARGBShuffleRow_Any_MSA(const uint8_t* src_ptr,
                            uint8_t* dst_ptr,
                            const uint8_t* param,
                            int width);
void ARGBShuffleRow_Any_MMI(const uint8_t* src_ptr,
                            uint8_t* dst_ptr,
                            const uint8_t* param,
                            int width);

void RGB24ToARGBRow_SSSE3(const uint8_t* src_rgb24,
                          uint8_t* dst_argb,
                          int width);
void RAWToARGBRow_SSSE3(const uint8_t* src_raw, uint8_t* dst_argb, int width);
void RAWToRGBARow_SSSE3(const uint8_t* src_raw, uint8_t* dst_rgba, int width);
void RAWToRGB24Row_SSSE3(const uint8_t* src_raw, uint8_t* dst_rgb24, int width);

void RGB24ToARGBRow_NEON(const uint8_t* src_rgb24,
                         uint8_t* dst_argb,
                         int width);
void RGB24ToARGBRow_MSA(const uint8_t* src_rgb24, uint8_t* dst_argb, int width);
void RGB24ToARGBRow_MMI(const uint8_t* src_rgb24, uint8_t* dst_argb, int width);
void RAWToARGBRow_NEON(const uint8_t* src_raw, uint8_t* dst_argb, int width);
void RAWToRGBARow_NEON(const uint8_t* src_raw, uint8_t* dst_rgba, int width);
void RAWToARGBRow_MSA(const uint8_t* src_raw, uint8_t* dst_argb, int width);
void RAWToARGBRow_MMI(const uint8_t* src_raw, uint8_t* dst_argb, int width);
void RAWToRGB24Row_NEON(const uint8_t* src_raw, uint8_t* dst_rgb24, int width);
void RAWToRGB24Row_MSA(const uint8_t* src_raw, uint8_t* dst_rgb24, int width);
void RAWToRGB24Row_MMI(const uint8_t* src_raw, uint8_t* dst_rgb24, int width);
void RGB24ToARGBRow_C(const uint8_t* src_rgb24, uint8_t* dst_argb, int width);
void RAWToARGBRow_C(const uint8_t* src_raw, uint8_t* dst_argb, int width);
void RAWToRGBARow_C(const uint8_t* src_raw, uint8_t* dst_rgba, int width);
void RAWToRGB24Row_C(const uint8_t* src_raw, uint8_t* dst_rgb24, int width);

void RGB24ToARGBRow_Any_SSSE3(const uint8_t* src_ptr,
                              uint8_t* dst_ptr,
                              int width);
void RAWToARGBRow_Any_SSSE3(const uint8_t* src_ptr,
                            uint8_t* dst_ptr,
                            int width);
void RAWToRGBARow_Any_SSSE3(const uint8_t* src_ptr,
                            uint8_t* dst_ptr,
                            int width);
void RAWToRGB24Row_Any_SSSE3(const uint8_t* src_ptr,
                             uint8_t* dst_ptr,
                             int width);

void RGB24ToARGBRow_Any_NEON(const uint8_t* src_ptr,
                             uint8_t* dst_ptr,
                             int width);
void RGB24ToARGBRow_Any_MSA(const uint8_t* src_ptr,
                            uint8_t* dst_ptr,
                            int width);
void RGB24ToARGBRow_Any_MMI(const uint8_t* src_ptr,
                            uint8_t* dst_ptr,
                            int width);
void RAWToARGBRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void RAWToRGBARow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void RAWToARGBRow_Any_MSA(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void RAWToARGBRow_Any_MMI(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void RAWToRGB24Row_Any_NEON(const uint8_t* src_ptr,
                            uint8_t* dst_ptr,
                            int width);
void RAWToRGB24Row_Any_MSA(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void RAWToRGB24Row_Any_MMI(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);

void ARGBToRGB24Row_SSSE3(const uint8_t* src, uint8_t* dst, int width);
void ARGBToRAWRow_SSSE3(const uint8_t* src, uint8_t* dst, int width);

void ARGBToRAWRow_AVX2(const uint8_t* src, uint8_t* dst, int width);
void ARGBToRGB24Row_AVX2(const uint8_t* src, uint8_t* dst, int width);

void ARGBToRGB24Row_AVX512VBMI(const uint8_t* src, uint8_t* dst, int width);

void ARGBToRGB24Row_NEON(const uint8_t* src_argb,
                         uint8_t* dst_rgb24,
                         int width);
void ARGBToRAWRow_NEON(const uint8_t* src_argb, uint8_t* dst_raw, int width);
void ARGBToRGB24Row_MSA(const uint8_t* src_argb, uint8_t* dst_rgb, int width);
void ARGBToRAWRow_MSA(const uint8_t* src_argb, uint8_t* dst_rgb, int width);

void ARGBToRGB24Row_MMI(const uint8_t* src_argb, uint8_t* dst_rgb, int width);
void ARGBToRAWRow_MMI(const uint8_t* src_argb, uint8_t* dst_rgb, int width);

void ARGBToRGBARow_C(const uint8_t* src_argb, uint8_t* dst_rgb, int width);
void ARGBToRGB24Row_C(const uint8_t* src_argb, uint8_t* dst_rgb, int width);
void ARGBToRAWRow_C(const uint8_t* src_argb, uint8_t* dst_rgb, int width);

void I422ToARGBRow_C(const uint8_t* src_y,
                     const uint8_t* src_u,
                     const uint8_t* src_v,
                     uint8_t* rgb_buf,
                     const struct YuvConstants* yuvconstants,
                     int width);
void I422ToAR30Row_C(const uint8_t* src_y,
                     const uint8_t* src_u,
                     const uint8_t* src_v,
                     uint8_t* rgb_buf,
                     const struct YuvConstants* yuvconstants,
                     int width);
void I422AlphaToARGBRow_C(const uint8_t* src_y,
                          const uint8_t* src_u,
                          const uint8_t* src_v,
                          const uint8_t* src_a,
                          uint8_t* rgb_buf,
                          const struct YuvConstants* yuvconstants,
                          int width);
void UYVYToARGBRow_C(const uint8_t* src_uyvy,
                     uint8_t* rgb_buf,
                     const struct YuvConstants* yuvconstants,
                     int width);

void I422ToRGBARow_C(const uint8_t* src_y,
                     const uint8_t* src_u,
                     const uint8_t* src_v,
                     uint8_t* rgb_buf,
                     const struct YuvConstants* yuvconstants,
                     int width);
void I422ToRGB24Row_C(const uint8_t* src_y,
                      const uint8_t* src_u,
                      const uint8_t* src_v,
                      uint8_t* rgb_buf,
                      const struct YuvConstants* yuvconstants,
                      int width);
void I422ToARGB4444Row_C(const uint8_t* src_y,
                         const uint8_t* src_u,
                         const uint8_t* src_v,
                         uint8_t* dst_argb4444,
                         const struct YuvConstants* yuvconstants,
                         int width);
void I422ToARGB1555Row_C(const uint8_t* src_y,
                         const uint8_t* src_u,
                         const uint8_t* src_v,
                         uint8_t* dst_argb1555,
                         const struct YuvConstants* yuvconstants,
                         int width);
void I422ToRGB565Row_C(const uint8_t* src_y,
                       const uint8_t* src_u,
                       const uint8_t* src_v,
                       uint8_t* dst_rgb565,
                       const struct YuvConstants* yuvconstants,
                       int width);
void I422ToARGBRow_AVX2(const uint8_t* y_buf,
                        const uint8_t* u_buf,
                        const uint8_t* v_buf,
                        uint8_t* dst_argb,
                        const struct YuvConstants* yuvconstants,
                        int width);
void I422ToRGBARow_AVX2(const uint8_t* y_buf,
                        const uint8_t* u_buf,
                        const uint8_t* v_buf,
                        uint8_t* dst_argb,
                        const struct YuvConstants* yuvconstants,
                        int width);
void I422ToARGBRow_SSSE3(const uint8_t* y_buf,
                         const uint8_t* u_buf,
                         const uint8_t* v_buf,
                         uint8_t* dst_argb,
                         const struct YuvConstants* yuvconstants,
                         int width);

void I422ToAR30Row_SSSE3(const uint8_t* y_buf,
                         const uint8_t* u_buf,
                         const uint8_t* v_buf,
                         uint8_t* dst_ar30,
                         const struct YuvConstants* yuvconstants,
                         int width);
void I422AlphaToARGBRow_SSSE3(const uint8_t* y_buf,
                              const uint8_t* u_buf,
                              const uint8_t* v_buf,
                              const uint8_t* a_buf,
                              uint8_t* dst_argb,
                              const struct YuvConstants* yuvconstants,
                              int width);
void I422AlphaToARGBRow_AVX2(const uint8_t* y_buf,
                             const uint8_t* u_buf,
                             const uint8_t* v_buf,
                             const uint8_t* a_buf,
                             uint8_t* dst_argb,
                             const struct YuvConstants* yuvconstants,
                             int width);
void UYVYToARGBRow_SSSE3(const uint8_t* uyvy_buf,
                         uint8_t* dst_argb,
                         const struct YuvConstants* yuvconstants,
                         int width);
void UYVYToARGBRow_AVX2(const uint8_t* uyvy_buf,
                        uint8_t* dst_argb,
                        const struct YuvConstants* yuvconstants,
                        int width);

void I422ToRGBARow_SSSE3(const uint8_t* y_buf,
                         const uint8_t* u_buf,
                         const uint8_t* v_buf,
                         uint8_t* dst_rgba,
                         const struct YuvConstants* yuvconstants,
                         int width);
void I422ToARGBRow_Any_AVX2(const uint8_t* y_buf,
                            const uint8_t* u_buf,
                            const uint8_t* v_buf,
                            uint8_t* dst_ptr,
                            const struct YuvConstants* yuvconstants,
                            int width);
void I422ToRGBARow_Any_AVX2(const uint8_t* y_buf,
                            const uint8_t* u_buf,
                            const uint8_t* v_buf,
                            uint8_t* dst_ptr,
                            const struct YuvConstants* yuvconstants,
                            int width);
void I422ToARGBRow_Any_SSSE3(const uint8_t* y_buf,
                             const uint8_t* u_buf,
                             const uint8_t* v_buf,
                             uint8_t* dst_ptr,
                             const struct YuvConstants* yuvconstants,
                             int width);
void I422ToAR30Row_Any_SSSE3(const uint8_t* y_buf,
                             const uint8_t* u_buf,
                             const uint8_t* v_buf,
                             uint8_t* dst_ptr,
                             const struct YuvConstants* yuvconstants,
                             int width);
void I422AlphaToARGBRow_Any_SSSE3(const uint8_t* y_buf,
                                  const uint8_t* u_buf,
                                  const uint8_t* v_buf,
                                  const uint8_t* a_buf,
                                  uint8_t* dst_ptr,
                                  const struct YuvConstants* yuvconstants,
                                  int width);
void I422AlphaToARGBRow_Any_AVX2(const uint8_t* y_buf,
                                 const uint8_t* u_buf,
                                 const uint8_t* v_buf,
                                 const uint8_t* a_buf,
                                 uint8_t* dst_ptr,
                                 const struct YuvConstants* yuvconstants,
                                 int width);
void UYVYToARGBRow_Any_SSSE3(const uint8_t* src_ptr,
                             uint8_t* dst_ptr,
                             const struct YuvConstants* yuvconstants,
                             int width);
void UYVYToARGBRow_Any_AVX2(const uint8_t* src_ptr,
                            uint8_t* dst_ptr,
                            const struct YuvConstants* yuvconstants,
                            int width);
void I422ToRGBARow_Any_SSSE3(const uint8_t* y_buf,
                             const uint8_t* u_buf,
                             const uint8_t* v_buf,
                             uint8_t* dst_ptr,
                             const struct YuvConstants* yuvconstants,
                             int width);

void ARGBToRGB24Row_Any_SSSE3(const uint8_t* src_ptr,
                              uint8_t* dst_ptr,
                              int width);
void ARGBToRAWRow_Any_SSSE3(const uint8_t* src_ptr,
                            uint8_t* dst_ptr,
                            int width);
void ARGBToRAWRow_Any_AVX2(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void ARGBToRGB24Row_Any_AVX2(const uint8_t* src_ptr,
                             uint8_t* dst_ptr,
                             int width);
void ARGBToRGB24Row_Any_AVX512VBMI(const uint8_t* src_ptr,
                                   uint8_t* dst_ptr,
                                   int width);

void ARGBToRGB24Row_Any_NEON(const uint8_t* src_ptr,
                             uint8_t* dst_ptr,
                             int width);
void ARGBToRAWRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void ARGBToRGB24Row_Any_MSA(const uint8_t* src_ptr,
                            uint8_t* dst_ptr,
                            int width);
void ARGBToRAWRow_Any_MSA(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);

void ARGBToRGB24Row_Any_MMI(const uint8_t* src_ptr,
                            uint8_t* dst_ptr,
                            int width);
void ARGBToRAWRow_Any_MMI(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);

void I422ToARGBRow_Any_NEON(const uint8_t* y_buf,
                            const uint8_t* u_buf,
                            const uint8_t* v_buf,
                            uint8_t* dst_ptr,
                            const struct YuvConstants* yuvconstants,
                            int width);
void I422AlphaToARGBRow_Any_NEON(const uint8_t* y_buf,
                                 const uint8_t* u_buf,
                                 const uint8_t* v_buf,
                                 const uint8_t* a_buf,
                                 uint8_t* dst_ptr,
                                 const struct YuvConstants* yuvconstants,
                                 int width);
void I422ToRGBARow_Any_NEON(const uint8_t* y_buf,
                            const uint8_t* u_buf,
                            const uint8_t* v_buf,
                            uint8_t* dst_ptr,
                            const struct YuvConstants* yuvconstants,
                            int width);
void UYVYToARGBRow_Any_NEON(const uint8_t* src_ptr,
                            uint8_t* dst_ptr,
                            const struct YuvConstants* yuvconstants,
                            int width);
void I422ToARGBRow_Any_MSA(const uint8_t* y_buf,
                           const uint8_t* u_buf,
                           const uint8_t* v_buf,
                           uint8_t* dst_ptr,
                           const struct YuvConstants* yuvconstants,
                           int width);
void I422ToARGBRow_Any_MMI(const uint8_t* y_buf,
                           const uint8_t* u_buf,
                           const uint8_t* v_buf,
                           uint8_t* dst_ptr,
                           const struct YuvConstants* yuvconstants,
                           int width);
void I422ToRGBARow_Any_MSA(const uint8_t* y_buf,
                           const uint8_t* u_buf,
                           const uint8_t* v_buf,
                           uint8_t* dst_ptr,
                           const struct YuvConstants* yuvconstants,
                           int width);
void I422AlphaToARGBRow_Any_MSA(const uint8_t* y_buf,
                                const uint8_t* u_buf,
                                const uint8_t* v_buf,
                                const uint8_t* a_buf,
                                uint8_t* dst_ptr,
                                const struct YuvConstants* yuvconstants,
                                int width);
void UYVYToARGBRow_Any_MSA(const uint8_t* src_ptr,
                           uint8_t* dst_ptr,
                           const struct YuvConstants* yuvconstants,
                           int width);

void UYVYToYRow_AVX2(const uint8_t* src_uyvy, uint8_t* dst_y, int width);
void UYVYToUVRow_AVX2(const uint8_t* src_uyvy,
                      int stride_uyvy,
                      uint8_t* dst_u,
                      uint8_t* dst_v,
                      int width);
void UYVYToUV422Row_AVX2(const uint8_t* src_uyvy,
                         uint8_t* dst_u,
                         uint8_t* dst_v,
                         int width);
void UYVYToYRow_SSE2(const uint8_t* src_uyvy, uint8_t* dst_y, int width);
void UYVYToUVRow_SSE2(const uint8_t* src_uyvy,
                      int stride_uyvy,
                      uint8_t* dst_u,
                      uint8_t* dst_v,
                      int width);
void UYVYToUV422Row_SSE2(const uint8_t* src_uyvy,
                         uint8_t* dst_u,
                         uint8_t* dst_v,
                         int width);
void UYVYToYRow_AVX2(const uint8_t* src_uyvy, uint8_t* dst_y, int width);
void UYVYToUVRow_AVX2(const uint8_t* src_uyvy,
                      int stride_uyvy,
                      uint8_t* dst_u,
                      uint8_t* dst_v,
                      int width);
void UYVYToUV422Row_AVX2(const uint8_t* src_uyvy,
                         uint8_t* dst_u,
                         uint8_t* dst_v,
                         int width);
void UYVYToYRow_NEON(const uint8_t* src_uyvy, uint8_t* dst_y, int width);
void UYVYToUVRow_NEON(const uint8_t* src_uyvy,
                      int stride_uyvy,
                      uint8_t* dst_u,
                      uint8_t* dst_v,
                      int width);
void UYVYToUV422Row_NEON(const uint8_t* src_uyvy,
                         uint8_t* dst_u,
                         uint8_t* dst_v,
                         int width);
void UYVYToYRow_MSA(const uint8_t* src_uyvy, uint8_t* dst_y, int width);
void UYVYToYRow_MMI(const uint8_t* src_uyvy, uint8_t* dst_y, int width);
void UYVYToUVRow_MSA(const uint8_t* src_uyvy,
                     int src_stride_uyvy,
                     uint8_t* dst_u,
                     uint8_t* dst_v,
                     int width);
void UYVYToUVRow_MMI(const uint8_t* src_uyvy,
                     int src_stride_uyvy,
                     uint8_t* dst_u,
                     uint8_t* dst_v,
                     int width);
void UYVYToUV422Row_MSA(const uint8_t* src_uyvy,
                        uint8_t* dst_u,
                        uint8_t* dst_v,
                        int width);
void UYVYToUV422Row_MMI(const uint8_t* src_uyvy,
                        uint8_t* dst_u,
                        uint8_t* dst_v,
                        int width);

void UYVYToYRow_C(const uint8_t* src_uyvy, uint8_t* dst_y, int width);
void UYVYToUVRow_C(const uint8_t* src_uyvy,
                   int src_stride_uyvy,
                   uint8_t* dst_u,
                   uint8_t* dst_v,
                   int width);
void UYVYToUV422Row_C(const uint8_t* src_uyvy,
                      uint8_t* dst_u,
                      uint8_t* dst_v,
                      int width);
void UYVYToYRow_Any_AVX2(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void UYVYToUVRow_Any_AVX2(const uint8_t* src_ptr,
                          int src_stride,
                          uint8_t* dst_u,
                          uint8_t* dst_v,
                          int width);
void UYVYToUV422Row_Any_AVX2(const uint8_t* src_ptr,
                             uint8_t* dst_u,
                             uint8_t* dst_v,
                             int width);
void UYVYToYRow_Any_SSE2(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void UYVYToUVRow_Any_SSE2(const uint8_t* src_ptr,
                          int src_stride,
                          uint8_t* dst_u,
                          uint8_t* dst_v,
                          int width);
void UYVYToUV422Row_Any_SSE2(const uint8_t* src_ptr,
                             uint8_t* dst_u,
                             uint8_t* dst_v,
                             int width);
void UYVYToYRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void UYVYToUVRow_Any_NEON(const uint8_t* src_ptr,
                          int src_stride,
                          uint8_t* dst_u,
                          uint8_t* dst_v,
                          int width);
void UYVYToUV422Row_Any_NEON(const uint8_t* src_ptr,
                             uint8_t* dst_u,
                             uint8_t* dst_v,
                             int width);
void UYVYToYRow_Any_MSA(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void UYVYToYRow_Any_MMI(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void UYVYToUVRow_Any_MSA(const uint8_t* src_ptr,
                         int src_stride_ptr,
                         uint8_t* dst_u,
                         uint8_t* dst_v,
                         int width);
void UYVYToUVRow_Any_MMI(const uint8_t* src_ptr,
                         int src_stride_ptr,
                         uint8_t* dst_u,
                         uint8_t* dst_v,
                         int width);
void UYVYToUV422Row_Any_MSA(const uint8_t* src_ptr,
                            uint8_t* dst_u,
                            uint8_t* dst_v,
                            int width);
void UYVYToUV422Row_Any_MMI(const uint8_t* src_ptr,
                            uint8_t* dst_u,
                            uint8_t* dst_v,
                            int width);
void SwapUVRow_C(const uint8_t* src_uv, uint8_t* dst_vu, int width);
void SwapUVRow_NEON(const uint8_t* src_uv, uint8_t* dst_vu, int width);
void SwapUVRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void SwapUVRow_SSSE3(const uint8_t* src_uv, uint8_t* dst_vu, int width);
void SwapUVRow_Any_SSSE3(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void SwapUVRow_AVX2(const uint8_t* src_uv, uint8_t* dst_vu, int width);
void SwapUVRow_Any_AVX2(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void AYUVToYRow_C(const uint8_t* src_ayuv, uint8_t* dst_y, int width);
void AYUVToUVRow_C(const uint8_t* src_ayuv,
                   int src_stride_ayuv,
                   uint8_t* dst_uv,
                   int width);
void AYUVToVURow_C(const uint8_t* src_ayuv,
                   int src_stride_ayuv,
                   uint8_t* dst_vu,
                   int width);
void AYUVToYRow_NEON(const uint8_t* src_ayuv, uint8_t* dst_y, int width);
void AYUVToUVRow_NEON(const uint8_t* src_ayuv,
                      int src_stride_ayuv,
                      uint8_t* dst_uv,
                      int width);
void AYUVToVURow_NEON(const uint8_t* src_ayuv,
                      int src_stride_ayuv,
                      uint8_t* dst_vu,
                      int width);
void AYUVToYRow_Any_NEON(const uint8_t* src_ptr, uint8_t* dst_ptr, int width);
void AYUVToUVRow_Any_NEON(const uint8_t* src_ptr,
                          int src_stride,
                          uint8_t* dst_vu,
                          int width);
void AYUVToVURow_Any_NEON(const uint8_t* src_ptr,
                          int src_stride,
                          uint8_t* dst_vu,
                          int width);

void I422ToUYVYRow_C(const uint8_t* src_y,
                     const uint8_t* src_u,
                     const uint8_t* src_v,
                     uint8_t* dst_frame,
                     int width);
void I422ToUYVYRow_SSE2(const uint8_t* src_y,
                        const uint8_t* src_u,
                        const uint8_t* src_v,
                        uint8_t* dst_uyvy,
                        int width);
void I422ToUYVYRow_Any_SSE2(const uint8_t* y_buf,
                            const uint8_t* u_buf,
                            const uint8_t* v_buf,
                            uint8_t* dst_ptr,
                            int width);
void I422ToUYVYRow_AVX2(const uint8_t* src_y,
                        const uint8_t* src_u,
                        const uint8_t* src_v,
                        uint8_t* dst_uyvy,
                        int width);
void I422ToUYVYRow_Any_AVX2(const uint8_t* y_buf,
                            const uint8_t* u_buf,
                            const uint8_t* v_buf,
                            uint8_t* dst_ptr,
                            int width);
void I422ToUYVYRow_NEON(const uint8_t* src_y,
                        const uint8_t* src_u,
                        const uint8_t* src_v,
                        uint8_t* dst_uyvy,
                        int width);
void I422ToUYVYRow_Any_NEON(const uint8_t* y_buf,
                            const uint8_t* u_buf,
                            const uint8_t* v_buf,
                            uint8_t* dst_ptr,
                            int width);
void I422ToUYVYRow_MSA(const uint8_t* src_y,
                       const uint8_t* src_u,
                       const uint8_t* src_v,
                       uint8_t* dst_uyvy,
                       int width);
void I422ToUYVYRow_MMI(const uint8_t* src_y,
                       const uint8_t* src_u,
                       const uint8_t* src_v,
                       uint8_t* dst_uyvy,
                       int width);
void I422ToUYVYRow_Any_MSA(const uint8_t* y_buf,
                           const uint8_t* u_buf,
                           const uint8_t* v_buf,
                           uint8_t* dst_ptr,
                           int width);
void I422ToUYVYRow_Any_MMI(const uint8_t* y_buf,
                           const uint8_t* u_buf,
                           const uint8_t* v_buf,
                           uint8_t* dst_ptr,
                           int width);

// Effects related row functions.
void ARGBAttenuateRow_C(const uint8_t* src_argb, uint8_t* dst_argb, int width);
void ARGBAttenuateRow_SSSE3(const uint8_t* src_argb,
                            uint8_t* dst_argb,
                            int width);
void ARGBAttenuateRow_AVX2(const uint8_t* src_argb,
                           uint8_t* dst_argb,
                           int width);
void ARGBAttenuateRow_NEON(const uint8_t* src_argb,
                           uint8_t* dst_argb,
                           int width);
void ARGBAttenuateRow_MSA(const uint8_t* src_argb,
                          uint8_t* dst_argb,
                          int width);
void ARGBAttenuateRow_MMI(const uint8_t* src_argb,
                          uint8_t* dst_argb,
                          int width);
void ARGBAttenuateRow_Any_SSSE3(const uint8_t* src_ptr,
                                uint8_t* dst_ptr,
                                int width);
void ARGBAttenuateRow_Any_AVX2(const uint8_t* src_ptr,
                               uint8_t* dst_ptr,
                               int width);
void ARGBAttenuateRow_Any_NEON(const uint8_t* src_ptr,
                               uint8_t* dst_ptr,
                               int width);
void ARGBAttenuateRow_Any_MSA(const uint8_t* src_ptr,
                              uint8_t* dst_ptr,
                              int width);
void ARGBAttenuateRow_Any_MMI(const uint8_t* src_ptr,
                              uint8_t* dst_ptr,
                              int width);

// Inverse table for unattenuate, shared by C and SSE2.
extern const uint32_t fixed_invtbl8[256];
void ARGBUnattenuateRow_C(const uint8_t* src_argb,
                          uint8_t* dst_argb,
                          int width);
void ARGBUnattenuateRow_SSE2(const uint8_t* src_argb,
                             uint8_t* dst_argb,
                             int width);
void ARGBUnattenuateRow_AVX2(const uint8_t* src_argb,
                             uint8_t* dst_argb,
                             int width);
void ARGBUnattenuateRow_Any_SSE2(const uint8_t* src_ptr,
                                 uint8_t* dst_ptr,
                                 int width);
void ARGBUnattenuateRow_Any_AVX2(const uint8_t* src_ptr,
                                 uint8_t* dst_ptr,
                                 int width);

// Used for I420Scale, ARGBScale, and ARGBInterpolate.
void InterpolateRow_C(uint8_t* dst_ptr,
                      const uint8_t* src_ptr,
                      ptrdiff_t src_stride,
                      int width,
                      int source_y_fraction);
void InterpolateRow_SSSE3(uint8_t* dst_ptr,
                          const uint8_t* src_ptr,
                          ptrdiff_t src_stride,
                          int dst_width,
                          int source_y_fraction);
void InterpolateRow_AVX2(uint8_t* dst_ptr,
                         const uint8_t* src_ptr,
                         ptrdiff_t src_stride,
                         int dst_width,
                         int source_y_fraction);
void InterpolateRow_NEON(uint8_t* dst_ptr,
                         const uint8_t* src_ptr,
                         ptrdiff_t src_stride,
                         int dst_width,
                         int source_y_fraction);
void InterpolateRow_MSA(uint8_t* dst_ptr,
                        const uint8_t* src_ptr,
                        ptrdiff_t src_stride,
                        int width,
                        int source_y_fraction);
void InterpolateRow_MMI(uint8_t* dst_ptr,
                        const uint8_t* src_ptr,
                        ptrdiff_t src_stride,
                        int width,
                        int source_y_fraction);
void InterpolateRow_Any_NEON(uint8_t* dst_ptr,
                             const uint8_t* src_ptr,
                             ptrdiff_t src_stride_ptr,
                             int width,
                             int source_y_fraction);
void InterpolateRow_Any_SSSE3(uint8_t* dst_ptr,
                              const uint8_t* src_ptr,
                              ptrdiff_t src_stride_ptr,
                              int width,
                              int source_y_fraction);
void InterpolateRow_Any_AVX2(uint8_t* dst_ptr,
                             const uint8_t* src_ptr,
                             ptrdiff_t src_stride_ptr,
                             int width,
                             int source_y_fraction);
void InterpolateRow_Any_MSA(uint8_t* dst_ptr,
                            const uint8_t* src_ptr,
                            ptrdiff_t src_stride_ptr,
                            int width,
                            int source_y_fraction);
void InterpolateRow_Any_MMI(uint8_t* dst_ptr,
                            const uint8_t* src_ptr,
                            ptrdiff_t src_stride_ptr,
                            int width,
                            int source_y_fraction);

void InterpolateRow_16_C(uint16_t* dst_ptr,
                         const uint16_t* src_ptr,
                         ptrdiff_t src_stride,
                         int width,
                         int source_y_fraction);

void I422ToRGBARow_MMI(const uint8_t* src_y,
                       const uint8_t* src_u,
                       const uint8_t* src_v,
                       uint8_t* dst_argb,
                       const struct YuvConstants* yuvconstants,
                       int width);
void I422AlphaToARGBRow_MMI(const uint8_t* src_y,
                            const uint8_t* src_u,
                            const uint8_t* src_v,
                            const uint8_t* src_a,
                            uint8_t* dst_argb,
                            const struct YuvConstants* yuvconstants,
                            int width);
void UYVYToARGBRow_MMI(const uint8_t* src_uyvy,
                       uint8_t* dst_argb,
                       const struct YuvConstants* yuvconstants,
                       int width);
void I422ToRGBARow_Any_MMI(const uint8_t* y_buf,
                           const uint8_t* u_buf,
                           const uint8_t* v_buf,
                           uint8_t* dst_ptr,
                           const struct YuvConstants* yuvconstants,
                           int width);
void I422AlphaToARGBRow_Any_MMI(const uint8_t* y_buf,
                                const uint8_t* u_buf,
                                const uint8_t* v_buf,
                                const uint8_t* a_buf,
                                uint8_t* dst_ptr,
                                const struct YuvConstants* yuvconstants,
                                int width);
void UYVYToARGBRow_Any_MMI(const uint8_t* src_ptr,
                           uint8_t* dst_ptr,
                           const struct YuvConstants* yuvconstants,
                           int width);

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif

#endif  // INCLUDE_LIBYUV_ROW_H_
