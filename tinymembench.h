#pragma once
/*
 * Copyright © 2011 Siarhei Siamashka <siarhei.siamashka@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#endif

#include "util.h"
#include "asm-opt.h"
#include "version.h"

#define SIZE (32 * 1024 * 1024)
#define BLOCKSIZE 2048
#ifndef MAXREPEATS
#define MAXREPEATS 10
#endif
#ifndef LATBENCH_COUNT
#define LATBENCH_COUNT 10000000
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __linux__
    void *mmap_framebuffer(size_t *fbsize);
#endif

    double bandwidth_bench_helper(int64_t *dstbuf, int64_t *srcbuf,
                                  int64_t *tmpbuf,
                                  int size, int blocksize,
                                  const char *indent_prefix,
                                  int use_tmpbuf,
                                  void (*f)(int64_t *, int64_t *, int),
                                  const char *description);

    void memcpy_wrapper(int64_t *dst, int64_t *src, int size);

    void memset_wrapper(int64_t *dst, int64_t *src, int size);

    static bench_info c_benchmarks[] =
        {
            {"C copy backwards", 0, aligned_block_copy_backwards},
            {"C copy backwards (32 byte blocks)", 0, aligned_block_copy_backwards_bs32},
            {"C copy backwards (64 byte blocks)", 0, aligned_block_copy_backwards_bs64},
            {"C copy", 0, aligned_block_copy},
            {"C copy prefetched (32 bytes step)", 0, aligned_block_copy_pf32},
            {"C copy prefetched (64 bytes step)", 0, aligned_block_copy_pf64},
            {"C 2-pass copy", 1, aligned_block_copy},
            {"C 2-pass copy prefetched (32 bytes step)", 1, aligned_block_copy_pf32},
            {"C 2-pass copy prefetched (64 bytes step)", 1, aligned_block_copy_pf64},
            {"C fill", 0, aligned_block_fill},
            {"C fill (shuffle within 16 byte blocks)", 0, aligned_block_fill_shuffle16},
            {"C fill (shuffle within 32 byte blocks)", 0, aligned_block_fill_shuffle32},
            {"C fill (shuffle within 64 byte blocks)", 0, aligned_block_fill_shuffle64},
            {NULL, 0, NULL}};

    static bench_info libc_benchmarks[] =
        {
            {"standard memcpy", 0, memcpy_wrapper},
            {"standard memset", 0, memset_wrapper},
            {NULL, 0, NULL}};

    void bandwidth_bench(int64_t *dstbuf, int64_t *srcbuf, int64_t *tmpbuf,
                         int size, int blocksize, const char *indent_prefix,
                         bench_info *bi);

    void __attribute__((noinline)) random_read_test(char *zerobuffer,
                                                    int count, int accessRange);

    void __attribute__((noinline)) random_dual_read_test(char *zerobuffer,
                                                         int count, int accessRange);

    uint32_t rand32();

    int latency_bench(int size, int count, int use_hugepage);

#ifdef __cplusplus
}
#endif