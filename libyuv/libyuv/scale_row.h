/*
 *  Copyright 2013 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef INCLUDE_LIBYUV_SCALE_ROW_H_
#define INCLUDE_LIBYUV_SCALE_ROW_H_

#include "libyuv/basic_types.h"
#include "libyuv/scale.h"

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

// Visual C 2012 required for AVX2.
#if defined(_M_IX86) && !defined(__clang__) && defined(_MSC_VER) && \
    _MSC_VER >= 1700
#define VISUALC_HAS_AVX2 1
#endif  // VisualStudio >= 2012

// The following are available on all x86 platforms:
#if !defined(LIBYUV_DISABLE_X86) && \
    (defined(_M_IX86) || defined(__x86_64__) || defined(__i386__))
#define HAS_FIXEDDIV1_X86
#define HAS_FIXEDDIV_X86
#define HAS_SCALEADDROW_SSE2
#define HAS_SCALEARGBCOLS_SSE2
#define HAS_SCALEARGBCOLSUP2_SSE2
#define HAS_SCALEARGBFILTERCOLS_SSSE3
#define HAS_SCALEARGBROWDOWN2_SSE2
#define HAS_SCALEARGBROWDOWNEVEN_SSE2
#define HAS_SCALECOLSUP2_SSE2
#define HAS_SCALEFILTERCOLS_SSSE3
#define HAS_SCALEROWDOWN2_SSSE3
#define HAS_SCALEROWDOWN34_SSSE3
#define HAS_SCALEROWDOWN38_SSSE3
#define HAS_SCALEROWDOWN4_SSSE3
#endif

// The following are available for gcc/clang x86 platforms:
// TODO(fbarchard): Port to Visual C
#if !defined(LIBYUV_DISABLE_X86) && (defined(__x86_64__) || defined(__i386__))
#define HAS_SCALEUVROWDOWN2BOX_SSSE3
#define HAS_SCALEROWUP2LINEAR_SSE2
#define HAS_SCALEROWUP2LINEAR_SSSE3
#define HAS_SCALEROWUP2BILINEAR_SSE2
#define HAS_SCALEROWUP2BILINEAR_SSSE3
#define HAS_SCALEROWUP2LINEAR_12_SSSE3
#define HAS_SCALEROWUP2BILINEAR_12_SSSE3
#define HAS_SCALEROWUP2LINEAR_16_SSE2
#define HAS_SCALEROWUP2BILINEAR_16_SSE2
#define HAS_SCALEUVROWUP2LINEAR_SSSE3
#define HAS_SCALEUVROWUP2BILINEAR_SSSE3
#define HAS_SCALEUVROWUP2LINEAR_16_SSE2
#define HAS_SCALEUVROWUP2BILINEAR_16_SSE2
#endif

// The following are available for gcc/clang x86 platforms, but
// require clang 3.4 or gcc 4.7.
// TODO(fbarchard): Port to Visual C
#if !defined(LIBYUV_DISABLE_X86) &&               \
    (defined(__x86_64__) || defined(__i386__)) && \
    (defined(CLANG_HAS_AVX2) || defined(GCC_HAS_AVX2))
#define HAS_SCALEUVROWDOWN2BOX_AVX2
#define HAS_SCALEROWUP2LINEAR_AVX2
#define HAS_SCALEROWUP2BILINEAR_AVX2
#define HAS_SCALEROWUP2LINEAR_12_AVX2
#define HAS_SCALEROWUP2BILINEAR_12_AVX2
#define HAS_SCALEROWUP2LINEAR_16_AVX2
#define HAS_SCALEROWUP2BILINEAR_16_AVX2
#define HAS_SCALEUVROWUP2LINEAR_AVX2
#define HAS_SCALEUVROWUP2BILINEAR_AVX2
#define HAS_SCALEUVROWUP2LINEAR_16_AVX2
#define HAS_SCALEUVROWUP2BILINEAR_16_AVX2
#endif

// The following are available on all x86 platforms, but
// require VS2012, clang 3.4 or gcc 4.7.
// The code supports NaCL but requires a new compiler and validator.
#if !defined(LIBYUV_DISABLE_X86) &&                          \
    (defined(VISUALC_HAS_AVX2) || defined(CLANG_HAS_AVX2) || \
     defined(GCC_HAS_AVX2))
#define HAS_SCALEADDROW_AVX2
#define HAS_SCALEROWDOWN2_AVX2
#define HAS_SCALEROWDOWN4_AVX2
#endif

// The following are available on Neon platforms:
#if !defined(LIBYUV_DISABLE_NEON) && \
    (defined(__ARM_NEON__) || defined(LIBYUV_NEON) || defined(__aarch64__))
#define HAS_SCALEADDROW_NEON
#define HAS_SCALEARGBCOLS_NEON
#define HAS_SCALEARGBFILTERCOLS_NEON
#define HAS_SCALEARGBROWDOWN2_NEON
#define HAS_SCALEARGBROWDOWNEVEN_NEON
#define HAS_SCALEFILTERCOLS_NEON
#define HAS_SCALEROWDOWN2_NEON
#define HAS_SCALEROWDOWN34_NEON
#define HAS_SCALEROWDOWN38_NEON
#define HAS_SCALEROWDOWN4_NEON
#define HAS_SCALEUVROWDOWN2BOX_NEON
#define HAS_SCALEUVROWDOWNEVEN_NEON
#define HAS_SCALEROWUP2LINEAR_NEON
#define HAS_SCALEROWUP2BILINEAR_NEON
#define HAS_SCALEROWUP2LINEAR_12_NEON
#define HAS_SCALEROWUP2BILINEAR_12_NEON
#define HAS_SCALEROWUP2LINEAR_16_NEON
#define HAS_SCALEROWUP2BILINEAR_16_NEON
#define HAS_SCALEUVROWUP2LINEAR_NEON
#define HAS_SCALEUVROWUP2BILINEAR_NEON
#define HAS_SCALEUVROWUP2LINEAR_16_NEON
#define HAS_SCALEUVROWUP2BILINEAR_16_NEON
#endif

#if !defined(LIBYUV_DISABLE_MSA) && defined(__mips_msa)
#define HAS_SCALEADDROW_MSA
#define HAS_SCALEARGBCOLS_MSA
#define HAS_SCALEARGBFILTERCOLS_MSA
#define HAS_SCALEARGBROWDOWN2_MSA
#define HAS_SCALEARGBROWDOWNEVEN_MSA
#define HAS_SCALEFILTERCOLS_MSA
#define HAS_SCALEROWDOWN2_MSA
#define HAS_SCALEROWDOWN34_MSA
#define HAS_SCALEROWDOWN38_MSA
#define HAS_SCALEROWDOWN4_MSA
#endif

#if !defined(LIBYUV_DISABLE_MMI) && defined(_MIPS_ARCH_LOONGSON3A)
#define HAS_FIXEDDIV1_MIPS
#define HAS_FIXEDDIV_MIPS
#define HAS_SCALEADDROW_16_MMI
#define HAS_SCALEADDROW_MMI
#define HAS_SCALEARGBCOLS_MMI
#define HAS_SCALEARGBCOLSUP2_MMI
#define HAS_SCALEARGBROWDOWN2_MMI
#define HAS_SCALEARGBROWDOWNEVEN_MMI
#define HAS_SCALECOLS_16_MMI
#define HAS_SCALECOLS_MMI
#define HAS_SCALEROWDOWN2_16_MMI
#define HAS_SCALEROWDOWN2_MMI
#define HAS_SCALEROWDOWN4_16_MMI
#define HAS_SCALEROWDOWN4_MMI
#define HAS_SCALEROWDOWN34_MMI
#endif

// Scale ARGB vertically with bilinear interpolation.
void ScalePlaneVertical(int src_height,
                        int dst_width,
                        int dst_height,
                        int src_stride,
                        int dst_stride,
                        const uint8_t* src_argb,
                        uint8_t* dst_argb,
                        int x,
                        int y,
                        int dy,
                        int bpp,
                        enum FilterMode filtering);

// Simplify the filtering based on scale factors.
enum FilterMode ScaleFilterReduce(int src_width,
                                  int src_height,
                                  int dst_width,
                                  int dst_height,
                                  enum FilterMode filtering);

// Divide num by div and return as 16.16 fixed point result.
int FixedDiv_C(int num, int div);
int FixedDiv_X86(int num, int div);
int FixedDiv_MIPS(int num, int div);
// Divide num - 1 by div - 1 and return as 16.16 fixed point result.
int FixedDiv1_C(int num, int div);
int FixedDiv1_X86(int num, int div);
int FixedDiv1_MIPS(int num, int div);
#ifdef HAS_FIXEDDIV_X86
#define FixedDiv FixedDiv_X86
#define FixedDiv1 FixedDiv1_X86
#elif defined HAS_FIXEDDIV_MIPS
#define FixedDiv FixedDiv_MIPS
#define FixedDiv1 FixedDiv1_MIPS
#else
#define FixedDiv FixedDiv_C
#define FixedDiv1 FixedDiv1_C
#endif

// Compute slope values for stepping.
void ScaleSlope(int src_width,
                int src_height,
                int dst_width,
                int dst_height,
                enum FilterMode filtering,
                int* x,
                int* y,
                int* dx,
                int* dy);
void ScaleARGBRowDown2_C(const uint8_t* src_argb,
                         ptrdiff_t src_stride,
                         uint8_t* dst_argb,
                         int dst_width);
void ScaleARGBRowDown2Linear_C(const uint8_t* src_argb,
                               ptrdiff_t src_stride,
                               uint8_t* dst_argb,
                               int dst_width);
void ScaleARGBRowDown2Box_C(const uint8_t* src_argb,
                            ptrdiff_t src_stride,
                            uint8_t* dst_argb,
                            int dst_width);
void ScaleARGBRowDownEven_C(const uint8_t* src_argb,
                            ptrdiff_t src_stride,
                            int src_stepx,
                            uint8_t* dst_argb,
                            int dst_width);
void ScaleARGBRowDownEvenBox_C(const uint8_t* src_argb,
                               ptrdiff_t src_stride,
                               int src_stepx,
                               uint8_t* dst_argb,
                               int dst_width);
void ScaleARGBCols_C(uint8_t* dst_argb,
                     const uint8_t* src_argb,
                     int dst_width,
                     int x,
                     int dx);
void ScaleARGBCols64_C(uint8_t* dst_argb,
                       const uint8_t* src_argb,
                       int dst_width,
                       int x32,
                       int dx);
void ScaleARGBColsUp2_C(uint8_t* dst_argb,
                        const uint8_t* src_argb,
                        int dst_width,
                        int,
                        int);
void ScaleARGBFilterCols_C(uint8_t* dst_argb,
                           const uint8_t* src_argb,
                           int dst_width,
                           int x,
                           int dx);
void ScaleARGBFilterCols64_C(uint8_t* dst_argb,
                             const uint8_t* src_argb,
                             int dst_width,
                             int x32,
                             int dx);

// ARGB Column functions
void ScaleARGBCols_SSE2(uint8_t* dst_argb,
                        const uint8_t* src_argb,
                        int dst_width,
                        int x,
                        int dx);
void ScaleARGBFilterCols_SSSE3(uint8_t* dst_argb,
                               const uint8_t* src_argb,
                               int dst_width,
                               int x,
                               int dx);
void ScaleARGBColsUp2_SSE2(uint8_t* dst_argb,
                           const uint8_t* src_argb,
                           int dst_width,
                           int x,
                           int dx);
void ScaleARGBFilterCols_NEON(uint8_t* dst_argb,
                              const uint8_t* src_argb,
                              int dst_width,
                              int x,
                              int dx);
void ScaleARGBCols_NEON(uint8_t* dst_argb,
                        const uint8_t* src_argb,
                        int dst_width,
                        int x,
                        int dx);
void ScaleARGBFilterCols_Any_NEON(uint8_t* dst_ptr,
                                  const uint8_t* src_ptr,
                                  int dst_width,
                                  int x,
                                  int dx);
void ScaleARGBCols_Any_NEON(uint8_t* dst_ptr,
                            const uint8_t* src_ptr,
                            int dst_width,
                            int x,
                            int dx);
void ScaleARGBFilterCols_MSA(uint8_t* dst_argb,
                             const uint8_t* src_argb,
                             int dst_width,
                             int x,
                             int dx);
void ScaleARGBCols_MSA(uint8_t* dst_argb,
                       const uint8_t* src_argb,
                       int dst_width,
                       int x,
                       int dx);
void ScaleARGBFilterCols_Any_MSA(uint8_t* dst_ptr,
                                 const uint8_t* src_ptr,
                                 int dst_width,
                                 int x,
                                 int dx);
void ScaleARGBCols_Any_MSA(uint8_t* dst_ptr,
                           const uint8_t* src_ptr,
                           int dst_width,
                           int x,
                           int dx);
void ScaleARGBCols_MMI(uint8_t* dst_argb,
                       const uint8_t* src_argb,
                       int dst_width,
                       int x,
                       int dx);
void ScaleARGBCols_Any_MMI(uint8_t* dst_ptr,
                           const uint8_t* src_ptr,
                           int dst_width,
                           int x,
                           int dx);

// ARGB Row functions
void ScaleARGBRowDown2_SSE2(const uint8_t* src_argb,
                            ptrdiff_t src_stride,
                            uint8_t* dst_argb,
                            int dst_width);
void ScaleARGBRowDown2Linear_SSE2(const uint8_t* src_argb,
                                  ptrdiff_t src_stride,
                                  uint8_t* dst_argb,
                                  int dst_width);
void ScaleARGBRowDown2Box_SSE2(const uint8_t* src_argb,
                               ptrdiff_t src_stride,
                               uint8_t* dst_argb,
                               int dst_width);
void ScaleARGBRowDown2_NEON(const uint8_t* src_ptr,
                            ptrdiff_t src_stride,
                            uint8_t* dst,
                            int dst_width);
void ScaleARGBRowDown2Linear_NEON(const uint8_t* src_argb,
                                  ptrdiff_t src_stride,
                                  uint8_t* dst_argb,
                                  int dst_width);
void ScaleARGBRowDown2Box_NEON(const uint8_t* src_ptr,
                               ptrdiff_t src_stride,
                               uint8_t* dst,
                               int dst_width);
void ScaleARGBRowDown2_MSA(const uint8_t* src_argb,
                           ptrdiff_t src_stride,
                           uint8_t* dst_argb,
                           int dst_width);
void ScaleARGBRowDown2Linear_MSA(const uint8_t* src_argb,
                                 ptrdiff_t src_stride,
                                 uint8_t* dst_argb,
                                 int dst_width);
void ScaleARGBRowDown2Box_MSA(const uint8_t* src_argb,
                              ptrdiff_t src_stride,
                              uint8_t* dst_argb,
                              int dst_width);
void ScaleARGBRowDown2_MMI(const uint8_t* src_argb,
                           ptrdiff_t src_stride,
                           uint8_t* dst_argb,
                           int dst_width);
void ScaleARGBRowDown2Linear_MMI(const uint8_t* src_argb,
                                 ptrdiff_t src_stride,
                                 uint8_t* dst_argb,
                                 int dst_width);
void ScaleARGBRowDown2Box_MMI(const uint8_t* src_argb,
                              ptrdiff_t src_stride,
                              uint8_t* dst_argb,
                              int dst_width);
void ScaleARGBRowDown2_Any_SSE2(const uint8_t* src_ptr,
                                ptrdiff_t src_stride,
                                uint8_t* dst_ptr,
                                int dst_width);
void ScaleARGBRowDown2Linear_Any_SSE2(const uint8_t* src_ptr,
                                      ptrdiff_t src_stride,
                                      uint8_t* dst_ptr,
                                      int dst_width);
void ScaleARGBRowDown2Box_Any_SSE2(const uint8_t* src_ptr,
                                   ptrdiff_t src_stride,
                                   uint8_t* dst_ptr,
                                   int dst_width);
void ScaleARGBRowDown2_Any_NEON(const uint8_t* src_ptr,
                                ptrdiff_t src_stride,
                                uint8_t* dst_ptr,
                                int dst_width);
void ScaleARGBRowDown2Linear_Any_NEON(const uint8_t* src_ptr,
                                      ptrdiff_t src_stride,
                                      uint8_t* dst_ptr,
                                      int dst_width);
void ScaleARGBRowDown2Box_Any_NEON(const uint8_t* src_ptr,
                                   ptrdiff_t src_stride,
                                   uint8_t* dst_ptr,
                                   int dst_width);
void ScaleARGBRowDown2_Any_MSA(const uint8_t* src_ptr,
                               ptrdiff_t src_stride,
                               uint8_t* dst_ptr,
                               int dst_width);
void ScaleARGBRowDown2Linear_Any_MSA(const uint8_t* src_ptr,
                                     ptrdiff_t src_stride,
                                     uint8_t* dst_ptr,
                                     int dst_width);
void ScaleARGBRowDown2Box_Any_MSA(const uint8_t* src_ptr,
                                  ptrdiff_t src_stride,
                                  uint8_t* dst_ptr,
                                  int dst_width);
void ScaleARGBRowDown2_Any_MMI(const uint8_t* src_ptr,
                               ptrdiff_t src_stride,
                               uint8_t* dst_ptr,
                               int dst_width);
void ScaleARGBRowDown2Linear_Any_MMI(const uint8_t* src_ptr,
                                     ptrdiff_t src_stride,
                                     uint8_t* dst_ptr,
                                     int dst_width);
void ScaleARGBRowDown2Box_Any_MMI(const uint8_t* src_ptr,
                                  ptrdiff_t src_stride,
                                  uint8_t* dst_ptr,
                                  int dst_width);
void ScaleARGBRowDownEven_SSE2(const uint8_t* src_argb,
                               ptrdiff_t src_stride,
                               int src_stepx,
                               uint8_t* dst_argb,
                               int dst_width);
void ScaleARGBRowDownEvenBox_SSE2(const uint8_t* src_argb,
                                  ptrdiff_t src_stride,
                                  int src_stepx,
                                  uint8_t* dst_argb,
                                  int dst_width);
void ScaleARGBRowDownEven_NEON(const uint8_t* src_argb,
                               ptrdiff_t src_stride,
                               int src_stepx,
                               uint8_t* dst_argb,
                               int dst_width);
void ScaleARGBRowDownEvenBox_NEON(const uint8_t* src_argb,
                                  ptrdiff_t src_stride,
                                  int src_stepx,
                                  uint8_t* dst_argb,
                                  int dst_width);
void ScaleARGBRowDownEven_MSA(const uint8_t* src_argb,
                              ptrdiff_t src_stride,
                              int32_t src_stepx,
                              uint8_t* dst_argb,
                              int dst_width);
void ScaleARGBRowDownEvenBox_MSA(const uint8_t* src_argb,
                                 ptrdiff_t src_stride,
                                 int src_stepx,
                                 uint8_t* dst_argb,
                                 int dst_width);
void ScaleARGBRowDownEven_MMI(const uint8_t* src_argb,
                              ptrdiff_t src_stride,
                              int32_t src_stepx,
                              uint8_t* dst_argb,
                              int dst_width);
void ScaleARGBRowDownEvenBox_MMI(const uint8_t* src_argb,
                                 ptrdiff_t src_stride,
                                 int src_stepx,
                                 uint8_t* dst_argb,
                                 int dst_width);
void ScaleARGBRowDownEven_Any_SSE2(const uint8_t* src_ptr,
                                   ptrdiff_t src_stride,
                                   int src_stepx,
                                   uint8_t* dst_ptr,
                                   int dst_width);
void ScaleARGBRowDownEvenBox_Any_SSE2(const uint8_t* src_ptr,
                                      ptrdiff_t src_stride,
                                      int src_stepx,
                                      uint8_t* dst_ptr,
                                      int dst_width);
void ScaleARGBRowDownEven_Any_NEON(const uint8_t* src_ptr,
                                   ptrdiff_t src_stride,
                                   int src_stepx,
                                   uint8_t* dst_ptr,
                                   int dst_width);
void ScaleARGBRowDownEvenBox_Any_NEON(const uint8_t* src_ptr,
                                      ptrdiff_t src_stride,
                                      int src_stepx,
                                      uint8_t* dst_ptr,
                                      int dst_width);
void ScaleARGBRowDownEven_Any_MSA(const uint8_t* src_ptr,
                                  ptrdiff_t src_stride,
                                  int32_t src_stepx,
                                  uint8_t* dst_ptr,
                                  int dst_width);
void ScaleARGBRowDownEvenBox_Any_MSA(const uint8_t* src_ptr,
                                     ptrdiff_t src_stride,
                                     int src_stepx,
                                     uint8_t* dst_ptr,
                                     int dst_width);
void ScaleARGBRowDownEven_Any_MMI(const uint8_t* src_ptr,
                                  ptrdiff_t src_stride,
                                  int32_t src_stepx,
                                  uint8_t* dst_ptr,
                                  int dst_width);
void ScaleARGBRowDownEvenBox_Any_MMI(const uint8_t* src_ptr,
                                     ptrdiff_t src_stride,
                                     int src_stepx,
                                     uint8_t* dst_ptr,
                                     int dst_width);

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif

#endif  // INCLUDE_LIBYUV_SCALE_ROW_H_
