//
//  human_ai.cpp
//  HeuristicProblemSolving
//
//  Created by Haoquan Guo on 10/4/12.
//  Copyright 2012 NYU. All rights reserved.
//

#include <iostream>
#include "game_state.h"

using namespace std;

class human_ai {
public:
	human_ai(string name) : ai_name(name) {
	}
	
	pair<int, int> ai_move(game_state state) {
		while (true) {
			vector<int> blocks = state.get_available_blocks();
			cout << "available blocks:";
			for (int i = 0; i < blocks.size(); i ++) {
				cout << " " <<  blocks[i];
			}
			cout << "\n";
			cout << "input your move: [block_id] [slot_position]\n";
			int block_id, slot_position;
			cin >> block_id >> slot_position;
			cout << "got <" << block_id << ", " << slot_position << ">\n";
			pair<int, int> move = pair<int, int>(block_id, slot_position);
			if (state.move_any(move).game_turn > state.game_turn) {
				return move;
			}
			else {
				cout << "invalid move.\n";
				cout << state.graphic_output();
			}
		}
	}
	string get_name() {
		return ai_name;
	}
	
protected:
	string ai_name;
};
