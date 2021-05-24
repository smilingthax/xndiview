/*
 *  Copyright 2011 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "libyuv/row.h"

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

// This module is for GCC x86 and x64.
#if !defined(LIBYUV_DISABLE_X86) && (defined(__x86_64__) || defined(__i386__))

#if defined(HAS_ARGBTOYROW_SSSE3) || defined(HAS_ARGBGRAYROW_SSSE3)

// Constants for ARGB
static const uvec8 kARGBToY = {25u, 129u, 66u, 0u, 25u, 129u, 66u, 0u,
                               25u, 129u, 66u, 0u, 25u, 129u, 66u, 0u};

#if 0 // thobi
// JPeg full range.
static const uvec8 kARGBToYJ = {29u, 150u, 77u, 0u, 29u, 150u, 77u, 0u,
                                29u, 150u, 77u, 0u, 29u, 150u, 77u, 0u};

static const uvec8 kRGBAToYJ = {0u, 29u, 150u, 77u, 0u, 29u, 150u, 77u,
                                0u, 29u, 150u, 77u, 0u, 29u, 150u, 77u};
#endif // thobi
#endif  // defined(HAS_ARGBTOYROW_SSSE3) || defined(HAS_ARGBGRAYROW_SSSE3)

#if defined(HAS_ARGBTOYROW_SSSE3) || defined(HAS_I422TOARGBROW_SSSE3)

static const vec8 kARGBToU = {112, -74, -38, 0, 112, -74, -38, 0,
                              112, -74, -38, 0, 112, -74, -38, 0};

#if 0 // thobi
static const vec8 kARGBToUJ = {127, -84, -43, 0, 127, -84, -43, 0,
                               127, -84, -43, 0, 127, -84, -43, 0};
#endif // thobi

static const vec8 kARGBToV = {-18, -94, 112, 0, -18, -94, 112, 0,
                              -18, -94, 112, 0, -18, -94, 112, 0};

#if 0 // thobi
static const vec8 kARGBToVJ = {-20, -107, 127, 0, -20, -107, 127, 0,
                               -20, -107, 127, 0, -20, -107, 127, 0};
#endif // thobi

// Constants for BGRA
static const uvec8 kBGRAToY = {0u, 66u, 129u, 25u, 0u, 66u, 129u, 25u,
                               0u, 66u, 129u, 25u, 0u, 66u, 129u, 25u};

static const vec8 kBGRAToU = {0, -38, -74, 112, 0, -38, -74, 112,
                              0, -38, -74, 112, 0, -38, -74, 112};

static const vec8 kBGRAToV = {0, 112, -94, -18, 0, 112, -94, -18,
                              0, 112, -94, -18, 0, 112, -94, -18};

// Constants for ABGR
static const uvec8 kABGRToY = {66u, 129u, 25u, 0u, 66u, 129u, 25u, 0u,
                               66u, 129u, 25u, 0u, 66u, 129u, 25u, 0u};

static const vec8 kABGRToU = {-38, -74, 112, 0, -38, -74, 112, 0,
                              -38, -74, 112, 0, -38, -74, 112, 0};

static const vec8 kABGRToV = {112, -94, -18, 0, 112, -94, -18, 0,
                              112, -94, -18, 0, 112, -94, -18, 0};

// Constants for RGBA.
static const uvec8 kRGBAToY = {0u, 25u, 129u, 66u, 0u, 25u, 129u, 66u,
                               0u, 25u, 129u, 66u, 0u, 25u, 129u, 66u};

static const vec8 kRGBAToU = {0, 112, -74, -38, 0, 112, -74, -38,
                              0, 112, -74, -38, 0, 112, -74, -38};

static const vec8 kRGBAToV = {0, -18, -94, 112, 0, -18, -94, 112,
                              0, -18, -94, 112, 0, -18, -94, 112};

static const uvec16 kAddY16 = {0x7e80u, 0x7e80u, 0x7e80u, 0x7e80u,
                               0x7e80u, 0x7e80u, 0x7e80u, 0x7e80u};

static const uvec8 kAddUV128 = {128u, 128u, 128u, 128u, 128u, 128u, 128u, 128u,
                                128u, 128u, 128u, 128u, 128u, 128u, 128u, 128u};

static const uvec16 kSub128 = {0x8080u, 0x8080u, 0x8080u, 0x8080u,
                               0x8080u, 0x8080u, 0x8080u, 0x8080u};

#endif  // defined(HAS_ARGBTOYROW_SSSE3) || defined(HAS_I422TOARGBROW_SSSE3)

#ifdef HAS_RGB24TOARGBROW_SSSE3

// Shuffle table for converting RGB24 to ARGB.
static const uvec8 kShuffleMaskRGB24ToARGB = {
    0u, 1u, 2u, 12u, 3u, 4u, 5u, 13u, 6u, 7u, 8u, 14u, 9u, 10u, 11u, 15u};

// Shuffle table for converting RAW to ARGB.
static const uvec8 kShuffleMaskRAWToARGB = {2u, 1u, 0u, 12u, 5u,  4u,  3u, 13u,
                                            8u, 7u, 6u, 14u, 11u, 10u, 9u, 15u};

// Shuffle table for converting RAW to RGBA.
static const uvec8 kShuffleMaskRAWToRGBA = {12u, 2u, 1u, 0u, 13u, 5u,  4u,  3u,
                                            14u, 8u, 7u, 6u, 15u, 11u, 10u, 9u};

static const uvec8 kShuffleMaskARGBToRGB24 = {
    0u, 1u, 2u, 4u, 5u, 6u, 8u, 9u, 10u, 12u, 13u, 14u, 128u, 128u, 128u, 128u};

// Shuffle table for converting ARGB to RAW.
static const uvec8 kShuffleMaskARGBToRAW = {
    2u, 1u, 0u, 6u, 5u, 4u, 10u, 9u, 8u, 14u, 13u, 12u, 128u, 128u, 128u, 128u};

// UYVY shuf 16 Y to 32 Y.
static const lvec8 kShuffleUYVYY = {1,  1,  3,  3,  5,  5,  7,  7,  9,  9, 11,
                                    11, 13, 13, 15, 15, 1,  1,  3,  3,  5, 5,
                                    7,  7,  9,  9,  11, 11, 13, 13, 15, 15};

// UYVY shuf 8 UV to 16 UV.
static const lvec8 kShuffleUYVYUV = {0,  2,  0,  2,  4,  6,  4,  6,  8,  10, 8,
                                     10, 12, 14, 12, 14, 0,  2,  0,  2,  4,  6,
                                     4,  6,  8,  10, 8,  10, 12, 14, 12, 14};
#endif  // HAS_RGB24TOARGBROW_SSSE3

#ifdef HAS_RGB24TOARGBROW_SSSE3
void RGB24ToARGBRow_SSSE3(const uint8_t* src_rgb24,
                          uint8_t* dst_argb,
                          int width) {
  asm volatile(
      "pcmpeqb     %%xmm5,%%xmm5                 \n"  // 0xff000000
      "pslld       $0x18,%%xmm5                  \n"
      "movdqa      %3,%%xmm4                     \n"

      LABELALIGN
      "1:                                        \n"
      "movdqu      (%0),%%xmm0                   \n"
      "movdqu      0x10(%0),%%xmm1               \n"
      "movdqu      0x20(%0),%%xmm3               \n"
      "lea         0x30(%0),%0                   \n"
      "movdqa      %%xmm3,%%xmm2                 \n"
      "palignr     $0x8,%%xmm1,%%xmm2            \n"
      "pshufb      %%xmm4,%%xmm2                 \n"
      "por         %%xmm5,%%xmm2                 \n"
      "palignr     $0xc,%%xmm0,%%xmm1            \n"
      "pshufb      %%xmm4,%%xmm0                 \n"
      "movdqu      %%xmm2,0x20(%1)               \n"
      "por         %%xmm5,%%xmm0                 \n"
      "pshufb      %%xmm4,%%xmm1                 \n"
      "movdqu      %%xmm0,(%1)                   \n"
      "por         %%xmm5,%%xmm1                 \n"
      "palignr     $0x4,%%xmm3,%%xmm3            \n"
      "pshufb      %%xmm4,%%xmm3                 \n"
      "movdqu      %%xmm1,0x10(%1)               \n"
      "por         %%xmm5,%%xmm3                 \n"
      "movdqu      %%xmm3,0x30(%1)               \n"
      "lea         0x40(%1),%1                   \n"
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"
      : "+r"(src_rgb24),              // %0
        "+r"(dst_argb),               // %1
        "+r"(width)                   // %2
      : "m"(kShuffleMaskRGB24ToARGB)  // %3
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5");
}

void RAWToARGBRow_SSSE3(const uint8_t* src_raw, uint8_t* dst_argb, int width) {
  asm volatile(
      "pcmpeqb     %%xmm5,%%xmm5                 \n"  // 0xff000000
      "pslld       $0x18,%%xmm5                  \n"
      "movdqa      %3,%%xmm4                     \n"

      LABELALIGN
      "1:                                        \n"
      "movdqu      (%0),%%xmm0                   \n"
      "movdqu      0x10(%0),%%xmm1               \n"
      "movdqu      0x20(%0),%%xmm3               \n"
      "lea         0x30(%0),%0                   \n"
      "movdqa      %%xmm3,%%xmm2                 \n"
      "palignr     $0x8,%%xmm1,%%xmm2            \n"
      "pshufb      %%xmm4,%%xmm2                 \n"
      "por         %%xmm5,%%xmm2                 \n"
      "palignr     $0xc,%%xmm0,%%xmm1            \n"
      "pshufb      %%xmm4,%%xmm0                 \n"
      "movdqu      %%xmm2,0x20(%1)               \n"
      "por         %%xmm5,%%xmm0                 \n"
      "pshufb      %%xmm4,%%xmm1                 \n"
      "movdqu      %%xmm0,(%1)                   \n"
      "por         %%xmm5,%%xmm1                 \n"
      "palignr     $0x4,%%xmm3,%%xmm3            \n"
      "pshufb      %%xmm4,%%xmm3                 \n"
      "movdqu      %%xmm1,0x10(%1)               \n"
      "por         %%xmm5,%%xmm3                 \n"
      "movdqu      %%xmm3,0x30(%1)               \n"
      "lea         0x40(%1),%1                   \n"
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"
      : "+r"(src_raw),              // %0
        "+r"(dst_argb),             // %1
        "+r"(width)                 // %2
      : "m"(kShuffleMaskRAWToARGB)  // %3
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5");
}

// Same code as RAWToARGB with different shuffler and A in low bits
void RAWToRGBARow_SSSE3(const uint8_t* src_raw, uint8_t* dst_rgba, int width) {
  asm volatile(
      "pcmpeqb     %%xmm5,%%xmm5                 \n"  // 0x000000ff
      "psrld       $0x18,%%xmm5                  \n"
      "movdqa      %3,%%xmm4                     \n"

      LABELALIGN
      "1:                                        \n"
      "movdqu      (%0),%%xmm0                   \n"
      "movdqu      0x10(%0),%%xmm1               \n"
      "movdqu      0x20(%0),%%xmm3               \n"
      "lea         0x30(%0),%0                   \n"
      "movdqa      %%xmm3,%%xmm2                 \n"
      "palignr     $0x8,%%xmm1,%%xmm2            \n"
      "pshufb      %%xmm4,%%xmm2                 \n"
      "por         %%xmm5,%%xmm2                 \n"
      "palignr     $0xc,%%xmm0,%%xmm1            \n"
      "pshufb      %%xmm4,%%xmm0                 \n"
      "movdqu      %%xmm2,0x20(%1)               \n"
      "por         %%xmm5,%%xmm0                 \n"
      "pshufb      %%xmm4,%%xmm1                 \n"
      "movdqu      %%xmm0,(%1)                   \n"
      "por         %%xmm5,%%xmm1                 \n"
      "palignr     $0x4,%%xmm3,%%xmm3            \n"
      "pshufb      %%xmm4,%%xmm3                 \n"
      "movdqu      %%xmm1,0x10(%1)               \n"
      "por         %%xmm5,%%xmm3                 \n"
      "movdqu      %%xmm3,0x30(%1)               \n"
      "lea         0x40(%1),%1                   \n"
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"
      : "+r"(src_raw),              // %0
        "+r"(dst_rgba),             // %1
        "+r"(width)                 // %2
      : "m"(kShuffleMaskRAWToRGBA)  // %3
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5");
}

void ARGBToRGB24Row_SSSE3(const uint8_t* src, uint8_t* dst, int width) {
  asm volatile(

      "movdqa      %3,%%xmm6                     \n"

      LABELALIGN
      "1:                                        \n"
      "movdqu      (%0),%%xmm0                   \n"
      "movdqu      0x10(%0),%%xmm1               \n"
      "movdqu      0x20(%0),%%xmm2               \n"
      "movdqu      0x30(%0),%%xmm3               \n"
      "lea         0x40(%0),%0                   \n"
      "pshufb      %%xmm6,%%xmm0                 \n"
      "pshufb      %%xmm6,%%xmm1                 \n"
      "pshufb      %%xmm6,%%xmm2                 \n"
      "pshufb      %%xmm6,%%xmm3                 \n"
      "movdqa      %%xmm1,%%xmm4                 \n"
      "psrldq      $0x4,%%xmm1                   \n"
      "pslldq      $0xc,%%xmm4                   \n"
      "movdqa      %%xmm2,%%xmm5                 \n"
      "por         %%xmm4,%%xmm0                 \n"
      "pslldq      $0x8,%%xmm5                   \n"
      "movdqu      %%xmm0,(%1)                   \n"
      "por         %%xmm5,%%xmm1                 \n"
      "psrldq      $0x8,%%xmm2                   \n"
      "pslldq      $0x4,%%xmm3                   \n"
      "por         %%xmm3,%%xmm2                 \n"
      "movdqu      %%xmm1,0x10(%1)               \n"
      "movdqu      %%xmm2,0x20(%1)               \n"
      "lea         0x30(%1),%1                   \n"
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"
      : "+r"(src),                    // %0
        "+r"(dst),                    // %1
        "+r"(width)                   // %2
      : "m"(kShuffleMaskARGBToRGB24)  // %3
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6");
}

void ARGBToRAWRow_SSSE3(const uint8_t* src, uint8_t* dst, int width) {
  asm volatile(

      "movdqa      %3,%%xmm6                     \n"

      LABELALIGN
      "1:                                        \n"
      "movdqu      (%0),%%xmm0                   \n"
      "movdqu      0x10(%0),%%xmm1               \n"
      "movdqu      0x20(%0),%%xmm2               \n"
      "movdqu      0x30(%0),%%xmm3               \n"
      "lea         0x40(%0),%0                   \n"
      "pshufb      %%xmm6,%%xmm0                 \n"
      "pshufb      %%xmm6,%%xmm1                 \n"
      "pshufb      %%xmm6,%%xmm2                 \n"
      "pshufb      %%xmm6,%%xmm3                 \n"
      "movdqa      %%xmm1,%%xmm4                 \n"
      "psrldq      $0x4,%%xmm1                   \n"
      "pslldq      $0xc,%%xmm4                   \n"
      "movdqa      %%xmm2,%%xmm5                 \n"
      "por         %%xmm4,%%xmm0                 \n"
      "pslldq      $0x8,%%xmm5                   \n"
      "movdqu      %%xmm0,(%1)                   \n"
      "por         %%xmm5,%%xmm1                 \n"
      "psrldq      $0x8,%%xmm2                   \n"
      "pslldq      $0x4,%%xmm3                   \n"
      "por         %%xmm3,%%xmm2                 \n"
      "movdqu      %%xmm1,0x10(%1)               \n"
      "movdqu      %%xmm2,0x20(%1)               \n"
      "lea         0x30(%1),%1                   \n"
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"
      : "+r"(src),                  // %0
        "+r"(dst),                  // %1
        "+r"(width)                 // %2
      : "m"(kShuffleMaskARGBToRAW)  // %3
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6");
}

#ifdef HAS_ARGBTORGB24ROW_AVX2
// vpermd for 12+12 to 24
static const lvec32 kPermdRGB24_AVX = {0, 1, 2, 4, 5, 6, 3, 7};

void ARGBToRGB24Row_AVX2(const uint8_t* src, uint8_t* dst, int width) {
  asm volatile(
      "vbroadcastf128 %3,%%ymm6                  \n"
      "vmovdqa     %4,%%ymm7                     \n"

      LABELALIGN
      "1:                                        \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "vmovdqu     0x40(%0),%%ymm2               \n"
      "vmovdqu     0x60(%0),%%ymm3               \n"
      "lea         0x80(%0),%0                   \n"
      "vpshufb     %%ymm6,%%ymm0,%%ymm0          \n"  // xxx0yyy0
      "vpshufb     %%ymm6,%%ymm1,%%ymm1          \n"
      "vpshufb     %%ymm6,%%ymm2,%%ymm2          \n"
      "vpshufb     %%ymm6,%%ymm3,%%ymm3          \n"
      "vpermd      %%ymm0,%%ymm7,%%ymm0          \n"  // pack to 24 bytes
      "vpermd      %%ymm1,%%ymm7,%%ymm1          \n"
      "vpermd      %%ymm2,%%ymm7,%%ymm2          \n"
      "vpermd      %%ymm3,%%ymm7,%%ymm3          \n"
      "vpermq      $0x3f,%%ymm1,%%ymm4           \n"  // combine 24 + 8
      "vpor        %%ymm4,%%ymm0,%%ymm0          \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "vpermq      $0xf9,%%ymm1,%%ymm1           \n"  // combine 16 + 16
      "vpermq      $0x4f,%%ymm2,%%ymm4           \n"
      "vpor        %%ymm4,%%ymm1,%%ymm1          \n"
      "vmovdqu     %%ymm1,0x20(%1)               \n"
      "vpermq      $0xfe,%%ymm2,%%ymm2           \n"  // combine 8 + 24
      "vpermq      $0x93,%%ymm3,%%ymm3           \n"
      "vpor        %%ymm3,%%ymm2,%%ymm2          \n"
      "vmovdqu     %%ymm2,0x40(%1)               \n"
      "lea         0x60(%1),%1                   \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper                                \n"
      : "+r"(src),                     // %0
        "+r"(dst),                     // %1
        "+r"(width)                    // %2
      : "m"(kShuffleMaskARGBToRGB24),  // %3
        "m"(kPermdRGB24_AVX)           // %4
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}
#endif

#ifdef HAS_ARGBTORGB24ROW_AVX512VBMI
// Shuffle table for converting ARGBToRGB24
static const ulvec8 kPermARGBToRGB24_0 = {
    0u,  1u,  2u,  4u,  5u,  6u,  8u,  9u,  10u, 12u, 13u,
    14u, 16u, 17u, 18u, 20u, 21u, 22u, 24u, 25u, 26u, 28u,
    29u, 30u, 32u, 33u, 34u, 36u, 37u, 38u, 40u, 41u};
static const ulvec8 kPermARGBToRGB24_1 = {
    10u, 12u, 13u, 14u, 16u, 17u, 18u, 20u, 21u, 22u, 24u,
    25u, 26u, 28u, 29u, 30u, 32u, 33u, 34u, 36u, 37u, 38u,
    40u, 41u, 42u, 44u, 45u, 46u, 48u, 49u, 50u, 52u};
static const ulvec8 kPermARGBToRGB24_2 = {
    21u, 22u, 24u, 25u, 26u, 28u, 29u, 30u, 32u, 33u, 34u,
    36u, 37u, 38u, 40u, 41u, 42u, 44u, 45u, 46u, 48u, 49u,
    50u, 52u, 53u, 54u, 56u, 57u, 58u, 60u, 61u, 62u};

void ARGBToRGB24Row_AVX512VBMI(const uint8_t* src, uint8_t* dst, int width) {
  asm volatile(
      "vmovdqa     %3,%%ymm5                     \n"
      "vmovdqa     %4,%%ymm6                     \n"
      "vmovdqa     %5,%%ymm7                     \n"

      LABELALIGN
      "1:                                        \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "vmovdqu     0x40(%0),%%ymm2               \n"
      "vmovdqu     0x60(%0),%%ymm3               \n"
      "lea         0x80(%0),%0                   \n"
      "vpermt2b    %%ymm1,%%ymm5,%%ymm0          \n"
      "vpermt2b    %%ymm2,%%ymm6,%%ymm1          \n"
      "vpermt2b    %%ymm3,%%ymm7,%%ymm2          \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "vmovdqu     %%ymm1,0x20(%1)               \n"
      "vmovdqu     %%ymm2,0x40(%1)               \n"
      "lea         0x60(%1),%1                   \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper                                \n"
      : "+r"(src),                // %0
        "+r"(dst),                // %1
        "+r"(width)               // %2
      : "m"(kPermARGBToRGB24_0),  // %3
        "m"(kPermARGBToRGB24_1),  // %4
        "m"(kPermARGBToRGB24_2)   // %5
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm5", "xmm6", "xmm7");
}
#endif

#ifdef HAS_ARGBTORAWROW_AVX2
void ARGBToRAWRow_AVX2(const uint8_t* src, uint8_t* dst, int width) {
  asm volatile(
      "vbroadcastf128 %3,%%ymm6                  \n"
      "vmovdqa     %4,%%ymm7                     \n"

      LABELALIGN
      "1:                                        \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "vmovdqu     0x40(%0),%%ymm2               \n"
      "vmovdqu     0x60(%0),%%ymm3               \n"
      "lea         0x80(%0),%0                   \n"
      "vpshufb     %%ymm6,%%ymm0,%%ymm0          \n"  // xxx0yyy0
      "vpshufb     %%ymm6,%%ymm1,%%ymm1          \n"
      "vpshufb     %%ymm6,%%ymm2,%%ymm2          \n"
      "vpshufb     %%ymm6,%%ymm3,%%ymm3          \n"
      "vpermd      %%ymm0,%%ymm7,%%ymm0          \n"  // pack to 24 bytes
      "vpermd      %%ymm1,%%ymm7,%%ymm1          \n"
      "vpermd      %%ymm2,%%ymm7,%%ymm2          \n"
      "vpermd      %%ymm3,%%ymm7,%%ymm3          \n"
      "vpermq      $0x3f,%%ymm1,%%ymm4           \n"  // combine 24 + 8
      "vpor        %%ymm4,%%ymm0,%%ymm0          \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "vpermq      $0xf9,%%ymm1,%%ymm1           \n"  // combine 16 + 16
      "vpermq      $0x4f,%%ymm2,%%ymm4           \n"
      "vpor        %%ymm4,%%ymm1,%%ymm1          \n"
      "vmovdqu     %%ymm1,0x20(%1)               \n"
      "vpermq      $0xfe,%%ymm2,%%ymm2           \n"  // combine 8 + 24
      "vpermq      $0x93,%%ymm3,%%ymm3           \n"
      "vpor        %%ymm3,%%ymm2,%%ymm2          \n"
      "vmovdqu     %%ymm2,0x40(%1)               \n"
      "lea         0x60(%1),%1                   \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper                                \n"
      : "+r"(src),                   // %0
        "+r"(dst),                   // %1
        "+r"(width)                  // %2
      : "m"(kShuffleMaskARGBToRAW),  // %3
        "m"(kPermdRGB24_AVX)         // %4
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}
#endif

#endif  // HAS_RGB24TOARGBROW_SSSE3

// clang-format off

// TODO(mraptis): Consider passing R, G, B multipliers as parameter.
// round parameter is register containing value to add before shift.
#define RGBTOY(round)                            \
  "1:                                        \n" \
  "movdqu    (%0),%%xmm0                     \n" \
  "movdqu    0x10(%0),%%xmm1                 \n" \
  "movdqu    0x20(%0),%%xmm2                 \n" \
  "movdqu    0x30(%0),%%xmm3                 \n" \
  "psubb     %%xmm5,%%xmm0                   \n" \
  "psubb     %%xmm5,%%xmm1                   \n" \
  "psubb     %%xmm5,%%xmm2                   \n" \
  "psubb     %%xmm5,%%xmm3                   \n" \
  "movdqu    %%xmm4,%%xmm6                   \n" \
  "pmaddubsw %%xmm0,%%xmm6                   \n" \
  "movdqu    %%xmm4,%%xmm0                   \n" \
  "pmaddubsw %%xmm1,%%xmm0                   \n" \
  "movdqu    %%xmm4,%%xmm1                   \n" \
  "pmaddubsw %%xmm2,%%xmm1                   \n" \
  "movdqu    %%xmm4,%%xmm2                   \n" \
  "pmaddubsw %%xmm3,%%xmm2                   \n" \
  "lea       0x40(%0),%0                     \n" \
  "phaddw    %%xmm0,%%xmm6                   \n" \
  "phaddw    %%xmm2,%%xmm1                   \n" \
  "prefetcht0 1280(%0)                       \n" \
  "paddw     %%" #round ",%%xmm6             \n" \
  "paddw     %%" #round ",%%xmm1             \n" \
  "psrlw     $0x8,%%xmm6                     \n" \
  "psrlw     $0x8,%%xmm1                     \n" \
  "packuswb  %%xmm1,%%xmm6                   \n" \
  "movdqu    %%xmm6,(%1)                     \n" \
  "lea       0x10(%1),%1                     \n" \
  "sub       $0x10,%2                        \n" \
  "jg        1b                              \n"

#define RGBTOY_AVX2(round)                                       \
  "1:                                        \n"                 \
  "vmovdqu    (%0),%%ymm0                    \n"                 \
  "vmovdqu    0x20(%0),%%ymm1                \n"                 \
  "vmovdqu    0x40(%0),%%ymm2                \n"                 \
  "vmovdqu    0x60(%0),%%ymm3                \n"                 \
  "vpsubb     %%ymm5, %%ymm0, %%ymm0         \n"                 \
  "vpsubb     %%ymm5, %%ymm1, %%ymm1         \n"                 \
  "vpsubb     %%ymm5, %%ymm2, %%ymm2         \n"                 \
  "vpsubb     %%ymm5, %%ymm3, %%ymm3         \n"                 \
  "vpmaddubsw %%ymm0,%%ymm4,%%ymm0           \n"                 \
  "vpmaddubsw %%ymm1,%%ymm4,%%ymm1           \n"                 \
  "vpmaddubsw %%ymm2,%%ymm4,%%ymm2           \n"                 \
  "vpmaddubsw %%ymm3,%%ymm4,%%ymm3           \n"                 \
  "lea       0x80(%0),%0                     \n"                 \
  "vphaddw    %%ymm1,%%ymm0,%%ymm0           \n" /* mutates. */  \
  "vphaddw    %%ymm3,%%ymm2,%%ymm2           \n"                 \
  "prefetcht0 1280(%0)                       \n"                 \
  "vpaddw     %%" #round ",%%ymm0,%%ymm0     \n" /* Add .5 for rounding. */             \
  "vpaddw     %%" #round ",%%ymm2,%%ymm2     \n" \
  "vpsrlw     $0x8,%%ymm0,%%ymm0             \n"                 \
  "vpsrlw     $0x8,%%ymm2,%%ymm2             \n"                 \
  "vpackuswb  %%ymm2,%%ymm0,%%ymm0           \n" /* mutates. */  \
  "vpermd     %%ymm0,%%ymm6,%%ymm0           \n" /* unmutate. */ \
  "vmovdqu    %%ymm0,(%1)                    \n"                 \
  "lea       0x20(%1),%1                     \n"                 \
  "sub       $0x20,%2                        \n"                 \
  "jg        1b                              \n"                 \
  "vzeroupper                                \n"

