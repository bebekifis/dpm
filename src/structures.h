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
 * Time: Feb/28/2013
 * Email: bebekifis@gmail.com
 * Filename: structures.h
 * Description: 
 */

#ifndef __STRUCTURES_H__
#define __STRUCTURES_H__

class Filter
{
    public:
        int blocklabel;
        int size[2];
        int flip;
        int symbol;
};

typedef struct OFFSET{
    int blocklabel;
} Offset;

typedef struct LOC
{
    int blocklabel;
} Loc;

class Rule
{
    public:

    char type;
    vector<int> lhs;
    vector<int> rhs;
    int detwindow[2];
    int shiftwindow[2];
    int i;
    vector<int[3]> anchor;
    Offset offset;
    Loc loc;
    vector<int> blocks;
};

class Symbol
{
    public:

        char type;
        int filter;
};

class Block
{
    public:
        Tensor * w;
        Tensor * lb;
        int learn;
        int reg_mult;
        int dim;
        int shape[3];
        char type;
};

class Feature
{
    int sbin;
    int dim;
    int truncation_dim;
    int extra_octave;
    int bias;
};

class Stat
{
    vector<float> slave_problem_time;
    vector<float> data_mining_time;
    vector<float> pos_latent_time;
    vector<float> fitler_usage;
};

class Bboxpred
{
    float x1;
    float x2;
    float y1;
    float y2;
};

#endif    // #ifndef __STRUCTURES_H__

