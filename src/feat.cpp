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
 * Time: 06/Mar/2013
 * Email: bebekifis@gmail.com
 * Filename: feat.cpp
 * Description: 
 */
#include "feat.h"

Pyramid featpyramid(Tensor im, Model m, int padx, int pady)
{
	Pyramid pyra;
	if (padx < 0 || pady < 0)
	{
		padx = m.getpadding(0);
		pady = m.getpadding(1);
	}
	int extra_interval = 0;
	if (m.features.extra_octave)
		extra_interval = m.interval;
	int sbin = m.sbin;
	int interval = m.interval;
	float sc = pow(2.0, 1.0/interval);
	int imsize[2] = {im.size(0), im.size(1)};
	float max_scale = 1 + floor(log(min(imsize[0], imsize[1])/(5*sbin))/log(sc));
	pyra.feat = vector<Tensor>((int)max_scale + extra_interval + interval, Tensor());
	pyra.scales = vector<float>((int)max_scale + extra_interval + interval, 0);
	pyra.imsize = {imsize[0], imsize[1]};
	for (int i = 0; i < interval; i++)
	{
		scaled = im.resize(1.0/sc^(i-1));
		if (extra_interval > 0)
		{
			pyra.feat[i] = features(scaled, sbin/4);
			pyra.scales[i] = 4.0/pow(sc, (i));
		}
		pyra.feat[i+extra_interval] = features(scaled, sbin/2);
		pyra.scales[i+extra_interval] = 2.0/pow(sc, i);

		for (int j = i+interval; j < max_scale; j+=interval)
		{
			Tensor tmp = scaled;
			scaled = tmp.resize(0.5);
			tmp.release();
			pyra.feat[j+extra_interval+interval] = features(scaled, sbin);
			pyra.scales[j+extra_interval+interval] = 0.5 * pyra.scales(j + extra_interval);
		}
	}
	pyra.num_levels = pyra.feat.size();
	int td = m.features.truncation_dim;
	for (int i = 0; i < pyra.num_levels; i++)
	{
		pyra.feat[i].padarray(pady+1, padx+1, 0, 0);
		for (int y = 0; y < pady+1; y++)
		{
			for (int x = 0; x < pyra.feat[i]->size(1); x++)
			{
				(pyra.feat[i])[y, x, td-1] = 1;
			}
		}
		for (int y = pyra.feat[i].size(0) - pady - 1; y < pyra.feat[i].size(0); y++)
		{
			for (int x = 0; x < pyra.feat[i]->size(1); x++)
			{
				(pyra.feat[i])[y, x, td-1] = 1;
			}
		}
		for (int y = 0; y < pyra.feat[i].size(0); y++)
		{
			for (int x = 0; x < padx + 1; x++)
			{
				(pyra.feat[i])[y, x, td-1] = 1;
			}
		}
		for (int y = 0; y < pyra.feat[i].size(0); y++)
		{
			for (int x = pyra.feat[i].size(1) - padx - 1; x < pyra.feat[i].size(1); x++)
			{
				(pyra.feat[i])[y, x, td-1] = 1;
			}
		}
	}
	pyra.valid_levels = vector<int>(pyra.num_levels, 1);
	pyra.padx = padx;
	pyra.pady = pady;
	return pyra;
}

// small value, used to avoid division by zero
#define eps 0.0001

// unit vectors used to compute gradient orientation
double uu[9] = {1.0000, 
	0.9397, 
	0.7660, 
	0.500, 
	0.1736, 
	-0.1736, 
	-0.5000, 
	-0.7660, 
	-0.9397};
double vv[9] = {0.0000, 
	0.3420, 
	0.6428, 
	0.8660, 
	0.9848, 
	0.9848, 
	0.8660, 
	0.6428, 
	0.3420};

static inline float min(float x, float y) { return (x <= y ? x : y); }
static inline float max(float x, float y) { return (x <= y ? y : x); }

static inline int min(int x, int y) { return (x <= y ? x : y); }
static inline int max(int x, int y) { return (x <= y ? y : x); }


