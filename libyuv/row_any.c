/*
 *  Copyright 2012 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "libyuv/row.h"

#include <string.h>  // For memset.

#include "libyuv/basic_types.h"

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

// memset for temp is meant to clear the source buffer (not dest) so that
// SIMD that reads full multiple of 16 bytes will not trigger msan errors.
// memset is not needed for production, as the garbage values are processed but
// not used, although there may be edge cases for subsampling.
// The size of the buffer is based on the largest read, which can be inferred
// by the source type (e.g. ARGB) and the mask (last parameter), or by examining
// the source code for how much the source pointers are advanced.

// Subsampled source needs to be increase by 1 of not even.
#define SS(width, shift) (((width) + (1 << (shift)) - 1) >> (shift))

// Note that odd width replication includes 444 due to implementation
// on arm that subsamples 444 to 422 internally.
// Any 4 planes to 1 with yuvconstants
#define ANY41C(NAMEANY, ANY_SIMD, UVSHIFT, DUVSHIFT, BPP, MASK)              \
  void NAMEANY(const uint8_t* y_buf, const uint8_t* u_buf,                   \
               const uint8_t* v_buf, const uint8_t* a_buf, uint8_t* dst_ptr, \
               const struct YuvConstants* yuvconstants, int width) {         \
    SIMD_ALIGNED(uint8_t temp[64 * 5]);                                      \
    memset(temp, 0, 64 * 4); /* for msan */                                  \
    int r = width & MASK;                                                    \
    int n = width & ~MASK;                                                   \
    if (n > 0) {                                                             \
      ANY_SIMD(y_buf, u_buf, v_buf, a_buf, dst_ptr, yuvconstants, n);        \
    }                                                                        \
    memcpy(temp, y_buf + n, r);                                              \
    memcpy(temp + 64, u_buf + (n >> UVSHIFT), SS(r, UVSHIFT));               \
    memcpy(temp + 128, v_buf + (n >> UVSHIFT), SS(r, UVSHIFT));              \
    memcpy(temp + 192, a_buf + n, r);                                        \
    if (width & 1) {                                                         \
      temp[64 + SS(r, UVSHIFT)] = temp[64 + SS(r, UVSHIFT) - 1];             \
      temp[128 + SS(r, UVSHIFT)] = temp[128 + SS(r, UVSHIFT) - 1];           \
    }                                                                        \
    ANY_SIMD(temp, temp + 64, temp + 128, temp + 192, temp + 256,            \
             yuvconstants, MASK + 1);                                        \
    memcpy(dst_ptr + (n >> DUVSHIFT) * BPP, temp + 256,                      \
           SS(r, DUVSHIFT) * BPP);                                           \
  }

#ifdef HAS_I422ALPHATOARGBROW_SSSE3
ANY41C(I422AlphaToARGBRow_Any_SSSE3, I422AlphaToARGBRow_SSSE3, 1, 0, 4, 7)
#endif
#ifdef HAS_I422ALPHATOARGBROW_AVX2
ANY41C(I422AlphaToARGBRow_Any_AVX2, I422AlphaToARGBRow_AVX2, 1, 0, 4, 15)
#endif
#ifdef HAS_I422ALPHATOARGBROW_NEON
ANY41C(I422AlphaToARGBRow_Any_NEON, I422AlphaToARGBRow_NEON, 1, 0, 4, 7)
#endif
#ifdef HAS_I422ALPHATOARGBROW_MSA
ANY41C(I422AlphaToARGBRow_Any_MSA, I422AlphaToARGBRow_MSA, 1, 0, 4, 7)
#endif
#ifdef HAS_I422ALPHATOARGBROW_MMI
ANY41C(I422AlphaToARGBRow_Any_MMI, I422AlphaToARGBRow_MMI, 1, 0, 4, 7)
#endif
#undef ANY41C

// Any 3 planes to 1.
#define ANY31(NAMEANY, ANY_SIMD, UVSHIFT, DUVSHIFT, BPP, MASK)      \
  void NAMEANY(const uint8_t* y_buf, const uint8_t* u_buf,          \
               const uint8_t* v_buf, uint8_t* dst_ptr, int width) { \
    SIMD_ALIGNED(uint8_t temp[64 * 4]);                             \
    memset(temp, 0, 64 * 3); /* for YUY2 and msan */                \
    int r = width & MASK;                                           \
    int n = width & ~MASK;                                          \
    if (n > 0) {                                                    \
      ANY_SIMD(y_buf, u_buf, v_buf, dst_ptr, n);                    \
    }                                                               \
    memcpy(temp, y_buf + n, r);                                     \
    memcpy(temp + 64, u_buf + (n >> UVSHIFT), SS(r, UVSHIFT));      \
    memcpy(temp + 128, v_buf + (n >> UVSHIFT), SS(r, UVSHIFT));     \
    ANY_SIMD(temp, temp + 64, temp + 128, temp + 192, MASK + 1);    \
    memcpy(dst_ptr + (n >> DUVSHIFT) * BPP, temp + 192,             \
           SS(r, DUVSHIFT) * BPP);                                  \
  }

