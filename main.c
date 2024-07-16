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

#include "tinymembench.h"

int main(void)
{
    int latbench_size = SIZE * 2, latbench_count = LATBENCH_COUNT;
    int64_t *srcbuf, *dstbuf, *tmpbuf;
    void *poolbuf;
    size_t bufsize = SIZE;
#ifdef __linux__
    size_t fbsize = 0;
    int64_t *fbbuf = mmap_framebuffer(&fbsize);
    fbsize = (fbsize / BLOCKSIZE) * BLOCKSIZE;
#endif

    printf("tinymembench v" VERSION " (simple benchmark for memory throughput and latency)\n");


    poolbuf = alloc_four_nonaliased_buffers((void **)&srcbuf, bufsize,
                                            (void **)&dstbuf, bufsize,
                                            (void **)&tmpbuf, BLOCKSIZE,
                                            NULL, 0);
    printf("\n");
    printf("==========================================================================\n");
    printf("== Memory bandwidth tests                                               ==\n");
    printf("==                                                                      ==\n");
    printf("== Note 1: 1MB = 1000000 bytes                                          ==\n");
    printf("== Note 2: Results for 'copy' tests show how many bytes can be          ==\n");
    printf("==         copied per second (adding together read and writen           ==\n");
    printf("==         bytes would have provided twice higher numbers)              ==\n");
    printf("== Note 3: 2-pass copy means that we are using a small temporary buffer ==\n");
    printf("==         to first fetch data into it, and only then write it to the   ==\n");
    printf("==         destination (source -> L1 cache, L1 cache -> destination)    ==\n");
    printf("== Note 4: If sample standard deviation exceeds 0.1%%, it is shown in    ==\n");
    printf("==         brackets                                                     ==\n");
    printf("==========================================================================\n\n");

    bandwidth_bench(dstbuf, srcbuf, tmpbuf, bufsize, BLOCKSIZE, " ", c_benchmarks);
    printf(" ---\n");
    bandwidth_bench(dstbuf, srcbuf, tmpbuf, bufsize, BLOCKSIZE, " ", libc_benchmarks);
    bench_info *bi = get_asm_benchmarks();
    if (bi->f) {
        printf(" ---\n");
        bandwidth_bench(dstbuf, srcbuf, tmpbuf, bufsize, BLOCKSIZE, " ", bi);
    }

#ifdef __linux__
    bi = get_asm_framebuffer_benchmarks();
    if (bi->f && fbbuf)
    {
        printf("\n");
        printf("==========================================================================\n");
        printf("== Framebuffer read tests.                                              ==\n");
        printf("==                                                                      ==\n");
        printf("== Many ARM devices use a part of the system memory as the framebuffer, ==\n");
        printf("== typically mapped as uncached but with write-combining enabled.       ==\n");
        printf("== Writes to such framebuffers are quite fast, but reads are much       ==\n");
        printf("== slower and very sensitive to the alignment and the selection of      ==\n");
        printf("== CPU instructions which are used for accessing memory.                ==\n");
        printf("==                                                                      ==\n");
        printf("== Many x86 systems allocate the framebuffer in the GPU memory,         ==\n");
        printf("== accessible for the CPU via a relatively slow PCI-E bus. Moreover,    ==\n");
        printf("== PCI-E is asymmetric and handles reads a lot worse than writes.       ==\n");
        printf("==                                                                      ==\n");
        printf("== If uncached framebuffer reads are reasonably fast (at least 100 MB/s ==\n");
        printf("== or preferably >300 MB/s), then using the shadow framebuffer layer    ==\n");
        printf("== is not necessary in Xorg DDX drivers, resulting in a nice overall    ==\n");
        printf("== performance improvement. For example, the xf86-video-fbturbo DDX     ==\n");
        printf("== uses this trick.                                                     ==\n");
        printf("==========================================================================\n\n");

        srcbuf = fbbuf;
        if (bufsize > fbsize)
            bufsize = fbsize;
        bandwidth_bench(dstbuf, srcbuf, tmpbuf, bufsize, BLOCKSIZE, " ", bi);
    }
#endif

    free(poolbuf);

    printf("\n");
    printf("==========================================================================\n");
    printf("== Memory latency test                                                  ==\n");
    printf("==                                                                      ==\n");
    printf("== Average time is measured for random memory accesses in the buffers   ==\n");
    printf("== of different sizes. The larger is the buffer, the more significant   ==\n");
    printf("== are relative contributions of TLB, L1/L2 cache misses and SDRAM      ==\n");
    printf("== accesses. For extremely large buffer sizes we are expecting to see   ==\n");
    printf("== page table walk with several requests to SDRAM for almost every      ==\n");
    printf("== memory access (though 64MiB is not nearly large enough to experience ==\n");
    printf("== this effect to its fullest).                                         ==\n");
    printf("==                                                                      ==\n");
    printf("== Note 1: All the numbers are representing extra time, which needs to  ==\n");
    printf("==         be added to L1 cache latency. The cycle timings for L1 cache ==\n");
    printf("==         latency can be usually found in the processor documentation. ==\n");
    printf("== Note 2: Dual random read means that we are simultaneously performing ==\n");
    printf("==         two independent memory accesses at a time. In the case if    ==\n");
    printf("==         the memory subsystem can't handle multiple outstanding       ==\n");
    printf("==         requests, dual random read has the same timings as two       ==\n");
    printf("==         single reads performed one after another.                    ==\n");
    printf("==========================================================================\n");

    if (!latency_bench(latbench_size, latbench_count, -1) ||
        !latency_bench(latbench_size, latbench_count, 1))
    {
        latency_bench(latbench_size, latbench_count, 0);
    }

    return 0;
}