Tensor features(Tensor im, int sbin)
{
	float *im = (float *)im.data;
	const int * dims = im.getsize();
	fassert(dims[2] == 3, "features: input must be a color image: dims[2]=%d\n", dims[2]);

	// memory for caching orientation histograms & their norms
	int blocks[2];
	blocks[0] = (int)round((double)dims[0]/(double)sbin);
	blocks[1] = (int)round((double)dims[1]/(double)sbin);
	float *hist = new float[blocks[0]*blocks[1]*18];
	float *norm = new float[bocks[0]*blocks[1]];

	// memory for HOG features
	int out[3];
	out[0] = max(blocks[0]-2, 0);
	out[1] = max(blocks[1]-2, 0);
	out[2] = 27+4+1;
	Tensor mxfeat = Tensor(3, out);
	float *feat = (float *)mxfeat.data;

	int visible[2];
	visible[0] = blocks[0]*sbin;
	visible[1] = blocks[1]*sbin;

	for (int x = 1; x < visible[1]-1; x++) {
		for (int y = 1; y < visible[0]-1; y++) {
			// first color channel
			double *s = im + min(x, dims[1]-2)*dims[0] + min(y, dims[0]-2);
			double dy = *(s+1) - *(s-1);
			double dx = *(s+dims[0]) - *(s-dims[0]);
			double v = dx*dx + dy*dy;

			// second color channel
			s += dims[0]*dims[1];
			double dy2 = *(s+1) - *(s-1);
			double dx2 = *(s+dims[0]) - *(s-dims[0]);
			double v2 = dx2*dx2 + dy2*dy2;

			// third color channel
			s += dims[0]*dims[1];
			double dy3 = *(s+1) - *(s-1);
			double dx3 = *(s+dims[0]) - *(s-dims[0]);
			double v3 = dx3*dx3 + dy3*dy3;

			// pick channel with strongest gradient
			if (v2 > v) {
				v = v2;
				dx = dx2;
				dy = dy2;
			} 
			if (v3 > v) {
				v = v3;
				dx = dx3;
				dy = dy3;
			}

			// snap to one of 18 orientations
			double best_dot = 0;
			int best_o = 0;
			for (int o = 0; o < 9; o++) {
				double dot = uu[o]*dx + vv[o]*dy;
				if (dot > best_dot) {
					best_dot = dot;
					best_o = o;
				} else if (-dot > best_dot) {
					best_dot = -dot;
					best_o = o+9;
				}
			}

			// add to 4 histograms around pixel using linear interpolation
			double xp = ((double)x+0.5)/(double)sbin - 0.5;
			double yp = ((double)y+0.5)/(double)sbin - 0.5;
			int ixp = (int)floor(xp);
			int iyp = (int)floor(yp);
			double vx0 = xp-ixp;
			double vy0 = yp-iyp;
			double vx1 = 1.0-vx0;
			double vy1 = 1.0-vy0;
			v = sqrt(v);

			if (ixp >= 0 && iyp >= 0) {
				*(hist + ixp*blocks[0] + iyp + best_o*blocks[0]*blocks[1]) += 
					vx1*vy1*v;
			}

			if (ixp+1 < blocks[1] && iyp >= 0) {
				*(hist + (ixp+1)*blocks[0] + iyp + best_o*blocks[0]*blocks[1]) += 
					vx0*vy1*v;
			}

			if (ixp >= 0 && iyp+1 < blocks[0]) {
				*(hist + ixp*blocks[0] + (iyp+1) + best_o*blocks[0]*blocks[1]) += 
					vx1*vy0*v;
			}

			if (ixp+1 < blocks[1] && iyp+1 < blocks[0]) {
				*(hist + (ixp+1)*blocks[0] + (iyp+1) + best_o*blocks[0]*blocks[1]) += 
					vx0*vy0*v;
			}
		}
	}

	// compute energy in each block by summing over orientations
	for (int o = 0; o < 9; o++) {
		float *src1 = hist + o*blocks[0]*blocks[1];
		float *src2 = hist + (o+9)*blocks[0]*blocks[1];
		float *dst = norm;
		float *end = norm + blocks[1]*blocks[0];
		while (dst < end) {
			*(dst++) += (*src1 + *src2) * (*src1 + *src2);
			src1++;
			src2++;
		}
	}

	// compute features
	for (int x = 0; x < out[1]; x++) {
		for (int y = 0; y < out[0]; y++) {
			float *dst = feat + x*out[0] + y;      
			float *src, *p, n1, n2, n3, n4;

			p = norm + (x+1)*blocks[0] + y+1;
			n1 = 1.0 / sqrt(*p + *(p+1) + *(p+blocks[0]) + *(p+blocks[0]+1) + eps);
			p = norm + (x+1)*blocks[0] + y;
			n2 = 1.0 / sqrt(*p + *(p+1) + *(p+blocks[0]) + *(p+blocks[0]+1) + eps);
			p = norm + x*blocks[0] + y+1;
			n3 = 1.0 / sqrt(*p + *(p+1) + *(p+blocks[0]) + *(p+blocks[0]+1) + eps);
			p = norm + x*blocks[0] + y;      
			n4 = 1.0 / sqrt(*p + *(p+1) + *(p+blocks[0]) + *(p+blocks[0]+1) + eps);

			float t1 = 0;
			float t2 = 0;
			float t3 = 0;
			float t4 = 0;

			// contrast-sensitive features
			src = hist + (x+1)*blocks[0] + (y+1);
			for (int o = 0; o < 18; o++) {
				float h1 = min(*src * n1, 0.2);
				float h2 = min(*src * n2, 0.2);
				float h3 = min(*src * n3, 0.2);
				float h4 = min(*src * n4, 0.2);
				*dst = 0.5 * (h1 + h2 + h3 + h4);
				t1 += h1;
				t2 += h2;
				t3 += h3;
				t4 += h4;
				dst += out[0]*out[1];
				src += blocks[0]*blocks[1];
			}

			// contrast-insensitive features
			src = hist + (x+1)*blocks[0] + (y+1);
			for (int o = 0; o < 9; o++) {
				float sum = *src + *(src + 9*blocks[0]*blocks[1]);
				float h1 = min(sum * n1, 0.2);
				float h2 = min(sum * n2, 0.2);
				float h3 = min(sum * n3, 0.2);
				float h4 = min(sum * n4, 0.2);
				*dst = 0.5 * (h1 + h2 + h3 + h4);
				dst += out[0]*out[1];
				src += blocks[0]*blocks[1];
			}

			// texture features
			*dst = 0.2357 * t1;
			dst += out[0]*out[1];
			*dst = 0.2357 * t2;
			dst += out[0]*out[1];
			*dst = 0.2357 * t3;
			dst += out[0]*out[1];
			*dst = 0.2357 * t4;

			// truncation feature
			dst += out[0]*out[1];
			*dst = 0;
		}
	}

	delete [] hist;
	delete [] norm
	return mxfeat;
}