#ifdef HAS_I422TOYUY2ROW_SSE2
// ANY31(I422ToYUY2Row_Any_SSE2, I422ToYUY2Row_SSE2, 1, 1, 4, 15)
ANY31(I422ToUYVYRow_Any_SSE2, I422ToUYVYRow_SSE2, 1, 1, 4, 15)
#endif
#ifdef HAS_I422TOYUY2ROW_AVX2
// ANY31(I422ToYUY2Row_Any_AVX2, I422ToYUY2Row_AVX2, 1, 1, 4, 31)
ANY31(I422ToUYVYRow_Any_AVX2, I422ToUYVYRow_AVX2, 1, 1, 4, 31)
#endif
#ifdef HAS_I422TOUYVYROW_NEON
ANY31(I422ToUYVYRow_Any_NEON, I422ToUYVYRow_NEON, 1, 1, 4, 15)
#endif
#ifdef HAS_I422TOUYVYROW_MSA
ANY31(I422ToUYVYRow_Any_MSA, I422ToUYVYRow_MSA, 1, 1, 4, 31)
#endif
#ifdef HAS_I422TOUYVYROW_MMI
ANY31(I422ToUYVYRow_Any_MMI, I422ToUYVYRow_MMI, 1, 1, 4, 7)
#endif
#undef ANY31

// Note that odd width replication includes 444 due to implementation
// on arm that subsamples 444 to 422 internally.
// Any 3 planes to 1 with yuvconstants
#define ANY31C(NAMEANY, ANY_SIMD, UVSHIFT, DUVSHIFT, BPP, MASK)      \
  void NAMEANY(const uint8_t* y_buf, const uint8_t* u_buf,           \
               const uint8_t* v_buf, uint8_t* dst_ptr,               \
               const struct YuvConstants* yuvconstants, int width) { \
    SIMD_ALIGNED(uint8_t temp[128 * 4]);                             \
    memset(temp, 0, 128 * 3); /* for YUY2 and msan */                \
    int r = width & MASK;                                            \
    int n = width & ~MASK;                                           \
    if (n > 0) {                                                     \
      ANY_SIMD(y_buf, u_buf, v_buf, dst_ptr, yuvconstants, n);       \
    }                                                                \
    memcpy(temp, y_buf + n, r);                                      \
    memcpy(temp + 128, u_buf + (n >> UVSHIFT), SS(r, UVSHIFT));      \
    memcpy(temp + 256, v_buf + (n >> UVSHIFT), SS(r, UVSHIFT));      \
    if (width & 1) {                                                 \
      temp[128 + SS(r, UVSHIFT)] = temp[128 + SS(r, UVSHIFT) - 1];   \
      temp[256 + SS(r, UVSHIFT)] = temp[256 + SS(r, UVSHIFT) - 1];   \
    }                                                                \
    ANY_SIMD(temp, temp + 128, temp + 256, temp + 384, yuvconstants, \
             MASK + 1);                                              \
    memcpy(dst_ptr + (n >> DUVSHIFT) * BPP, temp + 384,              \
           SS(r, DUVSHIFT) * BPP);                                   \
  }

#ifdef HAS_I422TOARGBROW_SSSE3
ANY31C(I422ToARGBRow_Any_SSSE3, I422ToARGBRow_SSSE3, 1, 0, 4, 7)
#endif
#ifdef HAS_I422TORGBAROW_SSSE3
ANY31C(I422ToRGBARow_Any_SSSE3, I422ToRGBARow_SSSE3, 1, 0, 4, 7)
#endif
#ifdef HAS_I422TOARGBROW_AVX2
ANY31C(I422ToARGBRow_Any_AVX2, I422ToARGBRow_AVX2, 1, 0, 4, 15)
#endif
#ifdef HAS_I422TORGBAROW_AVX2
ANY31C(I422ToRGBARow_Any_AVX2, I422ToRGBARow_AVX2, 1, 0, 4, 15)
#endif
#ifdef HAS_I422TOARGBROW_NEON
// ANY31C(I444ToARGBRow_Any_NEON, I444ToARGBRow_NEON, 0, 0, 4, 7)
ANY31C(I422ToARGBRow_Any_NEON, I422ToARGBRow_NEON, 1, 0, 4, 7)
ANY31C(I422ToRGBARow_Any_NEON, I422ToRGBARow_NEON, 1, 0, 4, 7)
// ANY31C(I422ToRGB24Row_Any_NEON, I422ToRGB24Row_NEON, 1, 0, 3, 7)
// ANY31C(I422ToARGB4444Row_Any_NEON, I422ToARGB4444Row_NEON, 1, 0, 2, 7)
// ANY31C(I422ToARGB1555Row_Any_NEON, I422ToARGB1555Row_NEON, 1, 0, 2, 7)
// ANY31C(I422ToRGB565Row_Any_NEON, I422ToRGB565Row_NEON, 1, 0, 2, 7)
#endif
#ifdef HAS_I422TOARGBROW_MSA
// ANY31C(I444ToARGBRow_Any_MSA, I444ToARGBRow_MSA, 0, 0, 4, 7)
ANY31C(I422ToARGBRow_Any_MSA, I422ToARGBRow_MSA, 1, 0, 4, 7)
ANY31C(I422ToRGBARow_Any_MSA, I422ToRGBARow_MSA, 1, 0, 4, 7)
// ANY31C(I422ToRGB24Row_Any_MSA, I422ToRGB24Row_MSA, 1, 0, 3, 15)
// ANY31C(I422ToARGB4444Row_Any_MSA, I422ToARGB4444Row_MSA, 1, 0, 2, 7)
// ANY31C(I422ToARGB1555Row_Any_MSA, I422ToARGB1555Row_MSA, 1, 0, 2, 7)
// ANY31C(I422ToRGB565Row_Any_MSA, I422ToRGB565Row_MSA, 1, 0, 2, 7)
#endif
#ifdef HAS_I422TOARGBROW_MMI
// ANY31C(I444ToARGBRow_Any_MMI, I444ToARGBRow_MMI, 0, 0, 4, 7)
ANY31C(I422ToARGBRow_Any_MMI, I422ToARGBRow_MMI, 1, 0, 4, 7)
// ANY31C(I422ToRGB24Row_Any_MMI, I422ToRGB24Row_MMI, 1, 0, 3, 15)
// ANY31C(I422ToARGB4444Row_Any_MMI, I422ToARGB4444Row_MMI, 1, 0, 2, 7)
// ANY31C(I422ToARGB1555Row_Any_MMI, I422ToARGB1555Row_MMI, 1, 0, 2, 7)
// ANY31C(I422ToRGB565Row_Any_MMI, I422ToRGB565Row_MMI, 1, 0, 2, 7)
ANY31C(I422ToRGBARow_Any_MMI, I422ToRGBARow_MMI, 1, 0, 4, 7)
#endif
#undef ANY31C

