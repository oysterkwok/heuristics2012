//
//  match_maker.h
//  HeuristicProblemSolving
//
//  Created by Haoquan Guo on 11/20/12.
//  Copyright 2012 NYU. All rights reserved.
//

#ifndef HeuristicProblemSolving_match_maker_h
#define HeuristicProblemSolving_match_maker_h

#include <vector>
using namespace std;

class match_maker {
public:
	match_maker(int _n_attr);
	void read_init_candidates(string candidate_str);
	vector<float> generate_candidate();
	void read_candidate_score(float score);
protected:
	int n_attr;
	vector<vector<float> > candidates;
	vector<float> scores;
	int n_turn;
};

#endif
