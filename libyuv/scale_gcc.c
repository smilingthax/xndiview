/*
 *  Copyright 2013 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "libyuv/row.h"
#include "libyuv/scale_row.h"

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

// This module is for GCC x86 and x64.
#if !defined(LIBYUV_DISABLE_X86) && (defined(__x86_64__) || defined(__i386__))


// GCC versions of row functions are verbatim conversions from Visual C.
// Generated using gcc disassembly on Visual C object file:
// objdump -D yuvscaler.obj >yuvscaler.txt

void ScaleARGBRowDown2_SSE2(const uint8_t* src_argb,
                            ptrdiff_t src_stride,
                            uint8_t* dst_argb,
                            int dst_width) {
  (void)src_stride;
  asm volatile(LABELALIGN
      "1:                                        \n"
      "movdqu      (%0),%%xmm0                   \n"
      "movdqu      0x10(%0),%%xmm1               \n"
      "lea         0x20(%0),%0                   \n"
      "shufps      $0xdd,%%xmm1,%%xmm0           \n"
      "movdqu      %%xmm0,(%1)                   \n"
      "lea         0x10(%1),%1                   \n"
      "sub         $0x4,%2                       \n"
      "jg          1b                            \n"
               : "+r"(src_argb),  // %0
                 "+r"(dst_argb),  // %1
                 "+r"(dst_width)  // %2
                 ::"memory",
                 "cc", "xmm0", "xmm1");
}

void ScaleARGBRowDown2Linear_SSE2(const uint8_t* src_argb,
                                  ptrdiff_t src_stride,
                                  uint8_t* dst_argb,
                                  int dst_width) {
  (void)src_stride;
  asm volatile(LABELALIGN
      "1:                                        \n"
      "movdqu      (%0),%%xmm0                   \n"
      "movdqu      0x10(%0),%%xmm1               \n"
      "lea         0x20(%0),%0                   \n"
      "movdqa      %%xmm0,%%xmm2                 \n"
      "shufps      $0x88,%%xmm1,%%xmm0           \n"
      "shufps      $0xdd,%%xmm1,%%xmm2           \n"
      "pavgb       %%xmm2,%%xmm0                 \n"
      "movdqu      %%xmm0,(%1)                   \n"
      "lea         0x10(%1),%1                   \n"
      "sub         $0x4,%2                       \n"
      "jg          1b                            \n"
               : "+r"(src_argb),  // %0
                 "+r"(dst_argb),  // %1
                 "+r"(dst_width)  // %2
                 ::"memory",
                 "cc", "xmm0", "xmm1");
}

void ScaleARGBRowDown2Box_SSE2(const uint8_t* src_argb,
                               ptrdiff_t src_stride,
                               uint8_t* dst_argb,
                               int dst_width) {
  asm volatile(LABELALIGN
      "1:                                        \n"
      "movdqu      (%0),%%xmm0                   \n"
      "movdqu      0x10(%0),%%xmm1               \n"
      "movdqu      0x00(%0,%3,1),%%xmm2          \n"
      "movdqu      0x10(%0,%3,1),%%xmm3          \n"
      "lea         0x20(%0),%0                   \n"
      "pavgb       %%xmm2,%%xmm0                 \n"
      "pavgb       %%xmm3,%%xmm1                 \n"
      "movdqa      %%xmm0,%%xmm2                 \n"
      "shufps      $0x88,%%xmm1,%%xmm0           \n"
      "shufps      $0xdd,%%xmm1,%%xmm2           \n"
      "pavgb       %%xmm2,%%xmm0                 \n"
      "movdqu      %%xmm0,(%1)                   \n"
      "lea         0x10(%1),%1                   \n"
      "sub         $0x4,%2                       \n"
      "jg          1b                            \n"
               : "+r"(src_argb),              // %0
                 "+r"(dst_argb),              // %1
                 "+r"(dst_width)              // %2
               : "r"((intptr_t)(src_stride))  // %3
               : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3");
}

// Reads 4 pixels at a time.
// Alignment requirement: dst_argb 16 byte aligned.
void ScaleARGBRowDownEven_SSE2(const uint8_t* src_argb,
                               ptrdiff_t src_stride,
                               int src_stepx,
                               uint8_t* dst_argb,
                               int dst_width) {
  intptr_t src_stepx_x4 = (intptr_t)(src_stepx);
  intptr_t src_stepx_x12;
  (void)src_stride;
  asm volatile(
      "lea         0x00(,%1,4),%1                \n"
      "lea         0x00(%1,%1,2),%4              \n"

      LABELALIGN
      "1:                                        \n"
      "movd        (%0),%%xmm0                   \n"
      "movd        0x00(%0,%1,1),%%xmm1          \n"
      "punpckldq   %%xmm1,%%xmm0                 \n"
      "movd        0x00(%0,%1,2),%%xmm2          \n"
      "movd        0x00(%0,%4,1),%%xmm3          \n"
      "lea         0x00(%0,%1,4),%0              \n"
      "punpckldq   %%xmm3,%%xmm2                 \n"
      "punpcklqdq  %%xmm2,%%xmm0                 \n"
      "movdqu      %%xmm0,(%2)                   \n"
      "lea         0x10(%2),%2                   \n"
      "sub         $0x4,%3                       \n"
      "jg          1b                            \n"
      : "+r"(src_argb),       // %0
        "+r"(src_stepx_x4),   // %1
        "+r"(dst_argb),       // %2
        "+r"(dst_width),      // %3
        "=&r"(src_stepx_x12)  // %4
        ::"memory",
        "cc", "xmm0", "xmm1", "xmm2", "xmm3");
}

// Blends four 2x2 to 4x1.
// Alignment requirement: dst_argb 16 byte aligned.
void ScaleARGBRowDownEvenBox_SSE2(const uint8_t* src_argb,
                                  ptrdiff_t src_stride,
                                  int src_stepx,
                                  uint8_t* dst_argb,
                                  int dst_width) {
  intptr_t src_stepx_x4 = (intptr_t)(src_stepx);
  intptr_t src_stepx_x12;
  intptr_t row1 = (intptr_t)(src_stride);
  asm volatile(
      "lea         0x00(,%1,4),%1                \n"
      "lea         0x00(%1,%1,2),%4              \n"
      "lea         0x00(%0,%5,1),%5              \n"

      LABELALIGN
      "1:                                        \n"
      "movq        (%0),%%xmm0                   \n"
      "movhps      0x00(%0,%1,1),%%xmm0          \n"
      "movq        0x00(%0,%1,2),%%xmm1          \n"
      "movhps      0x00(%0,%4,1),%%xmm1          \n"
      "lea         0x00(%0,%1,4),%0              \n"
      "movq        (%5),%%xmm2                   \n"
      "movhps      0x00(%5,%1,1),%%xmm2          \n"
      "movq        0x00(%5,%1,2),%%xmm3          \n"
      "movhps      0x00(%5,%4,1),%%xmm3          \n"
      "lea         0x00(%5,%1,4),%5              \n"
      "pavgb       %%xmm2,%%xmm0                 \n"
      "pavgb       %%xmm3,%%xmm1                 \n"
      "movdqa      %%xmm0,%%xmm2                 \n"
      "shufps      $0x88,%%xmm1,%%xmm0           \n"
      "shufps      $0xdd,%%xmm1,%%xmm2           \n"
      "pavgb       %%xmm2,%%xmm0                 \n"
      "movdqu      %%xmm0,(%2)                   \n"
      "lea         0x10(%2),%2                   \n"
      "sub         $0x4,%3                       \n"
      "jg          1b                            \n"
      : "+r"(src_argb),        // %0
        "+r"(src_stepx_x4),    // %1
        "+r"(dst_argb),        // %2
        "+rm"(dst_width),      // %3
        "=&r"(src_stepx_x12),  // %4
        "+r"(row1)             // %5
        ::"memory",
        "cc", "xmm0", "xmm1", "xmm2", "xmm3");
}

void ScaleARGBCols_SSE2(uint8_t* dst_argb,
                        const uint8_t* src_argb,
                        int dst_width,
                        int x,
                        int dx) {
  intptr_t x0, x1;
  asm volatile(
      "movd        %5,%%xmm2                     \n"
      "movd        %6,%%xmm3                     \n"
      "pshufd      $0x0,%%xmm2,%%xmm2            \n"
      "pshufd      $0x11,%%xmm3,%%xmm0           \n"
      "paddd       %%xmm0,%%xmm2                 \n"
      "paddd       %%xmm3,%%xmm3                 \n"
      "pshufd      $0x5,%%xmm3,%%xmm0            \n"
      "paddd       %%xmm0,%%xmm2                 \n"
      "paddd       %%xmm3,%%xmm3                 \n"
      "pshufd      $0x0,%%xmm3,%%xmm3            \n"
      "pextrw      $0x1,%%xmm2,%k0               \n"
      "pextrw      $0x3,%%xmm2,%k1               \n"
      "cmp         $0x0,%4                       \n"
      "jl          99f                           \n"
      "sub         $0x4,%4                       \n"
      "jl          49f                           \n"

      LABELALIGN
      "40:                                       \n"
      "movd        0x00(%3,%0,4),%%xmm0          \n"
      "movd        0x00(%3,%1,4),%%xmm1          \n"
      "pextrw      $0x5,%%xmm2,%k0               \n"
      "pextrw      $0x7,%%xmm2,%k1               \n"
      "paddd       %%xmm3,%%xmm2                 \n"
      "punpckldq   %%xmm1,%%xmm0                 \n"
      "movd        0x00(%3,%0,4),%%xmm1          \n"
      "movd        0x00(%3,%1,4),%%xmm4          \n"
      "pextrw      $0x1,%%xmm2,%k0               \n"
      "pextrw      $0x3,%%xmm2,%k1               \n"
      "punpckldq   %%xmm4,%%xmm1                 \n"
      "punpcklqdq  %%xmm1,%%xmm0                 \n"
      "movdqu      %%xmm0,(%2)                   \n"
      "lea         0x10(%2),%2                   \n"
      "sub         $0x4,%4                       \n"
      "jge         40b                           \n"

      "49:                                       \n"
      "test        $0x2,%4                       \n"
      "je          29f                           \n"
      "movd        0x00(%3,%0,4),%%xmm0          \n"
      "movd        0x00(%3,%1,4),%%xmm1          \n"
      "pextrw      $0x5,%%xmm2,%k0               \n"
      "punpckldq   %%xmm1,%%xmm0                 \n"
      "movq        %%xmm0,(%2)                   \n"
      "lea         0x8(%2),%2                    \n"
      "29:                                       \n"
      "test        $0x1,%4                       \n"
      "je          99f                           \n"
      "movd        0x00(%3,%0,4),%%xmm0          \n"
      "movd        %%xmm0,(%2)                   \n"
      "99:                                       \n"
      : "=&a"(x0),       // %0
        "=&d"(x1),       // %1
        "+r"(dst_argb),  // %2
        "+r"(src_argb),  // %3
        "+r"(dst_width)  // %4
      : "rm"(x),         // %5
        "rm"(dx)         // %6
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4");
}

// Reads 4 pixels, duplicates them and writes 8 pixels.
// Alignment requirement: src_argb 16 byte aligned, dst_argb 16 byte aligned.
void ScaleARGBColsUp2_SSE2(uint8_t* dst_argb,
                           const uint8_t* src_argb,
                           int dst_width,
                           int x,
                           int dx) {
  (void)x;
  (void)dx;
  asm volatile(LABELALIGN
      "1:                                        \n"
      "movdqu      (%1),%%xmm0                   \n"
      "lea         0x10(%1),%1                   \n"
      "movdqa      %%xmm0,%%xmm1                 \n"
      "punpckldq   %%xmm0,%%xmm0                 \n"
      "punpckhdq   %%xmm1,%%xmm1                 \n"
      "movdqu      %%xmm0,(%0)                   \n"
      "movdqu      %%xmm1,0x10(%0)               \n"
      "lea         0x20(%0),%0                   \n"
      "sub         $0x8,%2                       \n"
      "jg          1b                            \n"

               : "+r"(dst_argb),  // %0
                 "+r"(src_argb),  // %1
                 "+r"(dst_width)  // %2
                 ::"memory",
                 "cc", "xmm0", "xmm1");
}

// Shuffle table for arranging 2 pixels into pairs for pmaddubsw
static const uvec8 kShuffleColARGB = {
    0u, 4u,  1u, 5u,  2u,  6u,  3u,  7u,  // bbggrraa 1st pixel
    8u, 12u, 9u, 13u, 10u, 14u, 11u, 15u  // bbggrraa 2nd pixel
};

// Shuffle table for duplicating 2 fractions into 8 bytes each
static const uvec8 kShuffleFractions = {
    0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u,
};

// Bilinear row filtering combines 4x2 -> 4x1. SSSE3 version
void ScaleARGBFilterCols_SSSE3(uint8_t* dst_argb,
                               const uint8_t* src_argb,
                               int dst_width,
                               int x,
                               int dx) {
  intptr_t x0, x1;
  asm volatile(
      "movdqa      %0,%%xmm4                     \n"
      "movdqa      %1,%%xmm5                     \n"
      :
      : "m"(kShuffleColARGB),   // %0
        "m"(kShuffleFractions)  // %1
  );

  asm volatile(
      "movd        %5,%%xmm2                     \n"
      "movd        %6,%%xmm3                     \n"
      "pcmpeqb     %%xmm6,%%xmm6                 \n"
      "psrlw       $0x9,%%xmm6                   \n"
      "pextrw      $0x1,%%xmm2,%k3               \n"
      "sub         $0x2,%2                       \n"
      "jl          29f                           \n"
      "movdqa      %%xmm2,%%xmm0                 \n"
      "paddd       %%xmm3,%%xmm0                 \n"
      "punpckldq   %%xmm0,%%xmm2                 \n"
      "punpckldq   %%xmm3,%%xmm3                 \n"
      "paddd       %%xmm3,%%xmm3                 \n"
      "pextrw      $0x3,%%xmm2,%k4               \n"

      LABELALIGN
      "2:                                        \n"
      "movdqa      %%xmm2,%%xmm1                 \n"
      "paddd       %%xmm3,%%xmm2                 \n"
      "movq        0x00(%1,%3,4),%%xmm0          \n"
      "psrlw       $0x9,%%xmm1                   \n"
      "movhps      0x00(%1,%4,4),%%xmm0          \n"
      "pshufb      %%xmm5,%%xmm1                 \n"
      "pshufb      %%xmm4,%%xmm0                 \n"
      "pxor        %%xmm6,%%xmm1                 \n"
      "pmaddubsw   %%xmm1,%%xmm0                 \n"
      "psrlw       $0x7,%%xmm0                   \n"
      "pextrw      $0x1,%%xmm2,%k3               \n"
      "pextrw      $0x3,%%xmm2,%k4               \n"
      "packuswb    %%xmm0,%%xmm0                 \n"
      "movq        %%xmm0,(%0)                   \n"
      "lea         0x8(%0),%0                    \n"
      "sub         $0x2,%2                       \n"
      "jge         2b                            \n"

      LABELALIGN
      "29:                                       \n"
      "add         $0x1,%2                       \n"
      "jl          99f                           \n"
      "psrlw       $0x9,%%xmm2                   \n"
      "movq        0x00(%1,%3,4),%%xmm0          \n"
      "pshufb      %%xmm5,%%xmm2                 \n"
      "pshufb      %%xmm4,%%xmm0                 \n"
      "pxor        %%xmm6,%%xmm2                 \n"
      "pmaddubsw   %%xmm2,%%xmm0                 \n"
      "psrlw       $0x7,%%xmm0                   \n"
      "packuswb    %%xmm0,%%xmm0                 \n"
      "movd        %%xmm0,(%0)                   \n"

      LABELALIGN
      "99:                                       \n"  // clang-format error.

      : "+r"(dst_argb),    // %0
        "+r"(src_argb),    // %1
        "+rm"(dst_width),  // %2
        "=&r"(x0),         // %3
        "=&r"(x1)          // %4
      : "rm"(x),           // %5
        "rm"(dx)           // %6
      : "memory", "cc", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6");
}

// Divide num by div and return as 16.16 fixed point result.
int FixedDiv_X86(int num, int div) {
  asm volatile(
      "cdq                                       \n"
      "shld        $0x10,%%eax,%%edx             \n"
      "shl         $0x10,%%eax                   \n"
      "idiv        %1                            \n"
      "mov         %0, %%eax                     \n"
      : "+a"(num)  // %0
      : "c"(div)   // %1
      : "memory", "cc", "edx");
  return num;
}

// Divide num - 1 by div - 1 and return as 16.16 fixed point result.
int FixedDiv1_X86(int num, int div) {
  asm volatile(
      "cdq                                       \n"
      "shld        $0x10,%%eax,%%edx             \n"
      "shl         $0x10,%%eax                   \n"
      "sub         $0x10001,%%eax                \n"
      "sbb         $0x0,%%edx                    \n"
      "sub         $0x1,%1                       \n"
      "idiv        %1                            \n"
      "mov         %0, %%eax                     \n"
      : "+a"(num)  // %0
      : "c"(div)   // %1
      : "memory", "cc", "edx");
  return num;
}

#endif  // defined(__x86_64__) || defined(__i386__)

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif
