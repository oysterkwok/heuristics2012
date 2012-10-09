//
//  game_state.h
//  HeuristicProblemSolving
//
//  Created by Haoquan Guo on 10/2/12.
//  Copyright 2012 NYU. All rights reserved.
//

#include <map>
#include <vector>

#ifndef HeuristicProblemSolving_game_state_h
#define HeuristicProblemSolving_game_state_h

using namespace std;

class game_state {
public:
	int state_vector[25]; // each element is the position of one block, range from -15 to 15, and 16 means unused/removed block; index 0 is the initial 3kg block, then 1-12 is the 1kg to 12kg blocks of the first player (who takes first move), and 13-24 is for the second player
	int left_support_state, right_support_state; // stores the left and right support state, and left one is on position -3, right one on position -1, would automately calculate when calling the constructor
	
	pair<int, int> winning_move; // if this is a winning state, stores the destiny move; or <-1, 16> if still uncertain; in the pair first is block id, second is position
	
	bool is_lose_state; // true if this state is a definatly lose, otherwise false if still uncertain
	
	bool is_adding; // true if this is add mode, otherwise false if this is remove mode
	int game_turn; // number of turns of this state, i.e. how many moves did for both players +1 (so starting from 1)
	
	game_state(const int _state_vector[25], const bool _is_adding); // default constructor
	game_state(const game_state& state); // copy constructor
	
	// a few useful get methods
	bool is_first_player_move(); // check whose turn is
	bool is_tip();
	vector<int> get_empty_slots(); // get empty slots on board, sorted from -15 to 15
	vector<int> get_available_blocks(); // get available blocks to add, sorted from 1 to 24
	vector<pair<int, int> > get_occupied_slots(); // get occupied slots, sorted by slot position, from -15 to 15, first of the pair is block id, second is slot position
	vector<pair<int, int> > get_all_no_tipping_moves();
	int * get_board_state();
	string get_hash_code();
	
	// moves, return original state if the move is illeagel
	// add moves, same effect with both
	game_state move_fast(pair<int, int> move);
	game_state move_add(int block_id, int slot_pos);
	game_state move_add(pair<int, int> move);
	// remove moves, same effect with both	
	game_state move_remove(int slot_pos);
	game_state move_remove(pair<int, int> move);
	game_state move_any(pair<int, int> move);
	
	// just for fun!
	string graphic_output();
};

game_state initial_game_state();


#endif
