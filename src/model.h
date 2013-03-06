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
 * Filename: model.h
 * Description: 
 */

#ifndef __MODEL_H__
#define __MODEL_H__

#include "structures.h"

class Model
{
    protected:
        void _gdetect_dp();
        void _gdetect_parse();
        void _gdetect_pos();
        void _gdetect_prepare();
        void _gdetect_write();
        int _sort(vector<int> & L, vector<int> & V);
        
        
        vector<vector<Rule> *> _rules;
    public:
        string class;
        string year;
        string note;
        
        vector<Filter> filters;
        vector<Symbol>  symbols;
        vector<Block> blocks;
        vector<Feature> features;
        vector<stat> stats;
        vector<Bboxpred> bboxpred;
        
        int numfilters;
        int numblocks;
        int numsymbols;
        int start;
        int maxsize;
        int minsize;
        int interval;
        int sbin;
        float thresh;
        char type;
    public:
        Model();
        ~Model();
        void load();
        void imgdetect();

        void gdetect();
        void filter_responses();
        void apply_rule();
        void apply_deformable_rule();
        void apply_structure_rule();
        void get_block();
        void loc_feat();
        void get_detection_tree();
        void symbol_score();
        void add_block();
        void add_def_rule();
        void add_nonterminal();
        void add_parts();
        void add_struct_rule();
        void add_symbol();
        void add_terminal();
        void create();
        void merge();
        void sort();
        void types();
        static Model root_model();
        static Model lr_root_model();
}

#endif    // #ifndef __MODEL_H__