// clang-format on

#ifdef HAS_ARGBTOYROW_SSSE3
// Convert 16 ARGB pixels (64 bytes) to 16 Y values.
void ARGBToYRow_SSSE3(const uint8_t* src_argb, uint8_t* dst_y, int width) {
  asm volatile(
      "movdqa      %3,%%xmm4                     \n"
      "movdqa      %4,%%xmm5                     \n"
      "movdqa      %5,%%xmm7                     \n"

      LABELALIGN RGBTOY(xmm7)
      : "+r"(src_argb),  // %0
        "+r"(dst_y),     // %1
        "+r"(width)      // %2
      : "m"(kARGBToY),   // %3
        "m"(kSub128),    // %4
        "m"(kAddY16)     // %5
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}
#endif  // HAS_ARGBTOYROW_SSSE3

#ifdef HAS_ARGBTOYROW_AVX2
// vpermd for vphaddw + vpackuswb vpermd.
static const lvec32 kPermdARGBToY_AVX = {0, 4, 1, 5, 2, 6, 3, 7};

// Convert 32 ARGB pixels (128 bytes) to 32 Y values.
void ARGBToYRow_AVX2(const uint8_t* src_argb, uint8_t* dst_y, int width) {
  asm volatile(
      "vbroadcastf128 %3,%%ymm4                  \n"
      "vbroadcastf128 %4,%%ymm5                  \n"
      "vbroadcastf128 %5,%%ymm7                  \n"
      "vmovdqu     %6,%%ymm6                     \n"

      LABELALIGN RGBTOY_AVX2(ymm7)
      : "+r"(src_argb),         // %0
        "+r"(dst_y),            // %1
        "+r"(width)             // %2
      : "m"(kARGBToY),          // %3
        "m"(kSub128),           // %4
        "m"(kAddY16),           // %5
        "m"(kPermdARGBToY_AVX)  // %6
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}
#endif  // HAS_ARGBTOYROW_AVX2

#ifdef HAS_ABGRTOYROW_AVX2
// Convert 32 ABGR pixels (128 bytes) to 32 Y values.
void ABGRToYRow_AVX2(const uint8_t* src_abgr, uint8_t* dst_y, int width) {
  asm volatile(
      "vbroadcastf128 %3,%%ymm4                  \n"
      "vbroadcastf128 %4,%%ymm5                  \n"
      "vbroadcastf128 %5,%%ymm7                  \n"
      "vmovdqu     %6,%%ymm6                     \n"

      LABELALIGN RGBTOY_AVX2(ymm7)
      : "+r"(src_abgr),         // %0
        "+r"(dst_y),            // %1
        "+r"(width)             // %2
      : "m"(kABGRToY),          // %3
        "m"(kSub128),           // %4
        "m"(kAddY16),           // %5
        "m"(kPermdARGBToY_AVX)  // %6
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}
#endif  // HAS_ABGRTOYROW_AVX2

#ifdef HAS_ARGBTOUVROW_SSSE3
void ARGBToUVRow_SSSE3(const uint8_t* src_argb,
                       int src_stride_argb,
                       uint8_t* dst_u,
                       uint8_t* dst_v,
                       int width) {
  asm volatile(
      "movdqa      %5,%%xmm3                     \n"
      "movdqa      %6,%%xmm4                     \n"
      "movdqa      %7,%%xmm5                     \n"
      "sub         %1,%2                         \n"

      LABELALIGN
      "1:                                        \n"
      "movdqu      (%0),%%xmm0                   \n"
      "movdqu      0x00(%0,%4,1),%%xmm7          \n"
      "pavgb       %%xmm7,%%xmm0                 \n"
      "movdqu      0x10(%0),%%xmm1               \n"
      "movdqu      0x10(%0,%4,1),%%xmm7          \n"
      "pavgb       %%xmm7,%%xmm1                 \n"
      "movdqu      0x20(%0),%%xmm2               \n"
      "movdqu      0x20(%0,%4,1),%%xmm7          \n"
      "pavgb       %%xmm7,%%xmm2                 \n"
      "movdqu      0x30(%0),%%xmm6               \n"
      "movdqu      0x30(%0,%4,1),%%xmm7          \n"
      "pavgb       %%xmm7,%%xmm6                 \n"

      "lea         0x40(%0),%0                   \n"
      "movdqa      %%xmm0,%%xmm7                 \n"
      "shufps      $0x88,%%xmm1,%%xmm0           \n"
      "shufps      $0xdd,%%xmm1,%%xmm7           \n"
      "pavgb       %%xmm7,%%xmm0                 \n"
      "movdqa      %%xmm2,%%xmm7                 \n"
      "shufps      $0x88,%%xmm6,%%xmm2           \n"
      "shufps      $0xdd,%%xmm6,%%xmm7           \n"
      "pavgb       %%xmm7,%%xmm2                 \n"
      "movdqa      %%xmm0,%%xmm1                 \n"
      "movdqa      %%xmm2,%%xmm6                 \n"
      "pmaddubsw   %%xmm4,%%xmm0                 \n"
      "pmaddubsw   %%xmm4,%%xmm2                 \n"
      "pmaddubsw   %%xmm3,%%xmm1                 \n"
      "pmaddubsw   %%xmm3,%%xmm6                 \n"
      "phaddw      %%xmm2,%%xmm0                 \n"
      "phaddw      %%xmm6,%%xmm1                 \n"
      "psraw       $0x8,%%xmm0                   \n"
      "psraw       $0x8,%%xmm1                   \n"
      "packsswb    %%xmm1,%%xmm0                 \n"
      "paddb       %%xmm5,%%xmm0                 \n"
      "movlps      %%xmm0,(%1)                   \n"
      "movhps      %%xmm0,0x00(%1,%2,1)          \n"
      "lea         0x8(%1),%1                    \n"
      "sub         $0x10,%3                      \n"
      "jg          1b                            \n"
      : "+r"(src_argb),                    // %0
        "+r"(dst_u),                       // %1
        "+r"(dst_v),                       // %2
        "+rm"(width)                       // %3
      : "r"((intptr_t)(src_stride_argb)),  // %4
        "m"(kARGBToV),                     // %5
        "m"(kARGBToU),                     // %6
        "m"(kAddUV128)                     // %7
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm6", "xmm7");
}
#endif  // HAS_ARGBTOUVROW_SSSE3

#ifdef HAS_ARGBTOUVROW_AVX2
// vpshufb for vphaddw + vpackuswb packed to shorts.
static const lvec8 kShufARGBToUV_AVX = {
    0, 1, 8, 9, 2, 3, 10, 11, 4, 5, 12, 13, 6, 7, 14, 15,
    0, 1, 8, 9, 2, 3, 10, 11, 4, 5, 12, 13, 6, 7, 14, 15};
void ARGBToUVRow_AVX2(const uint8_t* src_argb,
                      int src_stride_argb,
                      uint8_t* dst_u,
                      uint8_t* dst_v,
                      int width) {
  asm volatile(
      "vbroadcastf128 %5,%%ymm5                  \n"
      "vbroadcastf128 %6,%%ymm6                  \n"
      "vbroadcastf128 %7,%%ymm7                  \n"
      "sub         %1,%2                         \n"

      LABELALIGN
      "1:                                        \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "vmovdqu     0x40(%0),%%ymm2               \n"
      "vmovdqu     0x60(%0),%%ymm3               \n"
      "vpavgb      0x00(%0,%4,1),%%ymm0,%%ymm0   \n"
      "vpavgb      0x20(%0,%4,1),%%ymm1,%%ymm1   \n"
      "vpavgb      0x40(%0,%4,1),%%ymm2,%%ymm2   \n"
      "vpavgb      0x60(%0,%4,1),%%ymm3,%%ymm3   \n"
      "lea         0x80(%0),%0                   \n"
      "vshufps     $0x88,%%ymm1,%%ymm0,%%ymm4    \n"
      "vshufps     $0xdd,%%ymm1,%%ymm0,%%ymm0    \n"
      "vpavgb      %%ymm4,%%ymm0,%%ymm0          \n"
      "vshufps     $0x88,%%ymm3,%%ymm2,%%ymm4    \n"
      "vshufps     $0xdd,%%ymm3,%%ymm2,%%ymm2    \n"
      "vpavgb      %%ymm4,%%ymm2,%%ymm2          \n"

      "vpmaddubsw  %%ymm7,%%ymm0,%%ymm1          \n"
      "vpmaddubsw  %%ymm7,%%ymm2,%%ymm3          \n"
      "vpmaddubsw  %%ymm6,%%ymm0,%%ymm0          \n"
      "vpmaddubsw  %%ymm6,%%ymm2,%%ymm2          \n"
      "vphaddw     %%ymm3,%%ymm1,%%ymm1          \n"
      "vphaddw     %%ymm2,%%ymm0,%%ymm0          \n"
      "vpsraw      $0x8,%%ymm1,%%ymm1            \n"
      "vpsraw      $0x8,%%ymm0,%%ymm0            \n"
      "vpacksswb   %%ymm0,%%ymm1,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vpshufb     %8,%%ymm0,%%ymm0              \n"
      "vpaddb      %%ymm5,%%ymm0,%%ymm0          \n"

      "vextractf128 $0x0,%%ymm0,(%1)             \n"
      "vextractf128 $0x1,%%ymm0,0x0(%1,%2,1)     \n"
      "lea         0x10(%1),%1                   \n"
      "sub         $0x20,%3                      \n"
      "jg          1b                            \n"
      "vzeroupper                                \n"
      : "+r"(src_argb),                    // %0
        "+r"(dst_u),                       // %1
        "+r"(dst_v),                       // %2
        "+rm"(width)                       // %3
      : "r"((intptr_t)(src_stride_argb)),  // %4
        "m"(kAddUV128),                    // %5
        "m"(kARGBToV),                     // %6
        "m"(kARGBToU),                     // %7
        "m"(kShufARGBToUV_AVX)             // %8
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}
#endif  // HAS_ARGBTOUVROW_AVX2

#ifdef HAS_ABGRTOUVROW_AVX2
void ABGRToUVRow_AVX2(const uint8_t* src_abgr,
                      int src_stride_abgr,
                      uint8_t* dst_u,
                      uint8_t* dst_v,
                      int width) {
  asm volatile(
      "vbroadcastf128 %5,%%ymm5                  \n"
      "vbroadcastf128 %6,%%ymm6                  \n"
      "vbroadcastf128 %7,%%ymm7                  \n"
      "sub         %1,%2                         \n"

      LABELALIGN
      "1:                                        \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "vmovdqu     0x40(%0),%%ymm2               \n"
      "vmovdqu     0x60(%0),%%ymm3               \n"
      "vpavgb      0x00(%0,%4,1),%%ymm0,%%ymm0   \n"
      "vpavgb      0x20(%0,%4,1),%%ymm1,%%ymm1   \n"
      "vpavgb      0x40(%0,%4,1),%%ymm2,%%ymm2   \n"
      "vpavgb      0x60(%0,%4,1),%%ymm3,%%ymm3   \n"
      "lea         0x80(%0),%0                   \n"
      "vshufps     $0x88,%%ymm1,%%ymm0,%%ymm4    \n"
      "vshufps     $0xdd,%%ymm1,%%ymm0,%%ymm0    \n"
      "vpavgb      %%ymm4,%%ymm0,%%ymm0          \n"
      "vshufps     $0x88,%%ymm3,%%ymm2,%%ymm4    \n"
      "vshufps     $0xdd,%%ymm3,%%ymm2,%%ymm2    \n"
      "vpavgb      %%ymm4,%%ymm2,%%ymm2          \n"

      "vpmaddubsw  %%ymm7,%%ymm0,%%ymm1          \n"
      "vpmaddubsw  %%ymm7,%%ymm2,%%ymm3          \n"
      "vpmaddubsw  %%ymm6,%%ymm0,%%ymm0          \n"
      "vpmaddubsw  %%ymm6,%%ymm2,%%ymm2          \n"
      "vphaddw     %%ymm3,%%ymm1,%%ymm1          \n"
      "vphaddw     %%ymm2,%%ymm0,%%ymm0          \n"
      "vpsraw      $0x8,%%ymm1,%%ymm1            \n"
      "vpsraw      $0x8,%%ymm0,%%ymm0            \n"
      "vpacksswb   %%ymm0,%%ymm1,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vpshufb     %8,%%ymm0,%%ymm0              \n"
      "vpaddb      %%ymm5,%%ymm0,%%ymm0          \n"

      "vextractf128 $0x0,%%ymm0,(%1)             \n"
      "vextractf128 $0x1,%%ymm0,0x0(%1,%2,1)     \n"
      "lea         0x10(%1),%1                   \n"
      "sub         $0x20,%3                      \n"
      "jg          1b                            \n"
      "vzeroupper                                \n"
      : "+r"(src_abgr),                    // %0
        "+r"(dst_u),                       // %1
        "+r"(dst_v),                       // %2
        "+rm"(width)                       // %3
      : "r"((intptr_t)(src_stride_abgr)),  // %4
        "m"(kAddUV128),                    // %5
        "m"(kABGRToV),                     // %6
        "m"(kABGRToU),                     // %7
        "m"(kShufARGBToUV_AVX)             // %8
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}
#endif  // HAS_ABGRTOUVROW_AVX2

void BGRAToYRow_SSSE3(const uint8_t* src_bgra, uint8_t* dst_y, int width) {
  asm volatile(
      "movdqa      %3,%%xmm4                     \n"
      "movdqa      %4,%%xmm5                     \n"
      "movdqa      %5,%%xmm7                     \n"

      LABELALIGN RGBTOY(xmm7)
      : "+r"(src_bgra),  // %0
        "+r"(dst_y),     // %1
        "+r"(width)      // %2
      : "m"(kBGRAToY),   // %3
        "m"(kSub128),    // %4
        "m"(kAddY16)     // %5
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}

void BGRAToUVRow_SSSE3(const uint8_t* src_bgra,
                       int src_stride_bgra,
                       uint8_t* dst_u,
                       uint8_t* dst_v,
                       int width) {
  asm volatile(
      "movdqa      %5,%%xmm3                     \n"
      "movdqa      %6,%%xmm4                     \n"
      "movdqa      %7,%%xmm5                     \n"
      "sub         %1,%2                         \n"

      LABELALIGN
      "1:                                        \n"
      "movdqu      (%0),%%xmm0                   \n"
      "movdqu      0x00(%0,%4,1),%%xmm7          \n"
      "pavgb       %%xmm7,%%xmm0                 \n"
      "movdqu      0x10(%0),%%xmm1               \n"
      "movdqu      0x10(%0,%4,1),%%xmm7          \n"
      "pavgb       %%xmm7,%%xmm1                 \n"
      "movdqu      0x20(%0),%%xmm2               \n"
      "movdqu      0x20(%0,%4,1),%%xmm7          \n"
      "pavgb       %%xmm7,%%xmm2                 \n"
      "movdqu      0x30(%0),%%xmm6               \n"
      "movdqu      0x30(%0,%4,1),%%xmm7          \n"
      "pavgb       %%xmm7,%%xmm6                 \n"

      "lea         0x40(%0),%0                   \n"
      "movdqa      %%xmm0,%%xmm7                 \n"
      "shufps      $0x88,%%xmm1,%%xmm0           \n"
      "shufps      $0xdd,%%xmm1,%%xmm7           \n"
      "pavgb       %%xmm7,%%xmm0                 \n"
      "movdqa      %%xmm2,%%xmm7                 \n"
      "shufps      $0x88,%%xmm6,%%xmm2           \n"
      "shufps      $0xdd,%%xmm6,%%xmm7           \n"
      "pavgb       %%xmm7,%%xmm2                 \n"
      "movdqa      %%xmm0,%%xmm1                 \n"
      "movdqa      %%xmm2,%%xmm6                 \n"
      "pmaddubsw   %%xmm4,%%xmm0                 \n"
      "pmaddubsw   %%xmm4,%%xmm2                 \n"
      "pmaddubsw   %%xmm3,%%xmm1                 \n"
      "pmaddubsw   %%xmm3,%%xmm6                 \n"
      "phaddw      %%xmm2,%%xmm0                 \n"
      "phaddw      %%xmm6,%%xmm1                 \n"
      "psraw       $0x8,%%xmm0                   \n"
      "psraw       $0x8,%%xmm1                   \n"
      "packsswb    %%xmm1,%%xmm0                 \n"
      "paddb       %%xmm5,%%xmm0                 \n"
      "movlps      %%xmm0,(%1)                   \n"
      "movhps      %%xmm0,0x00(%1,%2,1)          \n"
      "lea         0x8(%1),%1                    \n"
      "sub         $0x10,%3                      \n"
      "jg          1b                            \n"
      : "+r"(src_bgra),                    // %0
        "+r"(dst_u),                       // %1
        "+r"(dst_v),                       // %2
        "+rm"(width)                       // %3
      : "r"((intptr_t)(src_stride_bgra)),  // %4
        "m"(kBGRAToV),                     // %5
        "m"(kBGRAToU),                     // %6
        "m"(kAddUV128)                     // %7
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm6", "xmm7");
}

void ABGRToYRow_SSSE3(const uint8_t* src_abgr, uint8_t* dst_y, int width) {
  asm volatile(
      "movdqa      %3,%%xmm4                     \n"
      "movdqa      %4,%%xmm5                     \n"
      "movdqa      %5,%%xmm7                     \n"

      LABELALIGN RGBTOY(xmm7)
      : "+r"(src_abgr),  // %0
        "+r"(dst_y),     // %1
        "+r"(width)      // %2
      : "m"(kABGRToY),   // %3
        "m"(kSub128),    // %4
        "m"(kAddY16)     // %5
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}

void RGBAToYRow_SSSE3(const uint8_t* src_rgba, uint8_t* dst_y, int width) {
  asm volatile(
      "movdqa      %3,%%xmm4                     \n"
      "movdqa      %4,%%xmm5                     \n"
      "movdqa      %5,%%xmm7                     \n"

      LABELALIGN RGBTOY(xmm7)
      : "+r"(src_rgba),  // %0
        "+r"(dst_y),     // %1
        "+r"(width)      // %2
      : "m"(kRGBAToY),   // %3
        "m"(kSub128),    // %4
        "m"(kAddY16)     // %5
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}

void ABGRToUVRow_SSSE3(const uint8_t* src_abgr,
                       int src_stride_abgr,
                       uint8_t* dst_u,
                       uint8_t* dst_v,
                       int width) {
  asm volatile(
      "movdqa      %5,%%xmm3                     \n"
      "movdqa      %6,%%xmm4                     \n"
      "movdqa      %7,%%xmm5                     \n"
      "sub         %1,%2                         \n"

      LABELALIGN
      "1:                                        \n"
      "movdqu      (%0),%%xmm0                   \n"
      "movdqu      0x00(%0,%4,1),%%xmm7          \n"
      "pavgb       %%xmm7,%%xmm0                 \n"
      "movdqu      0x10(%0),%%xmm1               \n"
      "movdqu      0x10(%0,%4,1),%%xmm7          \n"
      "pavgb       %%xmm7,%%xmm1                 \n"
      "movdqu      0x20(%0),%%xmm2               \n"
      "movdqu      0x20(%0,%4,1),%%xmm7          \n"
      "pavgb       %%xmm7,%%xmm2                 \n"
      "movdqu      0x30(%0),%%xmm6               \n"
      "movdqu      0x30(%0,%4,1),%%xmm7          \n"
      "pavgb       %%xmm7,%%xmm6                 \n"

      "lea         0x40(%0),%0                   \n"
      "movdqa      %%xmm0,%%xmm7                 \n"
      "shufps      $0x88,%%xmm1,%%xmm0           \n"
      "shufps      $0xdd,%%xmm1,%%xmm7           \n"
      "pavgb       %%xmm7,%%xmm0                 \n"
      "movdqa      %%xmm2,%%xmm7                 \n"
      "shufps      $0x88,%%xmm6,%%xmm2           \n"
      "shufps      $0xdd,%%xmm6,%%xmm7           \n"
      "pavgb       %%xmm7,%%xmm2                 \n"
      "movdqa      %%xmm0,%%xmm1                 \n"
      "movdqa      %%xmm2,%%xmm6                 \n"
      "pmaddubsw   %%xmm4,%%xmm0                 \n"
      "pmaddubsw   %%xmm4,%%xmm2                 \n"
      "pmaddubsw   %%xmm3,%%xmm1                 \n"
      "pmaddubsw   %%xmm3,%%xmm6                 \n"
      "phaddw      %%xmm2,%%xmm0                 \n"
      "phaddw      %%xmm6,%%xmm1                 \n"
      "psraw       $0x8,%%xmm0                   \n"
      "psraw       $0x8,%%xmm1                   \n"
      "packsswb    %%xmm1,%%xmm0                 \n"
      "paddb       %%xmm5,%%xmm0                 \n"
      "movlps      %%xmm0,(%1)                   \n"
      "movhps      %%xmm0,0x00(%1,%2,1)          \n"
      "lea         0x8(%1),%1                    \n"
      "sub         $0x10,%3                      \n"
      "jg          1b                            \n"
      : "+r"(src_abgr),                    // %0
        "+r"(dst_u),                       // %1
        "+r"(dst_v),                       // %2
        "+rm"(width)                       // %3
      : "r"((intptr_t)(src_stride_abgr)),  // %4
        "m"(kABGRToV),                     // %5
        "m"(kABGRToU),                     // %6
        "m"(kAddUV128)                     // %7
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm6", "xmm7");
}

void RGBAToUVRow_SSSE3(const uint8_t* src_rgba,
                       int src_stride_rgba,
                       uint8_t* dst_u,
                       uint8_t* dst_v,
                       int width) {
  asm volatile(
      "movdqa      %5,%%xmm3                     \n"
      "movdqa      %6,%%xmm4                     \n"
      "movdqa      %7,%%xmm5                     \n"
      "sub         %1,%2                         \n"

      LABELALIGN
      "1:                                        \n"
      "movdqu      (%0),%%xmm0                   \n"
      "movdqu      0x00(%0,%4,1),%%xmm7          \n"
      "pavgb       %%xmm7,%%xmm0                 \n"
      "movdqu      0x10(%0),%%xmm1               \n"
      "movdqu      0x10(%0,%4,1),%%xmm7          \n"
      "pavgb       %%xmm7,%%xmm1                 \n"
      "movdqu      0x20(%0),%%xmm2               \n"
      "movdqu      0x20(%0,%4,1),%%xmm7          \n"
      "pavgb       %%xmm7,%%xmm2                 \n"
      "movdqu      0x30(%0),%%xmm6               \n"
      "movdqu      0x30(%0,%4,1),%%xmm7          \n"
      "pavgb       %%xmm7,%%xmm6                 \n"

      "lea         0x40(%0),%0                   \n"
      "movdqa      %%xmm0,%%xmm7                 \n"
      "shufps      $0x88,%%xmm1,%%xmm0           \n"
      "shufps      $0xdd,%%xmm1,%%xmm7           \n"
      "pavgb       %%xmm7,%%xmm0                 \n"
      "movdqa      %%xmm2,%%xmm7                 \n"
      "shufps      $0x88,%%xmm6,%%xmm2           \n"
      "shufps      $0xdd,%%xmm6,%%xmm7           \n"
      "pavgb       %%xmm7,%%xmm2                 \n"
      "movdqa      %%xmm0,%%xmm1                 \n"
      "movdqa      %%xmm2,%%xmm6                 \n"
      "pmaddubsw   %%xmm4,%%xmm0                 \n"
      "pmaddubsw   %%xmm4,%%xmm2                 \n"
      "pmaddubsw   %%xmm3,%%xmm1                 \n"
      "pmaddubsw   %%xmm3,%%xmm6                 \n"
      "phaddw      %%xmm2,%%xmm0                 \n"
      "phaddw      %%xmm6,%%xmm1                 \n"
      "psraw       $0x8,%%xmm0                   \n"
      "psraw       $0x8,%%xmm1                   \n"
      "packsswb    %%xmm1,%%xmm0                 \n"
      "paddb       %%xmm5,%%xmm0                 \n"
      "movlps      %%xmm0,(%1)                   \n"
      "movhps      %%xmm0,0x00(%1,%2,1)          \n"
      "lea         0x8(%1),%1                    \n"
      "sub         $0x10,%3                      \n"
      "jg          1b                            \n"
      : "+r"(src_rgba),                    // %0
        "+r"(dst_u),                       // %1
        "+r"(dst_v),                       // %2
        "+rm"(width)                       // %3
      : "r"((intptr_t)(src_stride_rgba)),  // %4
        "m"(kRGBAToV),                     // %5
        "m"(kRGBAToU),                     // %6
        "m"(kAddUV128)                     // %7
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm6", "xmm7");
}

#if defined(HAS_I422TOARGBROW_SSSE3) || defined(HAS_I422TOARGBROW_AVX2)

// Read 8 UV from 444
#define READYUV444                                                \
  "movq       (%[u_buf]),%%xmm3                               \n" \
  "movq       0x00(%[u_buf],%[v_buf],1),%%xmm1                \n" \
  "lea        0x8(%[u_buf]),%[u_buf]                          \n" \
  "punpcklbw  %%xmm1,%%xmm3                                   \n" \
  "movq       (%[y_buf]),%%xmm4                               \n" \
  "punpcklbw  %%xmm4,%%xmm4                                   \n" \
  "lea        0x8(%[y_buf]),%[y_buf]                          \n"

// Read 4 UV from 422, upsample to 8 UV
#define READYUV422                                                \
  "movd       (%[u_buf]),%%xmm3                               \n" \
  "movd       0x00(%[u_buf],%[v_buf],1),%%xmm1                \n" \
  "lea        0x4(%[u_buf]),%[u_buf]                          \n" \
  "punpcklbw  %%xmm1,%%xmm3                                   \n" \
  "punpcklwd  %%xmm3,%%xmm3                                   \n" \
  "movq       (%[y_buf]),%%xmm4                               \n" \
  "punpcklbw  %%xmm4,%%xmm4                                   \n" \
  "lea        0x8(%[y_buf]),%[y_buf]                          \n"

// Read 4 UV from 422, upsample to 8 UV.  With 8 Alpha.
#define READYUVA422                                               \
  "movd       (%[u_buf]),%%xmm3                               \n" \
  "movd       0x00(%[u_buf],%[v_buf],1),%%xmm1                \n" \
  "lea        0x4(%[u_buf]),%[u_buf]                          \n" \
  "punpcklbw  %%xmm1,%%xmm3                                   \n" \
  "punpcklwd  %%xmm3,%%xmm3                                   \n" \
  "movq       (%[y_buf]),%%xmm4                               \n" \
  "punpcklbw  %%xmm4,%%xmm4                                   \n" \
  "lea        0x8(%[y_buf]),%[y_buf]                          \n" \
  "movq       (%[a_buf]),%%xmm5                               \n" \
  "lea        0x8(%[a_buf]),%[a_buf]                          \n"

// Read 4 UYVY with 8 Y and update 4 UV to 8 UV.
#define READUYVY                                                  \
  "movdqu     (%[uyvy_buf]),%%xmm4                            \n" \
  "pshufb     %[kShuffleUYVYY], %%xmm4                        \n" \
  "movdqu     (%[uyvy_buf]),%%xmm3                            \n" \
  "pshufb     %[kShuffleUYVYUV], %%xmm3                       \n" \
  "lea        0x10(%[uyvy_buf]),%[uyvy_buf]                   \n"

#if defined(__x86_64__)
#define YUVTORGB_SETUP(yuvconstants)                              \
  "pcmpeqb    %%xmm13,%%xmm13                                 \n" \
  "movdqa     (%[yuvconstants]),%%xmm8                        \n" \
  "pxor       %%xmm12,%%xmm12                                 \n" \
  "movdqa     32(%[yuvconstants]),%%xmm9                      \n" \
  "psllw      $7,%%xmm13                                      \n" \
  "movdqa     64(%[yuvconstants]),%%xmm10                     \n" \
  "pshufb     %%xmm12,%%xmm13                                 \n" \
  "movdqa     96(%[yuvconstants]),%%xmm11                     \n" \
  "movdqa     128(%[yuvconstants]),%%xmm12                    \n"

// Convert 8 pixels: 8 UV and 8 Y
#define YUVTORGB16(yuvconstants)                                  \
  "psubb      %%xmm13,%%xmm3                                  \n" \
  "pmulhuw    %%xmm11,%%xmm4                                  \n" \
  "movdqa     %%xmm8,%%xmm0                                   \n" \
  "movdqa     %%xmm9,%%xmm1                                   \n" \
  "movdqa     %%xmm10,%%xmm2                                  \n" \
  "paddw      %%xmm12,%%xmm4                                  \n" \
  "pmaddubsw  %%xmm3,%%xmm0                                   \n" \
  "pmaddubsw  %%xmm3,%%xmm1                                   \n" \
  "pmaddubsw  %%xmm3,%%xmm2                                   \n" \
  "paddsw     %%xmm4,%%xmm0                                   \n" \
  "paddsw     %%xmm4,%%xmm2                                   \n" \
  "psubsw     %%xmm1,%%xmm4                                   \n" \
  "movdqa     %%xmm4,%%xmm1                                   \n"

#define YUVTORGB_REGS "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13",

#else
#define YUVTORGB_SETUP(yuvconstants)
// Convert 8 pixels: 8 UV and 8 Y
#define YUVTORGB16(yuvconstants)                                  \
  "pcmpeqb    %%xmm0,%%xmm0                                   \n" \
  "pxor       %%xmm1,%%xmm1                                   \n" \
  "psllw      $7,%%xmm0                                       \n" \
  "pshufb     %%xmm1,%%xmm0                                   \n" \
  "psubb      %%xmm0,%%xmm3                                   \n" \
  "pmulhuw    96(%[yuvconstants]),%%xmm4                      \n" \
  "movdqa     (%[yuvconstants]),%%xmm0                        \n" \
  "movdqa     32(%[yuvconstants]),%%xmm1                      \n" \
  "movdqa     64(%[yuvconstants]),%%xmm2                      \n" \
  "pmaddubsw  %%xmm3,%%xmm0                                   \n" \
  "pmaddubsw  %%xmm3,%%xmm1                                   \n" \
  "pmaddubsw  %%xmm3,%%xmm2                                   \n" \
  "movdqa     128(%[yuvconstants]),%%xmm3                     \n" \
  "paddw      %%xmm3,%%xmm4                                   \n" \
  "paddsw     %%xmm4,%%xmm0                                   \n" \
  "paddsw     %%xmm4,%%xmm2                                   \n" \
  "psubsw     %%xmm1,%%xmm4                                   \n" \
  "movdqa     %%xmm4,%%xmm1                                   \n"

#define YUVTORGB_REGS
#endif

#define YUVTORGB(yuvconstants)                                    \
  YUVTORGB16(yuvconstants)                                        \
  "psraw      $0x6,%%xmm0                                     \n" \
  "psraw      $0x6,%%xmm1                                     \n" \
  "psraw      $0x6,%%xmm2                                     \n" \
  "packuswb   %%xmm0,%%xmm0                                   \n" \
  "packuswb   %%xmm1,%%xmm1                                   \n" \
  "packuswb   %%xmm2,%%xmm2                                   \n"

// Store 8 ARGB values.
#define STOREARGB                                                  \
  "punpcklbw  %%xmm1,%%xmm0                                    \n" \
  "punpcklbw  %%xmm5,%%xmm2                                    \n" \
  "movdqa     %%xmm0,%%xmm1                                    \n" \
  "punpcklwd  %%xmm2,%%xmm0                                    \n" \
  "punpckhwd  %%xmm2,%%xmm1                                    \n" \
  "movdqu     %%xmm0,(%[dst_argb])                             \n" \
  "movdqu     %%xmm1,0x10(%[dst_argb])                         \n" \
  "lea        0x20(%[dst_argb]), %[dst_argb]                   \n"

// Store 8 RGBA values.
#define STORERGBA                                                  \
  "pcmpeqb   %%xmm5,%%xmm5                                     \n" \
  "punpcklbw %%xmm2,%%xmm1                                     \n" \
  "punpcklbw %%xmm0,%%xmm5                                     \n" \
  "movdqa    %%xmm5,%%xmm0                                     \n" \
  "punpcklwd %%xmm1,%%xmm5                                     \n" \
  "punpckhwd %%xmm1,%%xmm0                                     \n" \
  "movdqu    %%xmm5,(%[dst_rgba])                              \n" \
  "movdqu    %%xmm0,0x10(%[dst_rgba])                          \n" \
  "lea       0x20(%[dst_rgba]),%[dst_rgba]                     \n"

void OMITFP I422ToARGBRow_SSSE3(const uint8_t* y_buf,
                                const uint8_t* u_buf,
                                const uint8_t* v_buf,
                                uint8_t* dst_argb,
                                const struct YuvConstants* yuvconstants,
                                int width) {
  asm volatile (
    YUVTORGB_SETUP(yuvconstants)
      "sub         %[u_buf],%[v_buf]             \n"
      "pcmpeqb     %%xmm5,%%xmm5                 \n"

    LABELALIGN
      "1:                                        \n"
    READYUV422
    YUVTORGB(yuvconstants)
    STOREARGB
      "sub         $0x8,%[width]                 \n"
      "jg          1b                            \n"
  : [y_buf]"+r"(y_buf),    // %[y_buf]
    [u_buf]"+r"(u_buf),    // %[u_buf]
    [v_buf]"+r"(v_buf),    // %[v_buf]
    [dst_argb]"+r"(dst_argb),  // %[dst_argb]
    [width]"+rm"(width)    // %[width]
  : [yuvconstants]"r"(yuvconstants)  // %[yuvconstants]
  : "memory", "cc", YUVTORGB_REGS
    "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5"
  );
}

#ifdef HAS_I422ALPHATOARGBROW_SSSE3
void OMITFP I422AlphaToARGBRow_SSSE3(const uint8_t* y_buf,
                                     const uint8_t* u_buf,
                                     const uint8_t* v_buf,
                                     const uint8_t* a_buf,
                                     uint8_t* dst_argb,
                                     const struct YuvConstants* yuvconstants,
                                     int width) {
  // clang-format off
  asm volatile (
    YUVTORGB_SETUP(yuvconstants)
      "sub         %[u_buf],%[v_buf]             \n"

    LABELALIGN
      "1:                                        \n"
    READYUVA422
    YUVTORGB(yuvconstants)
    STOREARGB
      "subl        $0x8,%[width]                 \n"
      "jg          1b                            \n"
  : [y_buf]"+r"(y_buf),    // %[y_buf]
    [u_buf]"+r"(u_buf),    // %[u_buf]
    [v_buf]"+r"(v_buf),    // %[v_buf]
    [a_buf]"+r"(a_buf),    // %[a_buf]
    [dst_argb]"+r"(dst_argb),  // %[dst_argb]
#if defined(__i386__)
    [width]"+m"(width)     // %[width]
#else
    [width]"+rm"(width)    // %[width]
#endif
  : [yuvconstants]"r"(yuvconstants)  // %[yuvconstants]
  : "memory", "cc", YUVTORGB_REGS
    "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5"
  );
  // clang-format on
}
#endif  // HAS_I422ALPHATOARGBROW_SSSE3

void OMITFP UYVYToARGBRow_SSSE3(const uint8_t* uyvy_buf,
                                uint8_t* dst_argb,
                                const struct YuvConstants* yuvconstants,
                                int width) {
  // clang-format off
  asm volatile (
    YUVTORGB_SETUP(yuvconstants)
      "pcmpeqb     %%xmm5,%%xmm5                 \n"

    LABELALIGN
      "1:                                        \n"
    READUYVY
    YUVTORGB(yuvconstants)
    STOREARGB
      "sub         $0x8,%[width]                 \n"
      "jg          1b                            \n"
  : [uyvy_buf]"+r"(uyvy_buf),    // %[uyvy_buf]
    [dst_argb]"+r"(dst_argb),  // %[dst_argb]
    [width]"+rm"(width)    // %[width]
  : [yuvconstants]"r"(yuvconstants), // %[yuvconstants]
    [kShuffleUYVYY]"m"(kShuffleUYVYY),
    [kShuffleUYVYUV]"m"(kShuffleUYVYUV)
    : "memory", "cc", YUVTORGB_REGS
      "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5"
  );
  // clang-format on
}

void OMITFP I422ToRGBARow_SSSE3(const uint8_t* y_buf,
                                const uint8_t* u_buf,
                                const uint8_t* v_buf,
                                uint8_t* dst_rgba,
                                const struct YuvConstants* yuvconstants,
                                int width) {
  asm volatile (
    YUVTORGB_SETUP(yuvconstants)
      "sub         %[u_buf],%[v_buf]             \n"
      "pcmpeqb     %%xmm5,%%xmm5                 \n"

    LABELALIGN
      "1:                                        \n"
    READYUV422
    YUVTORGB(yuvconstants)
    STORERGBA
      "sub         $0x8,%[width]                 \n"
      "jg          1b                            \n"
  : [y_buf]"+r"(y_buf),    // %[y_buf]
    [u_buf]"+r"(u_buf),    // %[u_buf]
    [v_buf]"+r"(v_buf),    // %[v_buf]
    [dst_rgba]"+r"(dst_rgba),  // %[dst_rgba]
    [width]"+rm"(width)    // %[width]
  : [yuvconstants]"r"(yuvconstants)  // %[yuvconstants]
  : "memory", "cc", YUVTORGB_REGS
    "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5"
  );
}

#endif  // HAS_I422TOARGBROW_SSSE3

// Read 8 UV from 422, upsample to 16 UV.
#define READYUV422_AVX2                                               \
  "vmovq      (%[u_buf]),%%xmm3                                   \n" \
  "vmovq      0x00(%[u_buf],%[v_buf],1),%%xmm1                    \n" \
  "lea        0x8(%[u_buf]),%[u_buf]                              \n" \
  "vpunpcklbw %%ymm1,%%ymm3,%%ymm3                                \n" \
  "vpermq     $0xd8,%%ymm3,%%ymm3                                 \n" \
  "vpunpcklwd %%ymm3,%%ymm3,%%ymm3                                \n" \
  "vmovdqu    (%[y_buf]),%%xmm4                                   \n" \
  "vpermq     $0xd8,%%ymm4,%%ymm4                                 \n" \
  "vpunpcklbw %%ymm4,%%ymm4,%%ymm4                                \n" \
  "lea        0x10(%[y_buf]),%[y_buf]                             \n"

// Read 16 UV from 444.  With 16 Alpha.
#define READYUVA444_AVX2                                              \
  "vmovdqu    (%[u_buf]),%%xmm3                                   \n" \
  "vmovdqu    0x00(%[u_buf],%[v_buf],1),%%xmm1                    \n" \
  "lea        0x10(%[u_buf]),%[u_buf]                             \n" \
  "vpermq     $0xd8,%%ymm3,%%ymm3                                 \n" \
  "vpermq     $0xd8,%%ymm1,%%ymm1                                 \n" \
  "vpunpcklbw %%ymm1,%%ymm3,%%ymm3                                \n" \
  "vmovdqu    (%[y_buf]),%%xmm4                                   \n" \
  "vpermq     $0xd8,%%ymm4,%%ymm4                                 \n" \
  "vpunpcklbw %%ymm4,%%ymm4,%%ymm4                                \n" \
  "lea        0x10(%[y_buf]),%[y_buf]                             \n" \
  "vmovdqu    (%[a_buf]),%%xmm5                                   \n" \
  "vpermq     $0xd8,%%ymm5,%%ymm5                                 \n" \
  "lea        0x10(%[a_buf]),%[a_buf]                             \n"

// Read 8 UV from 422, upsample to 16 UV.  With 16 Alpha.
#define READYUVA422_AVX2                                              \
  "vmovq      (%[u_buf]),%%xmm3                                   \n" \
  "vmovq      0x00(%[u_buf],%[v_buf],1),%%xmm1                    \n" \
  "lea        0x8(%[u_buf]),%[u_buf]                              \n" \
  "vpunpcklbw %%ymm1,%%ymm3,%%ymm3                                \n" \
  "vpermq     $0xd8,%%ymm3,%%ymm3                                 \n" \
  "vpunpcklwd %%ymm3,%%ymm3,%%ymm3                                \n" \
  "vmovdqu    (%[y_buf]),%%xmm4                                   \n" \
  "vpermq     $0xd8,%%ymm4,%%ymm4                                 \n" \
  "vpunpcklbw %%ymm4,%%ymm4,%%ymm4                                \n" \
  "lea        0x10(%[y_buf]),%[y_buf]                             \n" \
  "vmovdqu    (%[a_buf]),%%xmm5                                   \n" \
  "vpermq     $0xd8,%%ymm5,%%ymm5                                 \n" \
  "lea        0x10(%[a_buf]),%[a_buf]                             \n"

// Read 8 UYVY with 16 Y and upsample 8 UV to 16 UV.
#define READUYVY_AVX2                                                 \
  "vmovdqu    (%[uyvy_buf]),%%ymm4                                \n" \
  "vpshufb    %[kShuffleUYVYY], %%ymm4, %%ymm4                    \n" \
  "vmovdqu    (%[uyvy_buf]),%%ymm3                                \n" \
  "vpshufb    %[kShuffleUYVYUV], %%ymm3, %%ymm3                   \n" \
  "lea        0x20(%[uyvy_buf]),%[uyvy_buf]                       \n"

#if defined(__x86_64__)
#define YUVTORGB_SETUP_AVX2(yuvconstants)                             \
  "vpcmpeqb    %%xmm13,%%xmm13,%%xmm13                            \n" \
  "vmovdqa     (%[yuvconstants]),%%ymm8                           \n" \
  "vpsllw      $7,%%xmm13,%%xmm13                                 \n" \
  "vmovdqa     32(%[yuvconstants]),%%ymm9                         \n" \
  "vpbroadcastb %%xmm13,%%ymm13                                   \n" \
  "vmovdqa     64(%[yuvconstants]),%%ymm10                        \n" \
  "vmovdqa     96(%[yuvconstants]),%%ymm11                        \n" \
  "vmovdqa     128(%[yuvconstants]),%%ymm12                       \n"

#define YUVTORGB16_AVX2(yuvconstants)                                 \
  "vpsubb      %%ymm13,%%ymm3,%%ymm3                              \n" \
  "vpmulhuw    %%ymm11,%%ymm4,%%ymm4                              \n" \
  "vpmaddubsw  %%ymm3,%%ymm8,%%ymm0                               \n" \
  "vpmaddubsw  %%ymm3,%%ymm9,%%ymm1                               \n" \
  "vpmaddubsw  %%ymm3,%%ymm10,%%ymm2                              \n" \
  "vpaddw      %%ymm4,%%ymm12,%%ymm4                              \n" \
  "vpaddsw     %%ymm4,%%ymm0,%%ymm0                               \n" \
  "vpsubsw     %%ymm1,%%ymm4,%%ymm1                               \n" \
  "vpaddsw     %%ymm4,%%ymm2,%%ymm2                               \n"

#define YUVTORGB_REGS_AVX2 "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13",

#else  // Convert 16 pixels: 16 UV and 16 Y.

#define YUVTORGB_SETUP_AVX2(yuvconstants)
#define YUVTORGB16_AVX2(yuvconstants)                                 \
  "vpcmpeqb    %%xmm0,%%xmm0,%%xmm0                               \n" \
  "vpsllw      $7,%%xmm0,%%xmm0                                   \n" \
  "vpbroadcastb %%xmm0,%%ymm0                                     \n" \
  "vpsubb      %%ymm0,%%ymm3,%%ymm3                               \n" \
  "vpmulhuw    96(%[yuvconstants]),%%ymm4,%%ymm4                  \n" \
  "vmovdqa     (%[yuvconstants]),%%ymm0                           \n" \
  "vmovdqa     32(%[yuvconstants]),%%ymm1                         \n" \
  "vmovdqa     64(%[yuvconstants]),%%ymm2                         \n" \
  "vpmaddubsw  %%ymm3,%%ymm0,%%ymm0                               \n" \
  "vpmaddubsw  %%ymm3,%%ymm1,%%ymm1                               \n" \
  "vpmaddubsw  %%ymm3,%%ymm2,%%ymm2                               \n" \
  "vmovdqa     128(%[yuvconstants]),%%ymm3                        \n" \
  "vpaddw      %%ymm4,%%ymm3,%%ymm4                               \n" \
  "vpaddsw     %%ymm4,%%ymm0,%%ymm0                               \n" \
  "vpsubsw     %%ymm1,%%ymm4,%%ymm1                               \n" \
  "vpaddsw     %%ymm4,%%ymm2,%%ymm2                               \n"

#define YUVTORGB_REGS_AVX2
#endif

#define YUVTORGB_AVX2(yuvconstants)                                   \
  YUVTORGB16_AVX2(yuvconstants)                                       \
  "vpsraw      $0x6,%%ymm0,%%ymm0                                 \n" \
  "vpsraw      $0x6,%%ymm1,%%ymm1                                 \n" \
  "vpsraw      $0x6,%%ymm2,%%ymm2                                 \n" \
  "vpackuswb   %%ymm0,%%ymm0,%%ymm0                               \n" \
  "vpackuswb   %%ymm1,%%ymm1,%%ymm1                               \n" \
  "vpackuswb   %%ymm2,%%ymm2,%%ymm2                               \n"

// Store 16 ARGB values.
#define STOREARGB_AVX2                                                \
  "vpunpcklbw %%ymm1,%%ymm0,%%ymm0                                \n" \
  "vpermq     $0xd8,%%ymm0,%%ymm0                                 \n" \
  "vpunpcklbw %%ymm5,%%ymm2,%%ymm2                                \n" \
  "vpermq     $0xd8,%%ymm2,%%ymm2                                 \n" \
  "vpunpcklwd %%ymm2,%%ymm0,%%ymm1                                \n" \
  "vpunpckhwd %%ymm2,%%ymm0,%%ymm0                                \n" \
  "vmovdqu    %%ymm1,(%[dst_argb])                                \n" \
  "vmovdqu    %%ymm0,0x20(%[dst_argb])                            \n" \
  "lea        0x40(%[dst_argb]), %[dst_argb]                      \n"

#if defined(HAS_I422TOARGBROW_AVX2)
// 16 pixels
// 8 UV values upsampled to 16 UV, mixed with 16 Y producing 16 ARGB (64 bytes).
void OMITFP I422ToARGBRow_AVX2(const uint8_t* y_buf,
                               const uint8_t* u_buf,
                               const uint8_t* v_buf,
                               uint8_t* dst_argb,
                               const struct YuvConstants* yuvconstants,
                               int width) {
  asm volatile (
    YUVTORGB_SETUP_AVX2(yuvconstants)
      "sub         %[u_buf],%[v_buf]             \n"
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"

    LABELALIGN
      "1:                                        \n"
    READYUV422_AVX2
    YUVTORGB_AVX2(yuvconstants)
    STOREARGB_AVX2
      "sub         $0x10,%[width]                \n"
      "jg          1b                            \n"

      "vzeroupper                                \n"
  : [y_buf]"+r"(y_buf),    // %[y_buf]
    [u_buf]"+r"(u_buf),    // %[u_buf]
    [v_buf]"+r"(v_buf),    // %[v_buf]
    [dst_argb]"+r"(dst_argb),  // %[dst_argb]
    [width]"+rm"(width)    // %[width]
  : [yuvconstants]"r"(yuvconstants)  // %[yuvconstants]
  : "memory", "cc", YUVTORGB_REGS_AVX2
    "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5"
  );
}
#endif  // HAS_I422TOARGBROW_AVX2

#if defined(HAS_I422ALPHATOARGBROW_AVX2)
// 16 pixels
// 8 UV values upsampled to 16 UV, mixed with 16 Y and 16 A producing 16 ARGB.
void OMITFP I422AlphaToARGBRow_AVX2(const uint8_t* y_buf,
                                    const uint8_t* u_buf,
                                    const uint8_t* v_buf,
                                    const uint8_t* a_buf,
                                    uint8_t* dst_argb,
                                    const struct YuvConstants* yuvconstants,
                                    int width) {
  // clang-format off
  asm volatile (
    YUVTORGB_SETUP_AVX2(yuvconstants)
      "sub         %[u_buf],%[v_buf]             \n"

    LABELALIGN
      "1:                                        \n"
    READYUVA422_AVX2
    YUVTORGB_AVX2(yuvconstants)
    STOREARGB_AVX2
      "subl        $0x10,%[width]                \n"
      "jg          1b                            \n"
      "vzeroupper                                \n"
  : [y_buf]"+r"(y_buf),    // %[y_buf]
    [u_buf]"+r"(u_buf),    // %[u_buf]
    [v_buf]"+r"(v_buf),    // %[v_buf]
    [a_buf]"+r"(a_buf),    // %[a_buf]
    [dst_argb]"+r"(dst_argb),  // %[dst_argb]
#if defined(__i386__)
    [width]"+m"(width)     // %[width]
#else
    [width]"+rm"(width)    // %[width]
#endif
  : [yuvconstants]"r"(yuvconstants)  // %[yuvconstants]
  : "memory", "cc", YUVTORGB_REGS_AVX2
    "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5"
  );
  // clang-format on
}
#endif  // HAS_I422ALPHATOARGBROW_AVX2

#if defined(HAS_I422TORGBAROW_AVX2)
// 16 pixels
// 8 UV values upsampled to 16 UV, mixed with 16 Y producing 16 RGBA (64 bytes).
void OMITFP I422ToRGBARow_AVX2(const uint8_t* y_buf,
                               const uint8_t* u_buf,
                               const uint8_t* v_buf,
                               uint8_t* dst_argb,
                               const struct YuvConstants* yuvconstants,
                               int width) {
  asm volatile (
    YUVTORGB_SETUP_AVX2(yuvconstants)
      "sub         %[u_buf],%[v_buf]             \n"
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"

    LABELALIGN
      "1:                                        \n"
    READYUV422_AVX2
    YUVTORGB_AVX2(yuvconstants)

    // Step 3: Weave into RGBA
    "vpunpcklbw %%ymm2,%%ymm1,%%ymm1           \n"
    "vpermq     $0xd8,%%ymm1,%%ymm1            \n"
    "vpunpcklbw %%ymm0,%%ymm5,%%ymm2           \n"
    "vpermq     $0xd8,%%ymm2,%%ymm2            \n"
    "vpunpcklwd %%ymm1,%%ymm2,%%ymm0           \n"
    "vpunpckhwd %%ymm1,%%ymm2,%%ymm1           \n"
    "vmovdqu    %%ymm0,(%[dst_argb])           \n"
    "vmovdqu    %%ymm1,0x20(%[dst_argb])       \n"
    "lea        0x40(%[dst_argb]),%[dst_argb]  \n"
    "sub        $0x10,%[width]                 \n"
    "jg         1b                             \n"
    "vzeroupper                                \n"
  : [y_buf]"+r"(y_buf),    // %[y_buf]
    [u_buf]"+r"(u_buf),    // %[u_buf]
    [v_buf]"+r"(v_buf),    // %[v_buf]
    [dst_argb]"+r"(dst_argb),  // %[dst_argb]
    [width]"+rm"(width)    // %[width]
  : [yuvconstants]"r"(yuvconstants)  // %[yuvconstants]
  : "memory", "cc", YUVTORGB_REGS_AVX2
    "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5"
  );
}
#endif  // HAS_I422TORGBAROW_AVX2

#if defined(HAS_UYVYTOARGBROW_AVX2)
// 16 pixels.
// 8 UYVY values with 16 Y and 8 UV producing 16 ARGB (64 bytes).
void OMITFP UYVYToARGBRow_AVX2(const uint8_t* uyvy_buf,
                               uint8_t* dst_argb,
                               const struct YuvConstants* yuvconstants,
                               int width) {
  // clang-format off
  asm volatile (
    YUVTORGB_SETUP_AVX2(yuvconstants)
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"

    LABELALIGN
      "1:                                        \n"
    READUYVY_AVX2
    YUVTORGB_AVX2(yuvconstants)
    STOREARGB_AVX2
      "sub         $0x10,%[width]                \n"
      "jg          1b                            \n"
      "vzeroupper                                \n"
  : [uyvy_buf]"+r"(uyvy_buf),    // %[uyvy_buf]
    [dst_argb]"+r"(dst_argb),  // %[dst_argb]
    [width]"+rm"(width)    // %[width]
  : [yuvconstants]"r"(yuvconstants), // %[yuvconstants]
    [kShuffleUYVYY]"m"(kShuffleUYVYY),
    [kShuffleUYVYUV]"m"(kShuffleUYVYUV)
    : "memory", "cc", YUVTORGB_REGS_AVX2
      "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5"
  );
  // clang-format on
}
#endif  // HAS_UYVYTOARGBROW_AVX2

#ifdef HAS_COPYROW_SSE2
void CopyRow_SSE2(const uint8_t* src, uint8_t* dst, int width) {
  asm volatile(
      "test        $0xf,%0                       \n"
      "jne         2f                            \n"
      "test        $0xf,%1                       \n"
      "jne         2f                            \n"

      LABELALIGN
      "1:                                        \n"
      "movdqa      (%0),%%xmm0                   \n"
      "movdqa      0x10(%0),%%xmm1               \n"
      "lea         0x20(%0),%0                   \n"
      "movdqa      %%xmm0,(%1)                   \n"
      "movdqa      %%xmm1,0x10(%1)               \n"
      "lea         0x20(%1),%1                   \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "jmp         9f                            \n"

      LABELALIGN
      "2:                                        \n"
      "movdqu      (%0),%%xmm0                   \n"
      "movdqu      0x10(%0),%%xmm1               \n"
      "lea         0x20(%0),%0                   \n"
      "movdqu      %%xmm0,(%1)                   \n"
      "movdqu      %%xmm1,0x10(%1)               \n"
      "lea         0x20(%1),%1                   \n"
      "sub         $0x20,%2                      \n"
      "jg          2b                            \n"

      LABELALIGN "9:                                        \n"
      : "+r"(src),   // %0
        "+r"(dst),   // %1
        "+r"(width)  // %2
      :
      : "memory", "cc", "xmm0", "xmm1");
}
#endif  // HAS_COPYROW_SSE2

#ifdef HAS_COPYROW_AVX
void CopyRow_AVX(const uint8_t* src, uint8_t* dst, int width) {
  asm volatile(

      LABELALIGN
      "1:                                        \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "lea         0x40(%0),%0                   \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "vmovdqu     %%ymm1,0x20(%1)               \n"
      "lea         0x40(%1),%1                   \n"
      "sub         $0x40,%2                      \n"
      "jg          1b                            \n"
      : "+r"(src),   // %0
        "+r"(dst),   // %1
        "+r"(width)  // %2
      :
      : "memory", "cc", "xmm0", "xmm1");
}
#endif  // HAS_COPYROW_AVX

#ifdef HAS_COPYROW_ERMS
// Multiple of 1.
void CopyRow_ERMS(const uint8_t* src, uint8_t* dst, int width) {
  size_t width_tmp = (size_t)(width);
  asm volatile(

      "rep         movsb                         \n"
      : "+S"(src),       // %0
        "+D"(dst),       // %1
        "+c"(width_tmp)  // %2
      :
      : "memory", "cc");
}
#endif  // HAS_COPYROW_ERMS


#ifdef HAS_YUY2TOYROW_SSE2
void UYVYToYRow_SSE2(const uint8_t* src_uyvy, uint8_t* dst_y, int width) {
  asm volatile(

      LABELALIGN
      "1:                                        \n"
      "movdqu      (%0),%%xmm0                   \n"
      "movdqu      0x10(%0),%%xmm1               \n"
      "lea         0x20(%0),%0                   \n"
      "psrlw       $0x8,%%xmm0                   \n"
      "psrlw       $0x8,%%xmm1                   \n"
      "packuswb    %%xmm1,%%xmm0                 \n"
      "movdqu      %%xmm0,(%1)                   \n"
      "lea         0x10(%1),%1                   \n"
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"
      : "+r"(src_uyvy),  // %0
        "+r"(dst_y),     // %1
        "+r"(width)      // %2
      :
      : "memory", "cc", "xmm0", "xmm1");
}

void UYVYToUV422Row_SSE2(const uint8_t* src_uyvy,
                         uint8_t* dst_u,
                         uint8_t* dst_v,
                         int width) {
  asm volatile(
      "pcmpeqb     %%xmm5,%%xmm5                 \n"
      "psrlw       $0x8,%%xmm5                   \n"
      "sub         %1,%2                         \n"

      LABELALIGN
      "1:                                        \n"
      "movdqu      (%0),%%xmm0                   \n"
      "movdqu      0x10(%0),%%xmm1               \n"
      "lea         0x20(%0),%0                   \n"
      "pand        %%xmm5,%%xmm0                 \n"
      "pand        %%xmm5,%%xmm1                 \n"
      "packuswb    %%xmm1,%%xmm0                 \n"
      "movdqa      %%xmm0,%%xmm1                 \n"
      "pand        %%xmm5,%%xmm0                 \n"
      "packuswb    %%xmm0,%%xmm0                 \n"
      "psrlw       $0x8,%%xmm1                   \n"
      "packuswb    %%xmm1,%%xmm1                 \n"
      "movq        %%xmm0,(%1)                   \n"
      "movq        %%xmm1,0x00(%1,%2,1)          \n"
      "lea         0x8(%1),%1                    \n"
      "sub         $0x10,%3                      \n"
      "jg          1b                            \n"
      : "+r"(src_uyvy),  // %0
        "+r"(dst_u),     // %1
        "+r"(dst_v),     // %2
        "+r"(width)      // %3
      :
      : "memory", "cc", "xmm0", "xmm1", "xmm5");
}
#endif  // HAS_YUY2TOYROW_SSE2

#ifdef HAS_YUY2TOYROW_AVX2

void UYVYToYRow_AVX2(const uint8_t* src_uyvy, uint8_t* dst_y, int width) {
  asm volatile(

      LABELALIGN
      "1:                                        \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "lea         0x40(%0),%0                   \n"
      "vpsrlw      $0x8,%%ymm0,%%ymm0            \n"
      "vpsrlw      $0x8,%%ymm1,%%ymm1            \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "lea         0x20(%1),%1                   \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper                                \n"
      : "+r"(src_uyvy),  // %0
        "+r"(dst_y),     // %1
        "+r"(width)      // %2
      :
      : "memory", "cc", "xmm0", "xmm1", "xmm5");
}

void UYVYToUV422Row_AVX2(const uint8_t* src_uyvy,
                         uint8_t* dst_u,
                         uint8_t* dst_v,
                         int width) {
  asm volatile(
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"
      "vpsrlw      $0x8,%%ymm5,%%ymm5            \n"
      "sub         %1,%2                         \n"

      LABELALIGN
      "1:                                        \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "lea         0x40(%0),%0                   \n"
      "vpand       %%ymm5,%%ymm0,%%ymm0          \n"
      "vpand       %%ymm5,%%ymm1,%%ymm1          \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vpand       %%ymm5,%%ymm0,%%ymm1          \n"
      "vpsrlw      $0x8,%%ymm0,%%ymm0            \n"
      "vpackuswb   %%ymm1,%%ymm1,%%ymm1          \n"
      "vpackuswb   %%ymm0,%%ymm0,%%ymm0          \n"
      "vpermq      $0xd8,%%ymm1,%%ymm1           \n"
      "vpermq      $0xd8,%%ymm0,%%ymm0           \n"
      "vextractf128 $0x0,%%ymm1,(%1)             \n"
      "vextractf128 $0x0,%%ymm0,0x00(%1,%2,1)    \n"
      "lea         0x10(%1),%1                   \n"
      "sub         $0x20,%3                      \n"
      "jg          1b                            \n"
      "vzeroupper                                \n"
      : "+r"(src_uyvy),  // %0
        "+r"(dst_u),     // %1
        "+r"(dst_v),     // %2
        "+r"(width)      // %3
      :
      : "memory", "cc", "xmm0", "xmm1", "xmm5");
}
#endif  // HAS_BLENDPLANEROW_AVX2

#ifdef HAS_ARGBATTENUATEROW_SSSE3
// Shuffle table duplicating alpha.
static const uvec8 kShuffleAlpha0 = {3u, 3u, 3u, 3u, 3u, 3u, 128u, 128u,
                                     7u, 7u, 7u, 7u, 7u, 7u, 128u, 128u};
static const uvec8 kShuffleAlpha1 = {11u, 11u, 11u, 11u, 11u, 11u, 128u, 128u,
                                     15u, 15u, 15u, 15u, 15u, 15u, 128u, 128u};
// Attenuate 4 pixels at a time.
void ARGBAttenuateRow_SSSE3(const uint8_t* src_argb,
                            uint8_t* dst_argb,
                            int width) {
  asm volatile(
      "pcmpeqb     %%xmm3,%%xmm3                 \n"
      "pslld       $0x18,%%xmm3                  \n"
      "movdqa      %3,%%xmm4                     \n"
      "movdqa      %4,%%xmm5                     \n"

      // 4 pixel loop.
      LABELALIGN
      "1:                                        \n"
      "movdqu      (%0),%%xmm0                   \n"
      "pshufb      %%xmm4,%%xmm0                 \n"
      "movdqu      (%0),%%xmm1                   \n"
      "punpcklbw   %%xmm1,%%xmm1                 \n"
      "pmulhuw     %%xmm1,%%xmm0                 \n"
      "movdqu      (%0),%%xmm1                   \n"
      "pshufb      %%xmm5,%%xmm1                 \n"
      "movdqu      (%0),%%xmm2                   \n"
      "punpckhbw   %%xmm2,%%xmm2                 \n"
      "pmulhuw     %%xmm2,%%xmm1                 \n"
      "movdqu      (%0),%%xmm2                   \n"
      "lea         0x10(%0),%0                   \n"
      "pand        %%xmm3,%%xmm2                 \n"
      "psrlw       $0x8,%%xmm0                   \n"
      "psrlw       $0x8,%%xmm1                   \n"
      "packuswb    %%xmm1,%%xmm0                 \n"
      "por         %%xmm2,%%xmm0                 \n"
      "movdqu      %%xmm0,(%1)                   \n"
      "lea         0x10(%1),%1                   \n"
      "sub         $0x4,%2                       \n"
      "jg          1b                            \n"
      : "+r"(src_argb),       // %0
        "+r"(dst_argb),       // %1
        "+r"(width)           // %2
      : "m"(kShuffleAlpha0),  // %3
        "m"(kShuffleAlpha1)   // %4
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5");
}
#endif  // HAS_ARGBATTENUATEROW_SSSE3

#ifdef HAS_ARGBATTENUATEROW_AVX2
// Shuffle table duplicating alpha.
static const uvec8 kShuffleAlpha_AVX2 = {6u,   7u,   6u,   7u,  6u,  7u,
                                         128u, 128u, 14u,  15u, 14u, 15u,
                                         14u,  15u,  128u, 128u};
// Attenuate 8 pixels at a time.
void ARGBAttenuateRow_AVX2(const uint8_t* src_argb,
                           uint8_t* dst_argb,
                           int width) {
  asm volatile(
      "vbroadcastf128 %3,%%ymm4                  \n"
      "vpcmpeqb    %%ymm5,%%ymm5,%%ymm5          \n"
      "vpslld      $0x18,%%ymm5,%%ymm5           \n"
      "sub         %0,%1                         \n"

      // 8 pixel loop.
      LABELALIGN
      "1:                                        \n"
      "vmovdqu     (%0),%%ymm6                   \n"
      "vpunpcklbw  %%ymm6,%%ymm6,%%ymm0          \n"
      "vpunpckhbw  %%ymm6,%%ymm6,%%ymm1          \n"
      "vpshufb     %%ymm4,%%ymm0,%%ymm2          \n"
      "vpshufb     %%ymm4,%%ymm1,%%ymm3          \n"
      "vpmulhuw    %%ymm2,%%ymm0,%%ymm0          \n"
      "vpmulhuw    %%ymm3,%%ymm1,%%ymm1          \n"
      "vpand       %%ymm5,%%ymm6,%%ymm6          \n"
      "vpsrlw      $0x8,%%ymm0,%%ymm0            \n"
      "vpsrlw      $0x8,%%ymm1,%%ymm1            \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"
      "vpor        %%ymm6,%%ymm0,%%ymm0          \n"
      "vmovdqu     %%ymm0,0x00(%0,%1,1)          \n"
      "lea         0x20(%0),%0                   \n"
      "sub         $0x8,%2                       \n"
      "jg          1b                            \n"
      "vzeroupper                                \n"
      : "+r"(src_argb),          // %0
        "+r"(dst_argb),          // %1
        "+r"(width)              // %2
      : "m"(kShuffleAlpha_AVX2)  // %3
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6");
}
#endif  // HAS_ARGBATTENUATEROW_AVX2

#ifdef HAS_ARGBUNATTENUATEROW_SSE2
// Unattenuate 4 pixels at a time.
void ARGBUnattenuateRow_SSE2(const uint8_t* src_argb,
                             uint8_t* dst_argb,
                             int width) {
  uintptr_t alpha;
  asm volatile(
      // 4 pixel loop.
      LABELALIGN
      "1:                                        \n"
      "movdqu      (%0),%%xmm0                   \n"
      "movzb       0x03(%0),%3                   \n"
      "punpcklbw   %%xmm0,%%xmm0                 \n"
      "movd        0x00(%4,%3,4),%%xmm2          \n"
      "movzb       0x07(%0),%3                   \n"
      "movd        0x00(%4,%3,4),%%xmm3          \n"
      "pshuflw     $0x40,%%xmm2,%%xmm2           \n"
      "pshuflw     $0x40,%%xmm3,%%xmm3           \n"
      "movlhps     %%xmm3,%%xmm2                 \n"
      "pmulhuw     %%xmm2,%%xmm0                 \n"
      "movdqu      (%0),%%xmm1                   \n"
      "movzb       0x0b(%0),%3                   \n"
      "punpckhbw   %%xmm1,%%xmm1                 \n"
      "movd        0x00(%4,%3,4),%%xmm2          \n"
      "movzb       0x0f(%0),%3                   \n"
      "movd        0x00(%4,%3,4),%%xmm3          \n"
      "pshuflw     $0x40,%%xmm2,%%xmm2           \n"
      "pshuflw     $0x40,%%xmm3,%%xmm3           \n"
      "movlhps     %%xmm3,%%xmm2                 \n"
      "pmulhuw     %%xmm2,%%xmm1                 \n"
      "lea         0x10(%0),%0                   \n"
      "packuswb    %%xmm1,%%xmm0                 \n"
      "movdqu      %%xmm0,(%1)                   \n"
      "lea         0x10(%1),%1                   \n"
      "sub         $0x4,%2                       \n"
      "jg          1b                            \n"
      : "+r"(src_argb),     // %0
        "+r"(dst_argb),     // %1
        "+r"(width),        // %2
        "=&r"(alpha)        // %3
      : "r"(fixed_invtbl8)  // %4
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5");
}
#endif  // HAS_ARGBUNATTENUATEROW_SSE2

#ifdef HAS_ARGBUNATTENUATEROW_AVX2
// Shuffle table duplicating alpha.
static const uvec8 kUnattenShuffleAlpha_AVX2 = {
    0u, 1u, 0u, 1u, 0u, 1u, 6u, 7u, 8u, 9u, 8u, 9u, 8u, 9u, 14u, 15u};
// Unattenuate 8 pixels at a time.
void ARGBUnattenuateRow_AVX2(const uint8_t* src_argb,
                             uint8_t* dst_argb,
                             int width) {
  uintptr_t alpha;
  asm volatile(
      "sub         %0,%1                         \n"
      "vbroadcastf128 %5,%%ymm5                  \n"

      // 8 pixel loop.
      LABELALIGN
      "1:                                        \n"
      // replace VPGATHER
      "movzb       0x03(%0),%3                   \n"
      "vmovd       0x00(%4,%3,4),%%xmm0          \n"
      "movzb       0x07(%0),%3                   \n"
      "vmovd       0x00(%4,%3,4),%%xmm1          \n"
      "movzb       0x0b(%0),%3                   \n"
      "vpunpckldq  %%xmm1,%%xmm0,%%xmm6          \n"
      "vmovd       0x00(%4,%3,4),%%xmm2          \n"
      "movzb       0x0f(%0),%3                   \n"
      "vmovd       0x00(%4,%3,4),%%xmm3          \n"
      "movzb       0x13(%0),%3                   \n"
      "vpunpckldq  %%xmm3,%%xmm2,%%xmm7          \n"
      "vmovd       0x00(%4,%3,4),%%xmm0          \n"
      "movzb       0x17(%0),%3                   \n"
      "vmovd       0x00(%4,%3,4),%%xmm1          \n"
      "movzb       0x1b(%0),%3                   \n"
      "vpunpckldq  %%xmm1,%%xmm0,%%xmm0          \n"
      "vmovd       0x00(%4,%3,4),%%xmm2          \n"
      "movzb       0x1f(%0),%3                   \n"
      "vmovd       0x00(%4,%3,4),%%xmm3          \n"
      "vpunpckldq  %%xmm3,%%xmm2,%%xmm2          \n"
      "vpunpcklqdq %%xmm7,%%xmm6,%%xmm3          \n"
      "vpunpcklqdq %%xmm2,%%xmm0,%%xmm0          \n"
      "vinserti128 $0x1,%%xmm0,%%ymm3,%%ymm3     \n"
      // end of VPGATHER

      "vmovdqu     (%0),%%ymm6                   \n"
      "vpunpcklbw  %%ymm6,%%ymm6,%%ymm0          \n"
      "vpunpckhbw  %%ymm6,%%ymm6,%%ymm1          \n"
      "vpunpcklwd  %%ymm3,%%ymm3,%%ymm2          \n"
      "vpunpckhwd  %%ymm3,%%ymm3,%%ymm3          \n"
      "vpshufb     %%ymm5,%%ymm2,%%ymm2          \n"
      "vpshufb     %%ymm5,%%ymm3,%%ymm3          \n"
      "vpmulhuw    %%ymm2,%%ymm0,%%ymm0          \n"
      "vpmulhuw    %%ymm3,%%ymm1,%%ymm1          \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"
      "vmovdqu     %%ymm0,0x00(%0,%1,1)          \n"
      "lea         0x20(%0),%0                   \n"
      "sub         $0x8,%2                       \n"
      "jg          1b                            \n"
      "vzeroupper                                \n"
      : "+r"(src_argb),                 // %0
        "+r"(dst_argb),                 // %1
        "+r"(width),                    // %2
        "=&r"(alpha)                    // %3
      : "r"(fixed_invtbl8),             // %4
        "m"(kUnattenShuffleAlpha_AVX2)  // %5
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",
        "xmm7");
}
#endif  // HAS_ARGBUNATTENUATEROW_AVX2

#ifdef HAS_INTERPOLATEROW_SSSE3
// Bilinear filter 16x2 -> 16x1
void InterpolateRow_SSSE3(uint8_t* dst_ptr,
                          const uint8_t* src_ptr,
                          ptrdiff_t src_stride,
                          int dst_width,
                          int source_y_fraction) {
  asm volatile(
      "sub         %1,%0                         \n"
      "cmp         $0x0,%3                       \n"
      "je          100f                          \n"
      "cmp         $0x80,%3                      \n"
      "je          50f                           \n"

      "movd        %3,%%xmm0                     \n"
      "neg         %3                            \n"
      "add         $0x100,%3                     \n"
      "movd        %3,%%xmm5                     \n"
      "punpcklbw   %%xmm0,%%xmm5                 \n"
      "punpcklwd   %%xmm5,%%xmm5                 \n"
      "pshufd      $0x0,%%xmm5,%%xmm5            \n"
      "mov         $0x80808080,%%eax             \n"
      "movd        %%eax,%%xmm4                  \n"
      "pshufd      $0x0,%%xmm4,%%xmm4            \n"

      // General purpose row blend.
      LABELALIGN
      "1:                                        \n"
      "movdqu      (%1),%%xmm0                   \n"
      "movdqu      0x00(%1,%4,1),%%xmm2          \n"
      "movdqa      %%xmm0,%%xmm1                 \n"
      "punpcklbw   %%xmm2,%%xmm0                 \n"
      "punpckhbw   %%xmm2,%%xmm1                 \n"
      "psubb       %%xmm4,%%xmm0                 \n"
      "psubb       %%xmm4,%%xmm1                 \n"
      "movdqa      %%xmm5,%%xmm2                 \n"
      "movdqa      %%xmm5,%%xmm3                 \n"
      "pmaddubsw   %%xmm0,%%xmm2                 \n"
      "pmaddubsw   %%xmm1,%%xmm3                 \n"
      "paddw       %%xmm4,%%xmm2                 \n"
      "paddw       %%xmm4,%%xmm3                 \n"
      "psrlw       $0x8,%%xmm2                   \n"
      "psrlw       $0x8,%%xmm3                   \n"
      "packuswb    %%xmm3,%%xmm2                 \n"
      "movdqu      %%xmm2,0x00(%1,%0,1)          \n"
      "lea         0x10(%1),%1                   \n"
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"
      "jmp         99f                           \n"

      // Blend 50 / 50.
      LABELALIGN
      "50:                                       \n"
      "movdqu      (%1),%%xmm0                   \n"
      "movdqu      0x00(%1,%4,1),%%xmm1          \n"
      "pavgb       %%xmm1,%%xmm0                 \n"
      "movdqu      %%xmm0,0x00(%1,%0,1)          \n"
      "lea         0x10(%1),%1                   \n"
      "sub         $0x10,%2                      \n"
      "jg          50b                           \n"
      "jmp         99f                           \n"

      // Blend 100 / 0 - Copy row unchanged.
      LABELALIGN
      "100:                                      \n"
      "movdqu      (%1),%%xmm0                   \n"
      "movdqu      %%xmm0,0x00(%1,%0,1)          \n"
      "lea         0x10(%1),%1                   \n"
      "sub         $0x10,%2                      \n"
      "jg          100b                          \n"

      "99:                                       \n"
      : "+r"(dst_ptr),               // %0
        "+r"(src_ptr),               // %1
        "+rm"(dst_width),            // %2
        "+r"(source_y_fraction)      // %3
      : "r"((intptr_t)(src_stride))  // %4
      : "memory", "cc", "eax", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5");
}
#endif  // HAS_INTERPOLATEROW_SSSE3

#ifdef HAS_INTERPOLATEROW_AVX2
// Bilinear filter 32x2 -> 32x1
void InterpolateRow_AVX2(uint8_t* dst_ptr,
                         const uint8_t* src_ptr,
                         ptrdiff_t src_stride,
                         int dst_width,
                         int source_y_fraction) {
  asm volatile(
      "cmp         $0x0,%3                       \n"
      "je          100f                          \n"
      "sub         %1,%0                         \n"
      "cmp         $0x80,%3                      \n"
      "je          50f                           \n"

      "vmovd       %3,%%xmm0                     \n"
      "neg         %3                            \n"
      "add         $0x100,%3                     \n"
      "vmovd       %3,%%xmm5                     \n"
      "vpunpcklbw  %%xmm0,%%xmm5,%%xmm5          \n"
      "vpunpcklwd  %%xmm5,%%xmm5,%%xmm5          \n"
      "vbroadcastss %%xmm5,%%ymm5                \n"
      "mov         $0x80808080,%%eax             \n"
      "vmovd       %%eax,%%xmm4                  \n"
      "vbroadcastss %%xmm4,%%ymm4                \n"

      // General purpose row blend.
      LABELALIGN
      "1:                                        \n"
      "vmovdqu     (%1),%%ymm0                   \n"
      "vmovdqu     0x00(%1,%4,1),%%ymm2          \n"
      "vpunpckhbw  %%ymm2,%%ymm0,%%ymm1          \n"
      "vpunpcklbw  %%ymm2,%%ymm0,%%ymm0          \n"
      "vpsubb      %%ymm4,%%ymm1,%%ymm1          \n"
      "vpsubb      %%ymm4,%%ymm0,%%ymm0          \n"
      "vpmaddubsw  %%ymm1,%%ymm5,%%ymm1          \n"
      "vpmaddubsw  %%ymm0,%%ymm5,%%ymm0          \n"
      "vpaddw      %%ymm4,%%ymm1,%%ymm1          \n"
      "vpaddw      %%ymm4,%%ymm0,%%ymm0          \n"
      "vpsrlw      $0x8,%%ymm1,%%ymm1            \n"
      "vpsrlw      $0x8,%%ymm0,%%ymm0            \n"
      "vpackuswb   %%ymm1,%%ymm0,%%ymm0          \n"
      "vmovdqu     %%ymm0,0x00(%1,%0,1)          \n"
      "lea         0x20(%1),%1                   \n"
      "sub         $0x20,%2                      \n"
      "jg          1b                            \n"
      "jmp         99f                           \n"

      // Blend 50 / 50.
      LABELALIGN
      "50:                                       \n"
      "vmovdqu     (%1),%%ymm0                   \n"
      "vpavgb      0x00(%1,%4,1),%%ymm0,%%ymm0   \n"
      "vmovdqu     %%ymm0,0x00(%1,%0,1)          \n"
      "lea         0x20(%1),%1                   \n"
      "sub         $0x20,%2                      \n"
      "jg          50b                           \n"
      "jmp         99f                           \n"

      // Blend 100 / 0 - Copy row unchanged.
      LABELALIGN
      "100:                                      \n"
      "rep         movsb                         \n"
      "jmp         999f                          \n"

      "99:                                       \n"
      "vzeroupper                                \n"
      "999:                                      \n"
      : "+D"(dst_ptr),               // %0
        "+S"(src_ptr),               // %1
        "+cm"(dst_width),            // %2
        "+r"(source_y_fraction)      // %3
      : "r"((intptr_t)(src_stride))  // %4
      : "memory", "cc", "eax", "xmm0", "xmm1", "xmm2", "xmm4", "xmm5");
}
#endif  // HAS_INTERPOLATEROW_AVX2

#ifdef HAS_ARGBSHUFFLEROW_SSSE3
// For BGRAToARGB, ABGRToARGB, RGBAToARGB, and ARGBToRGBA.
void ARGBShuffleRow_SSSE3(const uint8_t* src_argb,
                          uint8_t* dst_argb,
                          const uint8_t* shuffler,
                          int width) {
  asm volatile(

      "movdqu      (%3),%%xmm5                   \n"

      LABELALIGN
      "1:                                        \n"
      "movdqu      (%0),%%xmm0                   \n"
      "movdqu      0x10(%0),%%xmm1               \n"
      "lea         0x20(%0),%0                   \n"
      "pshufb      %%xmm5,%%xmm0                 \n"
      "pshufb      %%xmm5,%%xmm1                 \n"
      "movdqu      %%xmm0,(%1)                   \n"
      "movdqu      %%xmm1,0x10(%1)               \n"
      "lea         0x20(%1),%1                   \n"
      "sub         $0x8,%2                       \n"
      "jg          1b                            \n"
      : "+r"(src_argb),  // %0
        "+r"(dst_argb),  // %1
        "+r"(width)      // %2
      : "r"(shuffler)    // %3
      : "memory", "cc", "xmm0", "xmm1", "xmm5");
}
#endif  // HAS_ARGBSHUFFLEROW_SSSE3

#ifdef HAS_ARGBSHUFFLEROW_AVX2
// For BGRAToARGB, ABGRToARGB, RGBAToARGB, and ARGBToRGBA.
void ARGBShuffleRow_AVX2(const uint8_t* src_argb,
                         uint8_t* dst_argb,
                         const uint8_t* shuffler,
                         int width) {
  asm volatile(

      "vbroadcastf128 (%3),%%ymm5                \n"

      LABELALIGN
      "1:                                        \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "vmovdqu     0x20(%0),%%ymm1               \n"
      "lea         0x40(%0),%0                   \n"
      "vpshufb     %%ymm5,%%ymm0,%%ymm0          \n"
      "vpshufb     %%ymm5,%%ymm1,%%ymm1          \n"
      "vmovdqu     %%ymm0,(%1)                   \n"
      "vmovdqu     %%ymm1,0x20(%1)               \n"
      "lea         0x40(%1),%1                   \n"
      "sub         $0x10,%2                      \n"
      "jg          1b                            \n"
      "vzeroupper                                \n"
      : "+r"(src_argb),  // %0
        "+r"(dst_argb),  // %1
        "+r"(width)      // %2
      : "r"(shuffler)    // %3
      : "memory", "cc", "xmm0", "xmm1", "xmm5");
}
#endif  // HAS_ARGBSHUFFLEROW_AVX2

#ifdef HAS_I422TOUYVYROW_SSE2
void I422ToUYVYRow_SSE2(const uint8_t* src_y,
                        const uint8_t* src_u,
                        const uint8_t* src_v,
                        uint8_t* dst_uyvy,
                        int width) {
  asm volatile(

      "sub         %1,%2                         \n"

      LABELALIGN
      "1:                                        \n"
      "movq        (%1),%%xmm2                   \n"
      "movq        0x00(%1,%2,1),%%xmm1          \n"
      "add         $0x8,%1                       \n"
      "punpcklbw   %%xmm1,%%xmm2                 \n"
      "movdqu      (%0),%%xmm0                   \n"
      "movdqa      %%xmm2,%%xmm1                 \n"
      "add         $0x10,%0                      \n"
      "punpcklbw   %%xmm0,%%xmm1                 \n"
      "punpckhbw   %%xmm0,%%xmm2                 \n"
      "movdqu      %%xmm1,(%3)                   \n"
      "movdqu      %%xmm2,0x10(%3)               \n"
      "lea         0x20(%3),%3                   \n"
      "sub         $0x10,%4                      \n"
      "jg          1b                            \n"
      : "+r"(src_y),     // %0
        "+r"(src_u),     // %1
        "+r"(src_v),     // %2
        "+r"(dst_uyvy),  // %3
        "+rm"(width)     // %4
      :
      : "memory", "cc", "xmm0", "xmm1", "xmm2");
}
#endif  // HAS_I422TOUYVYROW_SSE2

#ifdef HAS_I422TOUYVYROW_AVX2
void I422ToUYVYRow_AVX2(const uint8_t* src_y,
                        const uint8_t* src_u,
                        const uint8_t* src_v,
                        uint8_t* dst_uyvy,
                        int width) {
  asm volatile(

      "sub         %1,%2                         \n"

      LABELALIGN
      "1:                                        \n"
      "vpmovzxbw   (%1),%%ymm1                   \n"
      "vpmovzxbw   0x00(%1,%2,1),%%ymm2          \n"
      "add         $0x10,%1                      \n"
      "vpsllw      $0x8,%%ymm2,%%ymm2            \n"
      "vpor        %%ymm1,%%ymm2,%%ymm2          \n"
      "vmovdqu     (%0),%%ymm0                   \n"
      "add         $0x20,%0                      \n"
      "vpunpcklbw  %%ymm0,%%ymm2,%%ymm1          \n"
      "vpunpckhbw  %%ymm0,%%ymm2,%%ymm2          \n"
      "vextractf128 $0x0,%%ymm1,(%3)             \n"
      "vextractf128 $0x0,%%ymm2,0x10(%3)         \n"
      "vextractf128 $0x1,%%ymm1,0x20(%3)         \n"
      "vextractf128 $0x1,%%ymm2,0x30(%3)         \n"
      "lea         0x40(%3),%3                   \n"
      "sub         $0x20,%4                      \n"
      "jg          1b                            \n"
      "vzeroupper                                \n"
      : "+r"(src_y),     // %0
        "+r"(src_u),     // %1
        "+r"(src_v),     // %2
        "+r"(dst_uyvy),  // %3
        "+rm"(width)     // %4
      :
      : "memory", "cc", "xmm0", "xmm1", "xmm2");
}
#endif  // HAS_I422TOUYVYROW_AVX2

#endif  // defined(__x86_64__) || defined(__i386__)

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif
