//
//  game_state.cpp
//  HeuristicProblemSolving
//
//  Created by Haoquan Guo on 10/2/12.
//  Copyright 2012 NYU. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include "game_state.h"

game_state::game_state(const int _state_vector[25], const bool _is_adding) {

	// copy into the vector
	for (int i = 0; i < 25; i ++) {
		state_vector[i] = _state_vector[i];
	}
	is_adding = _is_adding;
	
	// compute support state
	// initial board weight
	left_support_state = 9;
	right_support_state = 3;
	// initial 3kg block
	if (state_vector[0] != 16) {
		left_support_state += (state_vector[0]-(-3)) * 3;
		right_support_state += (state_vector[0]-(-1)) * 3;
	}
	// other blocks
	for (int i = 1; i < 13; i ++) {
		if (state_vector[i] != 16) {
			left_support_state += (state_vector[i]-(-3)) * i;
			right_support_state += (state_vector[i]-(-1)) * i;
		}
	}
	for (int i = 13; i < 25; i ++) {
		if (state_vector[i] != 16) {
			left_support_state += (state_vector[i]-(-3)) * (i-12);
			right_support_state += (state_vector[i]-(-1)) * (i-12);
		}
	}
	
	int blocks_count = 0; // i.e. how many blocks on the board
	for (int i = 0; i < 25; i ++) {
		if (state_vector[i] != 16) {
			blocks_count ++;
		}
	}
	
	if (blocks_count == 25) {
		is_adding = false;
	}
	if (is_adding) {
		game_turn = blocks_count;
	}
	else {
		game_turn = 50 - blocks_count;
	}
	
	winning_move = pair<int, int>(-1, 16);
	if (left_support_state * right_support_state > 0) {
		is_lose_state = true;
	}
	else {
		is_lose_state = false;
	}
}

game_state::game_state(const game_state& state) {
	for (int i = 0; i < 25; i ++) {
		state_vector[i] = state.state_vector[i];
	}
	left_support_state = state.left_support_state;
	right_support_state = state.right_support_state;
	winning_move = state.winning_move;
	is_lose_state = state.is_lose_state;
	is_adding = state.is_adding;
	game_turn = state.game_turn;
}

bool game_state::is_first_player_move() {
	return game_turn % 2 == 1;
}

bool game_state::is_tip() {
	return left_support_state * right_support_state > 0;
}

vector<int> game_state::get_empty_slots() {
	bool empty_slot_map[31];
	for (int i = 0; i < 31; i ++) {
		empty_slot_map[i] = true;
	}
	for (int i = 0; i < 25; i ++) {
		if (state_vector[i] != 16) {
			empty_slot_map[state_vector[i]+15] = false;
		}
	}
	vector<int> empty_slots;
	for (int i = -15; i <= 15; i ++) {
		if (empty_slot_map[i+15]) {
			empty_slots.push_back(i);
		}
	}
	return empty_slots;
}

vector<int> game_state::get_available_blocks() {
	vector<int> available_blocks;
	if (!is_adding) {
		return available_blocks;
	}
	if (is_first_player_move()) {
		for (int i = 1; i < 13; i ++) {
			if (state_vector[i] == 16) {
				available_blocks.push_back(i);
			}
		}
	}
	else {
		for (int i = 13; i < 25; i ++) {
			if (state_vector[i] == 16) {
				available_blocks.push_back(i);
			}			
		}
		
	}
	return available_blocks;
}

vector<pair<int, int> > game_state::get_occupied_slots() {
	// fill the vector
	vector<pair<int, int> > occupied_slots;
	for (int i = 0; i < 25; i ++) {
		if (state_vector[i] != 16) {
			occupied_slots.push_back(pair<int, int> (i, state_vector[i]));
		}
	}
	// simple sort
	for (int i = 0; i < occupied_slots.size() - 1; i ++) {
		for (int j = i+1 ; j < occupied_slots.size(); j ++) {
			if (occupied_slots[i].second > occupied_slots[j].second) {
				pair<int, int> temp = occupied_slots[i];
				occupied_slots[i] = occupied_slots[j];
				occupied_slots[j] = temp;
			}
		}
	}
	return occupied_slots;
}

