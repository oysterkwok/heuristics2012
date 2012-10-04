//
//  game_ai.h
//  HeuristicProblemSolving
//
//  Created by Haoquan Guo on 10/4/12.
//  Copyright 2012 NYU. All rights reserved.
//

#include "game_state.h"

#ifndef HeuristicProblemSolving_game_ai_h
#define HeuristicProblemSolving_game_ai_h

using namespace std;

class game_ai {
protected:
	string ai_name;
	game_ai(string name) : ai_name(name) {
	}
	
public:	
//	virtual pair<int, int> ai_move(game_state &state);
	string get_name() {
		return ai_name;
	}
};

#endif