// Any 1 to 1.
#define ANY11(NAMEANY, ANY_SIMD, UVSHIFT, SBPP, BPP, MASK)                \
  void NAMEANY(const uint8_t* src_ptr, uint8_t* dst_ptr, int width) {     \
    SIMD_ALIGNED(uint8_t temp[128 * 2]);                                  \
    memset(temp, 0, 128); /* for YUY2 and msan */                         \
    int r = width & MASK;                                                 \
    int n = width & ~MASK;                                                \
    if (n > 0) {                                                          \
      ANY_SIMD(src_ptr, dst_ptr, n);                                      \
    }                                                                     \
    memcpy(temp, src_ptr + (n >> UVSHIFT) * SBPP, SS(r, UVSHIFT) * SBPP); \
    ANY_SIMD(temp, temp + 128, MASK + 1);                                 \
    memcpy(dst_ptr + n * BPP, temp + 128, r * BPP);                       \
  }

#ifdef HAS_COPYROW_AVX
ANY11(CopyRow_Any_AVX, CopyRow_AVX, 0, 1, 1, 63)
#endif
#ifdef HAS_COPYROW_SSE2
ANY11(CopyRow_Any_SSE2, CopyRow_SSE2, 0, 1, 1, 31)
#endif
#ifdef HAS_COPYROW_NEON
ANY11(CopyRow_Any_NEON, CopyRow_NEON, 0, 1, 1, 31)
#endif
#if defined(HAS_ARGBTORGB24ROW_SSSE3)
ANY11(ARGBToRGB24Row_Any_SSSE3, ARGBToRGB24Row_SSSE3, 0, 4, 3, 15)
ANY11(ARGBToRAWRow_Any_SSSE3, ARGBToRAWRow_SSSE3, 0, 4, 3, 15)
// ANY11(ARGBToRGB565Row_Any_SSE2, ARGBToRGB565Row_SSE2, 0, 4, 2, 3)
// ANY11(ARGBToARGB1555Row_Any_SSE2, ARGBToARGB1555Row_SSE2, 0, 4, 2, 3)
// ANY11(ARGBToARGB4444Row_Any_SSE2, ARGBToARGB4444Row_SSE2, 0, 4, 2, 3)
#endif
#if defined(HAS_ARGBTORGB24ROW_AVX2)
ANY11(ARGBToRGB24Row_Any_AVX2, ARGBToRGB24Row_AVX2, 0, 4, 3, 31)
#endif
#if defined(HAS_ARGBTORGB24ROW_AVX512VBMI)
ANY11(ARGBToRGB24Row_Any_AVX512VBMI, ARGBToRGB24Row_AVX512VBMI, 0, 4, 3, 31)
#endif
#if defined(HAS_ARGBTORAWROW_AVX2)
ANY11(ARGBToRAWRow_Any_AVX2, ARGBToRAWRow_AVX2, 0, 4, 3, 31)
#endif
#if defined(HAS_RGB24TOARGBROW_SSSE3)
ANY11(RGB24ToARGBRow_Any_SSSE3, RGB24ToARGBRow_SSSE3, 0, 3, 4, 15)
ANY11(RAWToARGBRow_Any_SSSE3, RAWToARGBRow_SSSE3, 0, 3, 4, 15)
// ANY11(RGB565ToARGBRow_Any_SSE2, RGB565ToARGBRow_SSE2, 0, 2, 4, 7)
// ANY11(ARGB1555ToARGBRow_Any_SSE2, ARGB1555ToARGBRow_SSE2, 0, 2, 4, 7)
// ANY11(ARGB4444ToARGBRow_Any_SSE2, ARGB4444ToARGBRow_SSE2, 0, 2, 4, 7)
#endif
#if defined(HAS_RAWTORGBAROW_SSSE3)
ANY11(RAWToRGBARow_Any_SSSE3, RAWToRGBARow_SSSE3, 0, 3, 4, 15)
#endif
#if defined(HAS_ARGBTORGB24ROW_NEON)
ANY11(ARGBToRGB24Row_Any_NEON, ARGBToRGB24Row_NEON, 0, 4, 3, 7)
ANY11(ARGBToRAWRow_Any_NEON, ARGBToRAWRow_NEON, 0, 4, 3, 7)
// ANY11(ARGBToRGB565Row_Any_NEON, ARGBToRGB565Row_NEON, 0, 4, 2, 7)
// ANY11(ARGBToARGB1555Row_Any_NEON, ARGBToARGB1555Row_NEON, 0, 4, 2, 7)
// ANY11(ARGBToARGB4444Row_Any_NEON, ARGBToARGB4444Row_NEON, 0, 4, 2, 7)
// ANY11(J400ToARGBRow_Any_NEON, J400ToARGBRow_NEON, 0, 1, 4, 7)
#endif
#if defined(HAS_ARGBTORGB24ROW_MSA)
ANY11(ARGBToRGB24Row_Any_MSA, ARGBToRGB24Row_MSA, 0, 4, 3, 15)
ANY11(ARGBToRAWRow_Any_MSA, ARGBToRAWRow_MSA, 0, 4, 3, 15)
// ANY11(ARGBToRGB565Row_Any_MSA, ARGBToRGB565Row_MSA, 0, 4, 2, 7)
// ANY11(ARGBToARGB1555Row_Any_MSA, ARGBToARGB1555Row_MSA, 0, 4, 2, 7)
// ANY11(ARGBToARGB4444Row_Any_MSA, ARGBToARGB4444Row_MSA, 0, 4, 2, 7)
// ANY11(J400ToARGBRow_Any_MSA, J400ToARGBRow_MSA, 0, 1, 4, 15)
#endif
#if defined(HAS_ARGBTORGB24ROW_MMI)
ANY11(ARGBToRGB24Row_Any_MMI, ARGBToRGB24Row_MMI, 0, 4, 3, 3)
ANY11(ARGBToRAWRow_Any_MMI, ARGBToRAWRow_MMI, 0, 4, 3, 3)
// ANY11(ARGBToRGB565Row_Any_MMI, ARGBToRGB565Row_MMI, 0, 4, 2, 3)
// ANY11(ARGBToARGB1555Row_Any_MMI, ARGBToARGB1555Row_MMI, 0, 4, 2, 3)
// ANY11(ARGBToARGB4444Row_Any_MMI, ARGBToARGB4444Row_MMI, 0, 4, 2, 3)
// ANY11(J400ToARGBRow_Any_MMI, J400ToARGBRow_MMI, 0, 1, 4, 3)
#endif
#ifdef HAS_ARGBTOYROW_AVX2
ANY11(ARGBToYRow_Any_AVX2, ARGBToYRow_AVX2, 0, 4, 1, 31)
#endif
#ifdef HAS_ABGRTOYROW_AVX2
ANY11(ABGRToYRow_Any_AVX2, ABGRToYRow_AVX2, 0, 4, 1, 31)
#endif
#ifdef HAS_UYVYTOYROW_AVX2
ANY11(UYVYToYRow_Any_AVX2, UYVYToYRow_AVX2, 0, 2, 1, 31)
#endif
#ifdef HAS_ARGBTOYROW_SSSE3
ANY11(ARGBToYRow_Any_SSSE3, ARGBToYRow_SSSE3, 0, 4, 1, 15)
#endif
#ifdef HAS_BGRATOYROW_SSSE3
ANY11(BGRAToYRow_Any_SSSE3, BGRAToYRow_SSSE3, 0, 4, 1, 15)
ANY11(ABGRToYRow_Any_SSSE3, ABGRToYRow_SSSE3, 0, 4, 1, 15)
ANY11(RGBAToYRow_Any_SSSE3, RGBAToYRow_SSSE3, 0, 4, 1, 15)
// ANY11(YUY2ToYRow_Any_SSE2, YUY2ToYRow_SSE2, 1, 4, 1, 15)
ANY11(UYVYToYRow_Any_SSE2, UYVYToYRow_SSE2, 1, 4, 1, 15)
#endif
#ifdef HAS_BGRATOYROW_NEON
ANY11(BGRAToYRow_Any_NEON, BGRAToYRow_NEON, 0, 4, 1, 7)
#endif
#ifdef HAS_BGRATOYROW_MSA
ANY11(BGRAToYRow_Any_MSA, BGRAToYRow_MSA, 0, 4, 1, 15)
#endif
#ifdef HAS_BGRATOYROW_MMI
ANY11(BGRAToYRow_Any_MMI, BGRAToYRow_MMI, 0, 4, 1, 7)
#endif
#ifdef HAS_ABGRTOYROW_NEON
ANY11(ABGRToYRow_Any_NEON, ABGRToYRow_NEON, 0, 4, 1, 7)
#endif
#ifdef HAS_ABGRTOYROW_MSA
ANY11(ABGRToYRow_Any_MSA, ABGRToYRow_MSA, 0, 4, 1, 7)
#endif
#ifdef HAS_ABGRTOYROW_MMI
ANY11(ABGRToYRow_Any_MMI, ABGRToYRow_MMI, 0, 4, 1, 7)
#endif
#ifdef HAS_RGBATOYROW_NEON
ANY11(RGBAToYRow_Any_NEON, RGBAToYRow_NEON, 0, 4, 1, 7)
#endif
#ifdef HAS_RGBATOYROW_MSA
ANY11(RGBAToYRow_Any_MSA, RGBAToYRow_MSA, 0, 4, 1, 15)
#endif
#ifdef HAS_RGBATOYROW_MMI
ANY11(RGBAToYRow_Any_MMI, RGBAToYRow_MMI, 0, 4, 1, 7)
#endif
#ifdef HAS_RGB24TOYROW_NEON
ANY11(RGB24ToYRow_Any_NEON, RGB24ToYRow_NEON, 0, 3, 1, 7)
#endif
#ifdef HAS_RGB24TOYROW_MSA
ANY11(RGB24ToYRow_Any_MSA, RGB24ToYRow_MSA, 0, 3, 1, 15)
#endif
#ifdef HAS_RGB24TOYROW_MMI
ANY11(RGB24ToYRow_Any_MMI, RGB24ToYRow_MMI, 0, 3, 1, 7)
#endif
#ifdef HAS_RAWTOYROW_NEON
ANY11(RAWToYRow_Any_NEON, RAWToYRow_NEON, 0, 3, 1, 7)
#endif
#ifdef HAS_RAWTOYROW_MSA
ANY11(RAWToYRow_Any_MSA, RAWToYRow_MSA, 0, 3, 1, 15)
#endif
#ifdef HAS_RAWTOYROW_MMI
ANY11(RAWToYRow_Any_MMI, RAWToYRow_MMI, 0, 3, 1, 7)
#endif
#ifdef HAS_UYVYTOYROW_NEON
ANY11(UYVYToYRow_Any_NEON, UYVYToYRow_NEON, 1, 4, 1, 15)
#endif
#ifdef HAS_UYVYTOYROW_MSA
ANY11(UYVYToYRow_Any_MSA, UYVYToYRow_MSA, 1, 4, 1, 31)
#endif
#ifdef HAS_UYVYTOYROW_MMI
ANY11(UYVYToYRow_Any_MMI, UYVYToYRow_MMI, 1, 4, 1, 15)
#endif
#ifdef HAS_AYUVTOYROW_NEON
ANY11(AYUVToYRow_Any_NEON, AYUVToYRow_NEON, 0, 4, 1, 15)
#endif
#ifdef HAS_RGB24TOARGBROW_NEON
ANY11(RGB24ToARGBRow_Any_NEON, RGB24ToARGBRow_NEON, 0, 3, 4, 7)
#endif
#ifdef HAS_RGB24TOARGBROW_MSA
ANY11(RGB24ToARGBRow_Any_MSA, RGB24ToARGBRow_MSA, 0, 3, 4, 15)
#endif
#ifdef HAS_RGB24TOARGBROW_MMI
ANY11(RGB24ToARGBRow_Any_MMI, RGB24ToARGBRow_MMI, 0, 3, 4, 3)
#endif
#ifdef HAS_RAWTOARGBROW_NEON
ANY11(RAWToARGBRow_Any_NEON, RAWToARGBRow_NEON, 0, 3, 4, 7)
#endif
#ifdef HAS_RAWTORGBAROW_NEON
ANY11(RAWToRGBARow_Any_NEON, RAWToRGBARow_NEON, 0, 3, 4, 7)
#endif
#ifdef HAS_RAWTOARGBROW_MSA
ANY11(RAWToARGBRow_Any_MSA, RAWToARGBRow_MSA, 0, 3, 4, 15)
#endif
#ifdef HAS_RAWTOARGBROW_MMI
ANY11(RAWToARGBRow_Any_MMI, RAWToARGBRow_MMI, 0, 3, 4, 3)
#endif
#ifdef HAS_ARGBATTENUATEROW_SSSE3
ANY11(ARGBAttenuateRow_Any_SSSE3, ARGBAttenuateRow_SSSE3, 0, 4, 4, 3)
#endif
#ifdef HAS_ARGBUNATTENUATEROW_SSE2
ANY11(ARGBUnattenuateRow_Any_SSE2, ARGBUnattenuateRow_SSE2, 0, 4, 4, 3)
#endif
#ifdef HAS_ARGBATTENUATEROW_AVX2
ANY11(ARGBAttenuateRow_Any_AVX2, ARGBAttenuateRow_AVX2, 0, 4, 4, 7)
#endif
#ifdef HAS_ARGBUNATTENUATEROW_AVX2
ANY11(ARGBUnattenuateRow_Any_AVX2, ARGBUnattenuateRow_AVX2, 0, 4, 4, 7)
#endif
#ifdef HAS_ARGBATTENUATEROW_NEON
ANY11(ARGBAttenuateRow_Any_NEON, ARGBAttenuateRow_NEON, 0, 4, 4, 7)
#endif
#ifdef HAS_ARGBATTENUATEROW_MSA
ANY11(ARGBAttenuateRow_Any_MSA, ARGBAttenuateRow_MSA, 0, 4, 4, 7)
#endif
#ifdef HAS_ARGBATTENUATEROW_MMI
ANY11(ARGBAttenuateRow_Any_MMI, ARGBAttenuateRow_MMI, 0, 4, 4, 1)
#endif
#undef ANY11