vector<pair<int, int> > game_state::get_all_no_tipping_moves() {
	vector<pair<int, int> > safe_moves;
	if (is_adding) {
		vector<int> blocks = get_available_blocks();
		vector<int> slots = get_empty_slots();
		for (int i = 0; i < blocks.size(); i ++) {
			int weight;
			if (blocks[i] == 0) {
				weight = 3;
			}
			else {
				weight = (blocks[i]-1) % 12 + 1;
			}
			int head = 0;
			int tail = (int) slots.size();
			while (head < tail) {
				int new_left = left_support_state + weight * (slots[head]-(-3));
				int new_right = right_support_state + weight * (slots[head]-(-1));
				if (new_left * new_right <= 0) {
					break;
				}
				head ++;
			}
			while (head < tail) {
				int new_left = left_support_state + weight * (slots[tail-1]-(-3));
				int new_right = right_support_state + weight * (slots[tail-1]-(-1));
				if (new_left * new_right <= 0) {
					break;
				}
				tail --;
			}
			for (int j = head; j < tail; j ++) {
				safe_moves.push_back(pair<int, int> (blocks[i], slots[j]));
			}
		}
	}
	else {
		vector<pair<int, int> > all_moves = get_occupied_slots();
		for (int i = 0; i < all_moves.size(); i ++) {
			int weight;
			if (all_moves[i].first == 0) {
				weight = 3;
			}
			else {
				weight = (all_moves[i].first-1) % 12 + 1;
			}
			int new_left = left_support_state - weight * (all_moves[i].second-(-3));
			int new_right = right_support_state - weight * (all_moves[i].second-(-1));
			if (new_left * new_right <= 0) {
				safe_moves.push_back(all_moves[i]);
			}
		}
	}
	return safe_moves;
}

int * game_state::get_board_state() {
	int * board_state = new int[31];
	for (int i = 0; i < 31; i ++) {
		board_state[i] = -1;
	}
	for (int i = 0; i < 25; i ++) {
		if (state_vector[i] != 16) {
			board_state[state_vector[i]+15] = i;
		}
	}
	return board_state;
}

string game_state::get_hash_code() {
	string hash;
	if (is_adding) {
		hash.push_back('A');
	}
	else {
		hash.push_back('R');
	}
	for (int i = 0; i < 25; i ++) {
		if (state_vector[i] == 0) {
			hash.push_back('0');
			continue;
		}
		if (state_vector[i] == 16) {
			hash.push_back('-');
			continue;
		}
		if (state_vector[i] > 0) {
			hash.push_back(char(65+state_vector[i]));
			continue;
		}
		if (state_vector[i] < 0) {
			hash.push_back(char(97-state_vector[i]));
			continue;
		}		
	}
	return hash;
}

game_state game_state::move_fast(pair<int, int> move) {
	if (is_adding) {
		int new_state_vector[25];
		for (int i = 0; i < 25; i ++) {
			new_state_vector[i] = state_vector[i];
		}
		new_state_vector[move.first] = move.second;
		return game_state(new_state_vector, true);
	}
	else {
		int new_state_vector[25];
		for (int i = 0; i < 25; i ++) {
			new_state_vector[i] = state_vector[i];
		}
		new_state_vector[move.first] = 16;
		return game_state(new_state_vector, false);
	}
}

game_state game_state::move_add(int block_id, int slot_pos) {
	return this->move_add(pair<int, int>(block_id, slot_pos));
}

