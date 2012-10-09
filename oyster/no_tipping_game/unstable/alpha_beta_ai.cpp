//
//  alpha_beta_ai.cpp
//  HeuristicProblemSolving
//
//  Created by Haoquan Guo on 10/9/12.
//  Copyright 2012 NYU. All rights reserved.
//

#include <iostream>
#include "game_state.h"

using namespace std;

class alpha_beta_ai {
public:
	alpha_beta_ai(string name) : ai_name(name) {
	}
	
	pair<int, int> ai_move(game_state state) {
		vector<pair<int, int> > all_moves = state.get_all_no_tipping_moves();
		if (all_moves.size() == 0) {
			// if all moves would tip, surrender
			return pair<int, int>(-1, 16);
		}
		
		// find best solution
		int alpha = -1000;
		int beta = 1000;
		int best_index = 0;

		if (state.game_turn < 15) {
			ai_depth = 3;
		}
		else {
			ai_depth = 4 + (state.game_turn-15) / 3;
		}
		
		cout << "[[TURN:" << state.game_turn << "]]\n";
		cout << "======= depth: " << ai_depth << " ==========\n";
		time_t s_time = time(NULL);
		
		for (int ii = 0; ii < all_moves.size(); ii ++) {
			int i = ii;
			if (ai_depth % 2 == 0) {
				i = (int) all_moves.size() - ii - 1;
			}
			int this_value = alpha_beta(state.move_fast(all_moves[i]), ai_depth-1, alpha, beta);
//			cout << all_moves[i].first << "," << all_moves[i].second << " : " << this_value << "\n";
			if (ai_depth % 2 == 0) {
				// minimax 2(2,3), 1(0), 3(1,4,1)->first
				if (this_value > alpha) {
					alpha = this_value;
					best_index = i;
				}
			}
			else {
				// maxmini 2,1,3->second
				if (this_value < beta) {
					beta = this_value;
					best_index = i;
				}
			}
			if (alpha >= beta) {
				break;
			}
		}
		
		time_t e_time = time(NULL);
		cout << "======used: " << e_time-s_time << " seconds=======\n";
		return all_moves[best_index];
	}
	
	string get_name() {
		return ai_name;
	}
	
private:
	int alpha_beta(game_state state, int depth, int alpha, int beta) {
		if (alpha == beta) {
			return alpha;
		}
		
		vector<pair<int, int> > children = state.get_all_no_tipping_moves();
		
		if (depth == 0) {
			return (int) children.size();
		}
		
		if (depth % 2 == 0) {
			// get max, only alpha
			// minimax 2(2,3), 1(0), 3(1,4,1)->first
			for (int i = 0; i < children.size(); i ++) {
				int value = alpha_beta(state.move_fast(children[i]), depth-1, alpha, beta);
				if (value > alpha) {
					alpha = value;
				}
				if (alpha >= beta) {
					return alpha;
				}
				
			}
			return alpha;
		}
		else {
			// get min, only beta
			// maxmini 2, 1, 3->second
			for (int i = 0; i < children.size(); i ++) {
				int value = alpha_beta(state.move_fast(children[i]), depth-1, alpha, beta);
				if (value < beta) {
					beta = value;
				}
				if (beta <= alpha) {
					return beta;
				}
			}
			return beta;			
		}
	}
	
protected:
	int ai_depth;
	string ai_name;
};