// Any 1 to 1 with parameter.
#define ANY11P(NAMEANY, ANY_SIMD, T, SBPP, BPP, MASK)                          \
  void NAMEANY(const uint8_t* src_ptr, uint8_t* dst_ptr, T param, int width) { \
    SIMD_ALIGNED(uint8_t temp[64 * 2]);                                        \
    memset(temp, 0, 64); /* for msan */                                        \
    int r = width & MASK;                                                      \
    int n = width & ~MASK;                                                     \
    if (n > 0) {                                                               \
      ANY_SIMD(src_ptr, dst_ptr, param, n);                                    \
    }                                                                          \
    memcpy(temp, src_ptr + n * SBPP, r * SBPP);                                \
    ANY_SIMD(temp, temp + 64, param, MASK + 1);                                \
    memcpy(dst_ptr + n * BPP, temp + 64, r * BPP);                             \
  }

#ifdef HAS_ARGBSHUFFLEROW_SSSE3
ANY11P(ARGBShuffleRow_Any_SSSE3, ARGBShuffleRow_SSSE3, const uint8_t*, 4, 4, 7)
#endif
#ifdef HAS_ARGBSHUFFLEROW_AVX2
ANY11P(ARGBShuffleRow_Any_AVX2, ARGBShuffleRow_AVX2, const uint8_t*, 4, 4, 15)
#endif
#ifdef HAS_ARGBSHUFFLEROW_NEON
ANY11P(ARGBShuffleRow_Any_NEON, ARGBShuffleRow_NEON, const uint8_t*, 4, 4, 3)
#endif
#ifdef HAS_ARGBSHUFFLEROW_MSA
ANY11P(ARGBShuffleRow_Any_MSA, ARGBShuffleRow_MSA, const uint8_t*, 4, 4, 7)
#endif
#ifdef HAS_ARGBSHUFFLEROW_MMI
ANY11P(ARGBShuffleRow_Any_MMI, ARGBShuffleRow_MMI, const uint8_t*, 4, 4, 1)
#endif
#undef ANY11P
#undef ANY11P