game_state game_state::move_add(pair<int, int> move) {
	// check if is add mode
	if (!is_adding) {
		cout << "[X]not adding\n";
		return *this;
	}
	
	// check if the block is available to move
	bool checked = false;
	vector<int> avail_blocks = get_available_blocks();
	for (int i = 0; i < avail_blocks.size(); i ++) {
		if (avail_blocks[i] == move.first) {
			checked = true;
			break;
		}
	}
	if (!checked) {
		cout << "[X]not no such block\n";
		return *this;
	}
	
	// check if the slot is empty
	checked = false;
	vector<int> empty_slots = get_empty_slots();
	for (int i = 0; i < empty_slots.size(); i ++) {
		if (empty_slots[i] == move.second) {
			checked = true;
			break;
		}
	}
	if (!checked) {
		cout << "[X]not no such slot\n";
		return *this;
	}
	
	int new_state_vector[25];
	for (int i = 0; i < 25; i ++) {
		new_state_vector[i] = state_vector[i];
	}
	new_state_vector[move.first] = move.second;
	return game_state(new_state_vector, true);
}

game_state game_state::move_remove(int slot_pos) {
	// check if is remove mode
	if (is_adding) {
		return *this;
	}
	
	// check if the slot is occupied
	int block_id = -1;
	for (int i = 0; i < 25; i ++) {
		if (state_vector[i] == slot_pos) {
			block_id = i;
			break;
		}
	}
	if (block_id == -1) {
		return *this;
	}
	
	int new_state_vector[25];
	for (int i = 0; i < 25; i ++) {
		new_state_vector[i] = state_vector[i];
	}
	new_state_vector[block_id] = 16;
	
	return game_state(new_state_vector, false);
}

game_state game_state::move_remove(pair<int, int> move) {
	// check if is remove mode
	if (is_adding) {
		return *this;
	}
	
	// check if the slot is occupied
	if (state_vector[move.first] != move.second) {
		return *this;
	}
	
	int new_state_vector[25];
	for (int i = 0; i < 25; i ++) {
		new_state_vector[i] = state_vector[i];
	}
	new_state_vector[move.first] = 16;
	
	return game_state(new_state_vector, false);
}

game_state game_state::move_any(pair<int, int> move) {
	if (is_adding) {
		return move_add(move);
	}
	else {
		return move_remove(move);
	}
}

string game_state::graphic_output() {
	int * board_state = get_board_state();
	string gout;
	
	// print blocks
	for (int i = 0; i < 31; i ++) {
		if (board_state[i] == -1) {
			gout.append("====");
			continue;
		}
		if (board_state[i] == 0) {
			gout.append("=<3>");
			continue;
		}
		if (board_state[i] >= 1 && board_state[i] <= 9) {
			char * block = new char[4];
			sprintf(block, "=[%d]", board_state[i]);
			gout.append(block);
			continue;
		}
		if (board_state[i] >= 10 && board_state[i] <= 12) {
			char * block = new char[4];
			sprintf(block, "[%d]", board_state[i]);
			gout.append(block);
			continue;
		}
		if (board_state[i] >= 13 && board_state[i] <= 21) {
			char * block = new char[4];
			sprintf(block, "=(%d)", board_state[i]-12);
			gout.append(block);
			continue;
		}
		if (board_state[i] >= 22 && board_state[i] <= 24) {
			char * block = new char[4];
			sprintf(block, "(%d)", board_state[i]-12);
			gout.append(block);
			continue;
		}
	}
	gout.append("\n");
	gout.append("-15 -14 -13 -12 -11 -10  -9  -8  -7  -6  -5  -4  /\\  -2  /\\   0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15\n");
	char * support_state = new char[30];
	sprintf(support_state, "left_support : %d\n", left_support_state);
	gout.append(support_state);
	sprintf(support_state, "right_support: %d\n", right_support_state);
	gout.append(support_state);
	gout.append("\n");
	return gout;
}

game_state initial_game_state() {
	int new_state_vector[25];
	new_state_vector[0] = -4;
	for (int i = 1; i < 25; i ++) {
		new_state_vector[i] = 16;
	}
	return game_state(new_state_vector, true);
}
