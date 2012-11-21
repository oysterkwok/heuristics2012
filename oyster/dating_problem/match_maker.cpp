//
//  match_maker.cpp
//  HeuristicProblemSolving
//
//  Created by Haoquan Guo on 11/20/12.
//  Copyright 2012 NYU. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <math.h>
#include "match_maker.h"

match_maker::match_maker(int _n_attr) : n_attr(_n_attr), n_turn(0) {
}

void match_maker::read_init_candidates(string candidate_str) {
	istringstream in(candidate_str.c_str());
	char temp = ' ';
	float num;
	for (int i = 0; i < 20; i++) {
		in >> temp; // toss the '['
		vector<float> new_candidate;
		for (int j = 0; j < n_attr; j++) {
			in >> num;
			in >> temp; // toss the ',' or ']'
			new_candidate.push_back(num);
		}
		in >> num;
		scores.push_back(num);
		in >> temp; // toss ','
		while (isspace(in.get())); // toss white space between ',' and '['
		candidates.push_back(new_candidate);
	}
}

vector<float> match_maker::generate_candidate() {
	vector<float> dps;
	dps.resize(n_attr, 0);
		
	for (int i = 0; i < candidates.size(); i ++) {
		int amp = 20;
		if (i >= 20) {
			amp = i;
		}
		float sum = 0;
		for (int j = 0; j < n_attr; j ++) {
			sum += candidates[i][j];
		}
		if (sum > 0) {
			for (int j = 0; j < n_attr; j ++) {
				dps[j] += scores[i] / sum * candidates[i][j] * amp / candidates.size();
			}
		}
		if (sum < n_attr) {
			for (int j = 0; j < n_attr; j ++) {
				dps[j] += -scores[i] / (n_attr-sum) * (1-candidates[i][j])* amp / candidates.size();
			}
		}
	}


	cout << "[DPS] ";
	for (int i = 0; i < n_attr; i ++) {
		cout << (int) (dps[i] * 100) << " ";
	}
	cout << "\n";

	int ac_rate = 150 - (int) candidates.size() * 2;
	
	int change_count = 0;
	
	vector<float> candidate;
	candidate.resize(n_attr, 0);
	for (int i = 0; i < n_attr; i ++) {
		int roll = rand() % 100;
		if (dps[i] > 0) {
			candidate[i] = 1;
		}
		if (fabs(roll * dps[i] * 100) < ac_rate) {
			candidate[i] = 1 - candidate[i];
			if (dps[i] != 0) {
				change_count ++;
			}
		}
	}
	candidates.push_back(candidate);
	
	cout << "[CGC] " << change_count << "\n";
	cout << "[CGS] ";
	for (int i = 0; i < n_attr; i ++) {
		if ((candidate[i] == 1 && dps[i] < 0) || (candidate[i] == 0 && dps[i] > 0)) {
			cout << (int) (100*dps[i]) << " ";
		}
		else {
			cout << "0 ";
		}
	}
	cout << "\n";
	
	return candidate;
}

void match_maker::read_candidate_score(float score) {
	scores.push_back(score);
}
