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
 * Time: 07/Mar/2013
 * Email: bebekifis@gmail.com
 * Filename: tensor.cpp
 * Description: 
 */
#include "tensor.h"

Tensor::Tensor()
{

}

Tensor::~Tensor()
{

}

Tensor::Tensor(int, int *)
{

}

Tensor::Tensor(float *, int, int *, int);
{
	
}

int Tensor::release()
{
	
}

int Tensor::resize(int dim, int * size)
{
}

int Tensor::padding(int pady, int padx, int padc)
{
	int new_size[3] = {_size[0]+2*pady, _size[1]+2*padx, _size[2]+2*padc};
	int new_total = new_size[0]*new_size[1]*new_size[2];
	float * new_data = new float[new_total];
	memset(new_data, 0, sizeof(float)*new_total);
	for (int c = padc; c < _size[2] + padc; c++)
	{
		for (int x = padx; x < _size[1] + padx; x++)
		{
			// y = 0;
			memcpy(&new_data[c*new_size[1]*new_size[0] + x*new_size[0] + pady],
					&data[(c-padc)*_size[1]*_size[0] + (x-padx)*_size[0]], sizeof(float)*_size[0]);
		}
	}
}

const int * Tensor::getsize()
{
	return _size;
}

int Tensor::getsize(int idx)
{
	fassert(idx < _dim, "Tensor::getsize: idx(%d) < _dim(%d)\n", idx, _dim);
	return _size[idx];
}

int Tensor::getdim()
{
	return _dim;
}
