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
 * Filename: model.cpp
 * Description: 
 */
#include "model.h"


Model::Model()
{

}

Model::~Model()
{
}

// Matlab comments
// Detect objects in a feature pyramid using a model and a score threshold.
// Higher thresholds lead to fewer detections.
//   [ds, bs, trees] = gdetect(pyra, model, thresh, max_num)
//
// Return values (more details are below)
//   ds        Detection windows
//   bs        Bounding boxes for all filters used in each detection
//   trees     Derivation trees corresponding to each detection
//
// Arguments
//   pyra      Feature pyramid to get detections from (output of featpyramid.m)
//   model     Model to use for detection
//   thresh    Detection threshold (scores must be > thresh)
//   max_num   Maximum number of detections to return
//
// ds
//   A matrix with 6 columns and one row per detection.  Columns 1-
//   give the pixel coordinates (x1,y1,x2,y2) of each detection bounding box.  
//   Column 5 specifies the model component used for each detection and column 
//   6 gives the score of each detection.
//
// bs 
//   A matrix with one row per detection and each sequential group
//   of columns specifies the pixel coordinates of each model filter bounding
//   box (i.e., where the parts were placed).  The index in the sequence is
//   the same as the index in model.filters.
//
// trees
//   Detailed information about each detection required for extracted feature 
//   vectors during learning. Each entry in trees describes the derivation
//   tree, under the grammar model, that corresponds to each detection.k



int Model::gdetect(vector<Tensor> pyra, float thresh, int max_num, Tensor ds, Tensor bs, Tree trees)
{
    _gdetect_dp(pyra);
    _gdetect_parse(pyra, thresh, max_num, ds, bs, trees);
    return 0;
}

// Matlab comments
// Compute dynamic programming tables used for finding detections.
//   model = gdetect_dp(pyra, model)
//
//   This function implements the dynamic programming algorithm for
//   computing high-scoring derivations using an Object Detection Grammar.
//   It is assumed that the detection grammar is an Isolated Deformation
//   Grammar and therefore contains only structural schemas and deformation
//   schemas.
//
// Return value
//   model   Object model augmented to store the dynamic programming tables
//
// Arguments
//   pyra    Feature pyramid returned by featpyramid.m
//   model   Object model

int Model::imgdetect(Tensor im, float thresh, int max_num, vector<Tensor> & ds, vector<Tensor> & bs, vector<Tree> & trees)
{
    im = color(im);
    Pyramid pyra = featpyramid(im, *this);
    gdetect(pyra, thresh, max_num, ds, bs, trees);
}

int Model::gdetect(Pyramid pyra, float thresh, int max_num, vector<Tensor> & ds, vector<Tensor> & bs, vector<Tree> & trees)
{
    _gdetect_dp(pyra);
    _gdetect_parse(pyra, thresh, max_num, ds, bs, trees);
}


int Model::_gdetect_dp(Pyramid pyra)
{
    vector<int> L;
    vector<int> V;
    _filter_response(pyra);
    _sort(L, V);
    for (int i = 0; i < L.size(); i++)
    {
        int s = L[i];
        for (int j = 0; j < _rules.size(); i++)
        {
            _apply_rule(_rules[s], pyra.pady, pyra.padx);
        }
        _symbol_score(s, pyra);
    }
    return 0;
}

int Model::_gdetect_parse(Pyramid pyra, float thresh, int max_num, vector<Tensor> & ds, vector<Tensor> & bs, vector<Tree> trees)
{
    //to do
}

int Model::_sort(int i, vector<int> & L, vector<int> & V)
{
    if (i < 0)
    {
        i = start;
        L.clear();
        V.clear();
        for (int j = 0; j < numsymbols; j++)
        {
            V.push_back(0);
        }
    }
    if (V[i] == 0) // V[i] == 1
    {
        fprintf(stderr, "Cycle detected in grammar\n");
        exit(-1);
    }
    V[i] = 0; // V[i] = 1
    for (int r = 0; r < rules[i].size(); r++)
    {
        for (int is = 0; is < rules[i][r].rhs.size(); is++)
        {
            int s = rules[i][r].rhs[is];
            if (symbols[s].type == 'N' && V[s] < 1) // V[s] < 2
                _sort(s, L, V);
        }
    }
    V[i] = 2;
    L.push_back(i);
}

int Model::_filter_response(Pyramid pyra)
{
    vector<Filter> filters;
    for (int i = 0; i < numfilters; i++)
    {
        Filter filter = _get_block(_filters[i]);
        filters.push_back(filter);
    }
    for (int level = 0; i < pyra.feat.size(); i++)
    {
        if (!pyra.valid_levels[level])
        {
            score[level] = 0;
            for (int i = 0; i < numfilters; i++)
            {
                _symbols[filters[i].symbol].score[level] = Tensor(-INF);
            }
        }
        vector<Tensor> r;
        fconvsse(pyra.feat[level], filters, 1, filters.size(), r);
        int s[2] = {-INF, -INF};
        for (int i = 0; i < r.size(); i++)
        {
            s[0] = max(s[0], r[i].getrows());
            s[1] = max(s[1], r[i].getcols());
        }
        for (int i = 0; i < r.size(); i++)
        {
            int spady = s[0] - r[i].getrows();
            int spadx = s[1] - r[i].getcols();
            r[i].padarray(spady, spadx, 0, -INF, 'post');
            int fsym = filters[i].symbol;
            symbols[fsym].score[level] = r[i];
        }
        scoretpt[level] = Tensor::zeros(s);
    }

}

int Model::_symbol_score(int s, Pyramid pyra)
{
    vector<Rule> rules = _rules[s];
    Tensor score = rules[0].score.clone();
    for (int i = 1; i < rules.size(); i++)
    {
        Rule r = rules[i];
        for (int j = 0; j < r.score.size(); j++)
        {
            score[i].max(r.score[i]);
        }
    }
    _symbols[s].score.copy(score);
    score.release();
}

int Model::_apply_rule(Rule r, int pady, int padx)
{
    if (r.type == 'S')
        _apply_structural_rule(r, pady, padx);
    else
        _apply_deformable_rule(r);
}

int Model::_apply_structure_rule(Rule r, int pady, int padx)
{
    //pass
    return 0;
}
    
int Model::_apply_deformable_rule(Rule, r, int pady, int padx)
{
    Tensor def_w = _get_block(r.def);
    vector<Tensor> score = symbols[r.rhs[0]].score;
    Tensor bias  = _get_block(r.offset) * features.bias;
    Tensor loc_w = _get_block(r.loc);
    Tensor loc_f = _loc_feat(score.size());
    Tensor loc_scores = loc_w * loc_f;
    vector<Tensor> Ix;
    vector<Tensor> Iy;
    for (int i = 0; i < score.size(); i++)
    {
        Tensor ix, iy;

        score[i] = score[i] + bias + loc_scores[i];
        bounded_dt(score[i], 
                def_w[0], def_w[1], def_w[2], def_w[3], 4,
                ix[i], iy[i]);
        Ix.push_back(ix);
        Iy.push_back(iy);
    }
    rules[r.lhs][r.i].score = score;
    rules[r.lhs][r.i].Iy = Iy;
    rules[r.lhs][r.i].Ix = Ix;


}