// Any 1 to 1 with yuvconstants
#define ANY11C(NAMEANY, ANY_SIMD, UVSHIFT, SBPP, BPP, MASK)               \
  void NAMEANY(const uint8_t* src_ptr, uint8_t* dst_ptr,                  \
               const struct YuvConstants* yuvconstants, int width) {      \
    SIMD_ALIGNED(uint8_t temp[128 * 2]);                                  \
    memset(temp, 0, 128); /* for YUY2 and msan */                         \
    int r = width & MASK;                                                 \
    int n = width & ~MASK;                                                \
    if (n > 0) {                                                          \
      ANY_SIMD(src_ptr, dst_ptr, yuvconstants, n);                        \
    }                                                                     \
    memcpy(temp, src_ptr + (n >> UVSHIFT) * SBPP, SS(r, UVSHIFT) * SBPP); \
    ANY_SIMD(temp, temp + 128, yuvconstants, MASK + 1);                   \
    memcpy(dst_ptr + n * BPP, temp + 128, r * BPP);                       \
  }
#if defined(HAS_YUY2TOARGBROW_SSSE3)
// ANY11C(YUY2ToARGBRow_Any_SSSE3, YUY2ToARGBRow_SSSE3, 1, 4, 4, 15)
ANY11C(UYVYToARGBRow_Any_SSSE3, UYVYToARGBRow_SSSE3, 1, 4, 4, 15)
#endif
#if defined(HAS_YUY2TOARGBROW_AVX2)
// ANY11C(YUY2ToARGBRow_Any_AVX2, YUY2ToARGBRow_AVX2, 1, 4, 4, 31)
ANY11C(UYVYToARGBRow_Any_AVX2, UYVYToARGBRow_AVX2, 1, 4, 4, 31)
#endif
#if defined(HAS_YUY2TOARGBROW_NEON)
// ANY11C(YUY2ToARGBRow_Any_NEON, YUY2ToARGBRow_NEON, 1, 4, 4, 7)
ANY11C(UYVYToARGBRow_Any_NEON, UYVYToARGBRow_NEON, 1, 4, 4, 7)
#endif
#if defined(HAS_YUY2TOARGBROW_MSA)
// ANY11C(YUY2ToARGBRow_Any_MSA, YUY2ToARGBRow_MSA, 1, 4, 4, 7)
ANY11C(UYVYToARGBRow_Any_MSA, UYVYToARGBRow_MSA, 1, 4, 4, 7)
#endif
#if defined(HAS_YUY2TOARGBROW_MMI)
// ANY11C(YUY2ToARGBRow_Any_MMI, YUY2ToARGBRow_MMI, 1, 4, 4, 7)
ANY11C(UYVYToARGBRow_Any_MMI, UYVYToARGBRow_MMI, 1, 4, 4, 7)
#endif
#undef ANY11C

