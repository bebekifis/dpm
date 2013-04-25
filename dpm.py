# Copyright (c) 2013, Yinan Yu (bebekifis@gmail.com)
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# - Redistributions of source code must retain the above copyright notice,
#   this list of conditions and the following disclaimer.
#
# - Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Author: Yinan Yu
# Time: Apr/25/2013
# Email: bebekifis@gmail.com
# Filename: dpm.py
# Description:

import numpy as np

class dpm:
    def __init__(self, op, params={}):
        self.model = []

    def imgdetect(self, im):
        pass

    def gdetect(self, pyra, thresh=0, max_num=np.inf):
        self.gdetect_dp(pyra)
        ds, bs, trees = gdetect_parse(pyra, thresh, max_num)

    def gdetect_dp(self, pyra):
        self.filter_response(pyra)

        L = self.sort()
        for s in L:
            for r in self.model.rules[s]:
                self.apply_rule(r, pyra.pady, pyra.padx)
            self.symbol_score(s, pyra)

    def gdetect_parse(self):

        pass

    def filter_respose(self, pyra):
        filters = []
        for i in xrange(self.model.numfilters):
            filters.append(self.get_block(self.model.filters[i]))

        for level in xrange(pyra.feat):
            if not pyra.valid_levels(level):
                self.model.scoretpt[level] = 0
                for i in xrange(self.model.numfilters):
                    self.model.symbols[self.model.filters[i].symbol].score[level] = -np.inf
                continue
            # resp is a list of filter response
            if pyra.feat[level].shape[2] == 32:
                resp = conv.fconv(pyra.feat[level], filters)
            else:
                resp = conv.fconv_var_dim(pyra.feat[level], filters)

            s = np.array((-np.inf, np.inf))
            for r in resp:
                s = np.maximum(s, r.shape)

            for i, r in enumerate(resp):
                spady = s[0] - r.shape[0]
                spadx = s[1] - r.shape[1]
                resp[i] = np.pad(r, ((0,spady), (0,spadx)), 'constant', constant_value=-100000)
                fsym = self.model.filters[i].symbol
                self.model.symbols[fsym].score[leve] = r

            self.model.scoretpt[level] = np.zeros(s)

    def sort(self, i=self.model.start, L=[], V=np.zeros((self.model.symbols,1))):
        V[i] = 0
        for r, rule in enumerate(self.model.rules[i]):
            for s in enumerate(self.model.rules[i][r].rhs):
                if self.model.symbols[s].type == 'N' and V[s] < 2:
                    L, V = self.sort(s, L, V)
        V[i] = 2
        L.append(i)
        return L, V

    def symbol_score(self, s, pyra):
        rules = self.model.rules[s]
        score = rules[0].score

        for r in rules[1:]:
            for i in xrange(len(r.score)):
                score[i] = np.maximum(score[i], r.score[i])
        self.model.symbols[s].score = score

    def apply_rule(self, r, pady, padx):
        if r.type == 'S':
            pass
        else:
            self.apply_deformable_rule(r)

    def apply_deformabel_rule(self, r):
        def_w = self.get_block(r.def)
        score = self.symbols(r.rhs[0]).score
        bias  = self.get_block(r.offset) * self.model.features.bias
        loc_w = self.get_block(r.loc)
        loc_f = self.loc_feat(len(score))
        loc_scores = loc_w * loc_f

        for i, s in enumerate(score):
            s += + bias + loc_scores[i]
            s, Ix[i], Iy[i] = bounded_dt(s, def_w[0], def_w[1], def_w[2], def_w[3], 4)

        self.model.rules[r.lhs][r.i].score = score
        self.model.rules[r.lhs][r.i].Ix = Ix
        self.model.rules[r.lhs][r.i].Iy = Iy

