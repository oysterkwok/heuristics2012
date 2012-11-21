//
//  dating_person.h
//  HeuristicProblemSolving
//
//  Created by Haoquan Guo on 11/20/12.
//  Copyright 2012 NYU. All rights reserved.
//

#ifndef HeuristicProblemSolving_dating_person_h
#define HeuristicProblemSolving_dating_person_h

#include <vector>
using namespace std;

class dating_person {
public:
	dating_person(int _n_attr);
	vector<float> generate_weights();
	vector<float> generate_noise();
protected:
	int n_attr;
	vector<int> weights;
	vector<int> last_noise;
	int n_turn;
};


#endif