// Any 1 to 1 interpolate.  Takes 2 rows of source via stride.
#define ANY11I(NAMEANY, ANY_SIMD, SBPP, BPP, MASK)                             \
  void NAMEANY(uint8_t* dst_ptr, const uint8_t* src_ptr, ptrdiff_t src_stride, \
               int width, int source_y_fraction) {                             \
    SIMD_ALIGNED(uint8_t temp[64 * 3]);                                        \
    memset(temp, 0, 64 * 2); /* for msan */                                    \
    int r = width & MASK;                                                      \
    int n = width & ~MASK;                                                     \
    if (n > 0) {                                                               \
      ANY_SIMD(dst_ptr, src_ptr, src_stride, n, source_y_fraction);            \
    }                                                                          \
    memcpy(temp, src_ptr + n * SBPP, r * SBPP);                                \
    memcpy(temp + 64, src_ptr + src_stride + n * SBPP, r * SBPP);              \
    ANY_SIMD(temp + 128, temp, 64, MASK + 1, source_y_fraction);               \
    memcpy(dst_ptr + n * BPP, temp + 128, r * BPP);                            \
  }

#ifdef HAS_INTERPOLATEROW_AVX2
ANY11I(InterpolateRow_Any_AVX2, InterpolateRow_AVX2, 1, 1, 31)
#endif
#ifdef HAS_INTERPOLATEROW_SSSE3
ANY11I(InterpolateRow_Any_SSSE3, InterpolateRow_SSSE3, 1, 1, 15)
#endif
#ifdef HAS_INTERPOLATEROW_NEON
ANY11I(InterpolateRow_Any_NEON, InterpolateRow_NEON, 1, 1, 15)
#endif
#ifdef HAS_INTERPOLATEROW_MSA
ANY11I(InterpolateRow_Any_MSA, InterpolateRow_MSA, 1, 1, 31)
#endif
#ifdef HAS_INTERPOLATEROW_MMI
ANY11I(InterpolateRow_Any_MMI, InterpolateRow_MMI, 1, 1, 7)
#endif
#undef ANY11I

