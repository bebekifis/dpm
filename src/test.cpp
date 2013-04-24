/*
 * Copyright (c) 2013, Yinan Yu (bebekifis@gmail.com)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * 
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * Author: Yinan Yu
 * Time: Mar/01/2013
 * Email: bebekifis@gmail.com
 * Filename: test.cpp
 * Description: 
 */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <xmmintrin.h>
// add comment
void test()
{
    const int length = 4;
    float product[128*4]  __attribute__ ((aligned(16)));
    __m128 x = _mm_set_ps(1.0f,2.0f,3.0f,4.0f);
    __m128 y = _mm_set_ps(1.0f,2.0f,3.0f,4.0f);
    __m128 z = _mm_add_ps(x,y);
    _mm_store_ps(product,z);
    fprintf(stderr, "%f %f %f %f\n", product[0], product[1], product[2], product[3]);
}

int main()
{
    test();
    struct timeval tpstart, tpend;
    gettimeofday(&tpstart, NULL);
    float x = 0;
    for (int i = 0; i < 100000000; i+=4)
    {
        x += 0.1;
        x += 0.1;
        x += 0.1;
        x += 0.1;
    }
    gettimeofday(&tpend, NULL);
    double timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec) + (tpend.tv_usec-tpstart.tv_usec);
    printf("%lf, %f\n", timeuse / 1000000.0, x);

}

