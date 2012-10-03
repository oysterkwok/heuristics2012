//
//  game_state.cpp
//  HeuristicProblemSolving
//
//  Created by Haoquan Guo on 10/2/12.
//  Copyright 2012 NYU. All rights reserved.
//

#include <iostream>
#include "game_state.h"

game_state::game_state(int _state_vector[25], bool _is_adding) {

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
	if (left_support_state * right_support_state < 0) {
		is_lose_state = true;
	}
	else {
		is_lose_state = false;
	}
}

bool game_state::is_first_player_move() {
	return game_turn % 2 == 1;
}

vector<int> game_state::get_empty_slots() {
	bool empty_slot_map[31];
	memset(empty_slot_map, true, sizeof(empty_slot_map));
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
			if (state_vector[i] != 16) {
				available_blocks.push_back(i);
			}
		}
	}
	else {
		for (int i = 13; i < 25; i ++) {
			if (state_vector[i] != 16) {
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

game_state game_state::move_add(int block_id, int slot_pos) {
	return move_add(pair<int, int>(block_id, slot_pos));
}

game_state game_state::move_add(pair<int, int> move) {
	// check if is add mode
	if (!is_adding) {
		return NULL;
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
		return NULL;
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
		return NULL;
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
		return NULL;
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
		return NULL;
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
		return NULL;
	}
	
	// check if the slot is occupied
	if (state_vector[move.first] != move.second) {
		return NULL;
	}
	
	int new_state_vector[25];
	for (int i = 0; i < 25; i ++) {
		new_state_vector[i] = state_vector[i];
	}
	new_state_vector[move.first] = 16;
	
	return game_state(new_state_vector, false);
}

string game_state::graphic_output() {
	
}

int main() {
	return 0;
}