// Any 1 to 2.  Outputs UV planes.
#define ANY12(NAMEANY, ANY_SIMD, UVSHIFT, BPP, DUVSHIFT, MASK)          \
  void NAMEANY(const uint8_t* src_ptr, uint8_t* dst_u, uint8_t* dst_v,  \
               int width) {                                             \
    SIMD_ALIGNED(uint8_t temp[128 * 3]);                                \
    memset(temp, 0, 128); /* for msan */                                \
    int r = width & MASK;                                               \
    int n = width & ~MASK;                                              \
    if (n > 0) {                                                        \
      ANY_SIMD(src_ptr, dst_u, dst_v, n);                               \
    }                                                                   \
    memcpy(temp, src_ptr + (n >> UVSHIFT) * BPP, SS(r, UVSHIFT) * BPP); \
    ANY_SIMD(temp, temp + 128, temp + 256, MASK + 1);                   \
    memcpy(dst_u + (n >> DUVSHIFT), temp + 128, SS(r, DUVSHIFT));       \
    memcpy(dst_v + (n >> DUVSHIFT), temp + 256, SS(r, DUVSHIFT));       \
  }

#ifdef HAS_YUY2TOUV422ROW_AVX2
// ANY12(YUY2ToUV422Row_Any_AVX2, YUY2ToUV422Row_AVX2, 1, 4, 1, 31)
ANY12(UYVYToUV422Row_Any_AVX2, UYVYToUV422Row_AVX2, 1, 4, 1, 31)
#endif
#ifdef HAS_YUY2TOUV422ROW_SSE2
// ANY12(YUY2ToUV422Row_Any_SSE2, YUY2ToUV422Row_SSE2, 1, 4, 1, 15)
ANY12(UYVYToUV422Row_Any_SSE2, UYVYToUV422Row_SSE2, 1, 4, 1, 15)
#endif
#ifdef HAS_YUY2TOUV422ROW_NEON
// ANY12(ARGBToUV444Row_Any_NEON, ARGBToUV444Row_NEON, 0, 4, 0, 7)
// ANY12(YUY2ToUV422Row_Any_NEON, YUY2ToUV422Row_NEON, 1, 4, 1, 15)
ANY12(UYVYToUV422Row_Any_NEON, UYVYToUV422Row_NEON, 1, 4, 1, 15)
#endif
#ifdef HAS_YUY2TOUV422ROW_MSA
// ANY12(ARGBToUV444Row_Any_MSA, ARGBToUV444Row_MSA, 0, 4, 0, 15)
// ANY12(YUY2ToUV422Row_Any_MSA, YUY2ToUV422Row_MSA, 1, 4, 1, 31)
ANY12(UYVYToUV422Row_Any_MSA, UYVYToUV422Row_MSA, 1, 4, 1, 31)
#endif
#ifdef HAS_YUY2TOUV422ROW_MMI
// ANY12(ARGBToUV444Row_Any_MMI, ARGBToUV444Row_MMI, 0, 4, 0, 7)
ANY12(UYVYToUV422Row_Any_MMI, UYVYToUV422Row_MMI, 1, 4, 1, 15)
// ANY12(YUY2ToUV422Row_Any_MMI, YUY2ToUV422Row_MMI, 1, 4, 1, 15)
#endif
#undef ANY12

// Any 1 to 2 with source stride (2 rows of source).  Outputs UV planes.
// 128 byte row allows for 32 avx ARGB pixels.
#define ANY12S(NAMEANY, ANY_SIMD, UVSHIFT, BPP, MASK)                        \
  void NAMEANY(const uint8_t* src_ptr, int src_stride, uint8_t* dst_u,       \
               uint8_t* dst_v, int width) {                                  \
    SIMD_ALIGNED(uint8_t temp[128 * 4]);                                     \
    memset(temp, 0, 128 * 2); /* for msan */                                 \
    int r = width & MASK;                                                    \
    int n = width & ~MASK;                                                   \
    if (n > 0) {                                                             \
      ANY_SIMD(src_ptr, src_stride, dst_u, dst_v, n);                        \
    }                                                                        \
    memcpy(temp, src_ptr + (n >> UVSHIFT) * BPP, SS(r, UVSHIFT) * BPP);      \
    memcpy(temp + 128, src_ptr + src_stride + (n >> UVSHIFT) * BPP,          \
           SS(r, UVSHIFT) * BPP);                                            \
    if ((width & 1) && UVSHIFT == 0) { /* repeat last pixel for subsample */ \
      memcpy(temp + SS(r, UVSHIFT) * BPP, temp + SS(r, UVSHIFT) * BPP - BPP, \
             BPP);                                                           \
      memcpy(temp + 128 + SS(r, UVSHIFT) * BPP,                              \
             temp + 128 + SS(r, UVSHIFT) * BPP - BPP, BPP);                  \
    }                                                                        \
    ANY_SIMD(temp, 128, temp + 256, temp + 384, MASK + 1);                   \
    memcpy(dst_u + (n >> 1), temp + 256, SS(r, 1));                          \
    memcpy(dst_v + (n >> 1), temp + 384, SS(r, 1));                          \
  }

