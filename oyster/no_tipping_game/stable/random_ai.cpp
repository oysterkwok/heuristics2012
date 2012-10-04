//
//  random_ai.cpp
//  HeuristicProblemSolving
//
//  Created by Haoquan Guo on 10/4/12.
//  Copyright 2012 NYU. All rights reserved.
//

#include <iostream>
#include "game_state.h"

using namespace std;

class random_ai {
public:
	random_ai(string name) : ai_name(name) {
	}
	
	pair<int, int> ai_move(game_state state) {
		vector<pair<int, int> > all_moves = state.get_all_no_tipping_moves();
		if (all_moves.size() == 0) {
			// if all moves would tip, surrender
			return pair<int, int>(-1, 16);
		}
		return all_moves[rand() % all_moves.size()];
	}
	
	string get_name() {
		return ai_name;
	}
protected:
	string ai_name;
};