#ifdef HAS_ARGBTOUVROW_AVX2
ANY12S(ARGBToUVRow_Any_AVX2, ARGBToUVRow_AVX2, 0, 4, 31)
#endif
#ifdef HAS_ARGBTOUVROW_SSSE3
ANY12S(ARGBToUVRow_Any_SSSE3, ARGBToUVRow_SSSE3, 0, 4, 15)
// ANY12S(ARGBToUVJRow_Any_SSSE3, ARGBToUVJRow_SSSE3, 0, 4, 15)
ANY12S(BGRAToUVRow_Any_SSSE3, BGRAToUVRow_SSSE3, 0, 4, 15)
ANY12S(ABGRToUVRow_Any_SSSE3, ABGRToUVRow_SSSE3, 0, 4, 15)
ANY12S(RGBAToUVRow_Any_SSSE3, RGBAToUVRow_SSSE3, 0, 4, 15)
#endif
#ifdef HAS_ARGBTOUVROW_NEON
ANY12S(ARGBToUVRow_Any_NEON, ARGBToUVRow_NEON, 0, 4, 15)
#endif
#ifdef HAS_ARGBTOUVROW_MSA
ANY12S(ARGBToUVRow_Any_MSA, ARGBToUVRow_MSA, 0, 4, 31)
#endif
#ifdef HAS_ARGBTOUVROW_MMI
ANY12S(ARGBToUVRow_Any_MMI, ARGBToUVRow_MMI, 0, 4, 15)
#endif
#ifdef HAS_BGRATOUVROW_NEON
ANY12S(BGRAToUVRow_Any_NEON, BGRAToUVRow_NEON, 0, 4, 15)
#endif
#ifdef HAS_BGRATOUVROW_MSA
ANY12S(BGRAToUVRow_Any_MSA, BGRAToUVRow_MSA, 0, 4, 15)
#endif
#ifdef HAS_BGRATOUVROW_MMI
ANY12S(BGRAToUVRow_Any_MMI, BGRAToUVRow_MMI, 0, 4, 15)
#endif
#ifdef HAS_ABGRTOUVROW_NEON
ANY12S(ABGRToUVRow_Any_NEON, ABGRToUVRow_NEON, 0, 4, 15)
#endif
#ifdef HAS_ABGRTOUVROW_MSA
ANY12S(ABGRToUVRow_Any_MSA, ABGRToUVRow_MSA, 0, 4, 15)
#endif
#ifdef HAS_ABGRTOUVROW_MMI
ANY12S(ABGRToUVRow_Any_MMI, ABGRToUVRow_MMI, 0, 4, 15)
#endif
#ifdef HAS_RGBATOUVROW_NEON
ANY12S(RGBAToUVRow_Any_NEON, RGBAToUVRow_NEON, 0, 4, 15)
#endif
#ifdef HAS_RGBATOUVROW_MSA
ANY12S(RGBAToUVRow_Any_MSA, RGBAToUVRow_MSA, 0, 4, 15)
#endif
#ifdef HAS_RGBATOUVROW_MMI
ANY12S(RGBAToUVRow_Any_MMI, RGBAToUVRow_MMI, 0, 4, 15)
#endif
#ifdef HAS_RGB24TOUVROW_NEON
ANY12S(RGB24ToUVRow_Any_NEON, RGB24ToUVRow_NEON, 0, 3, 15)
#endif
#ifdef HAS_RGB24TOUVROW_MSA
ANY12S(RGB24ToUVRow_Any_MSA, RGB24ToUVRow_MSA, 0, 3, 15)
#endif
#ifdef HAS_RGB24TOUVROW_MMI
ANY12S(RGB24ToUVRow_Any_MMI, RGB24ToUVRow_MMI, 0, 3, 15)
#endif
#ifdef HAS_RAWTOUVROW_NEON
ANY12S(RAWToUVRow_Any_NEON, RAWToUVRow_NEON, 0, 3, 15)
#endif
#ifdef HAS_RAWTOUVROW_MSA
ANY12S(RAWToUVRow_Any_MSA, RAWToUVRow_MSA, 0, 3, 15)
#endif
#ifdef HAS_RAWTOUVROW_MMI
ANY12S(RAWToUVRow_Any_MMI, RAWToUVRow_MMI, 0, 3, 15)
#endif
#undef ANY12S

// Any 1 to 1 with source stride (2 rows of source).  Outputs UV plane.
// 128 byte row allows for 32 avx ARGB pixels.
#define ANY11S(NAMEANY, ANY_SIMD, UVSHIFT, BPP, MASK)                        \
  void NAMEANY(const uint8_t* src_ptr, int src_stride, uint8_t* dst_vu,      \
               int width) {                                                  \
    SIMD_ALIGNED(uint8_t temp[128 * 3]);                                     \
    memset(temp, 0, 128 * 2); /* for msan */                                 \
    int r = width & MASK;                                                    \
    int n = width & ~MASK;                                                   \
    if (n > 0) {                                                             \
      ANY_SIMD(src_ptr, src_stride, dst_vu, n);                              \
    }                                                                        \
    memcpy(temp, src_ptr + (n >> UVSHIFT) * BPP, SS(r, UVSHIFT) * BPP);      \
    memcpy(temp + 128, src_ptr + src_stride + (n >> UVSHIFT) * BPP,          \
           SS(r, UVSHIFT) * BPP);                                            \
    if ((width & 1) && UVSHIFT == 0) { /* repeat last pixel for subsample */ \
      memcpy(temp + SS(r, UVSHIFT) * BPP, temp + SS(r, UVSHIFT) * BPP - BPP, \
             BPP);                                                           \
      memcpy(temp + 128 + SS(r, UVSHIFT) * BPP,                              \
             temp + 128 + SS(r, UVSHIFT) * BPP - BPP, BPP);                  \
    }                                                                        \
    ANY_SIMD(temp, 128, temp + 256, MASK + 1);                               \
    memcpy(dst_vu + (n >> 1) * 2, temp + 256, SS(r, 1) * 2);                 \
  }

#ifdef HAS_AYUVTOVUROW_NEON
ANY11S(AYUVToUVRow_Any_NEON, AYUVToUVRow_NEON, 0, 4, 15)
ANY11S(AYUVToVURow_Any_NEON, AYUVToVURow_NEON, 0, 4, 15)
#endif
#undef ANY11S

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif
