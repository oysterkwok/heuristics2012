//
//  game_play.cpp
//  HeuristicProblemSolving
//
//  Created by Haoquan Guo on 10/4/12.
//  Copyright 2012 NYU. All rights reserved.
//

#include <iostream>
#include "random_ai.cpp"
#include "human_ai.cpp"
#include "alpha_beta_ai.cpp"

void game_state_example();
void play_hr(human_ai &first_player, random_ai &second_player);
void play_rr(random_ai &first_player, random_ai &second_player);
void play_sr(alpha_beta_ai &first_player, random_ai &second_player);

bool is_print_info = true;
map<string, int> cache;

void game_state_example() {
	// this is the test/example
	
	// get the initial state
	game_state current_state = initial_game_state();
	
	// graphic output
	string gout = current_state.graphic_output();
	cout << gout;
	
	// state vector
	int * state_vector = current_state.state_vector;
	for (int i = 0; i < 25; i ++) {
		cout << state_vector[i] << " ";
	}
	cout << "\n";
	
	// move to another state
	game_state next_state = current_state.move_add(3, 5);
	gout = next_state.graphic_output();
	cout << gout;
	
	// left and right support state
	cout << "l:" << next_state.left_support_state << " r:" << next_state.right_support_state << "\n";
	
	// lose state
	cout << "is lost: " << next_state.is_lose_state << "\n";
	
	// game turn
	cout << "turn: " << next_state.game_turn << "\n";
	
	// is first player's move
	cout << "first player: " << next_state.is_first_player_move() << "\n";
	
	// get empty slots
	vector<int> empty_slots = next_state.get_empty_slots();
	for (int i = 0; i < empty_slots.size(); i ++) {
		cout << empty_slots[i] << " ";
	}
	cout << " | empty_slots\n";
	
	// get available blocks
	vector<int> avail_blocks = next_state.get_available_blocks();
	for (int i = 0; i < avail_blocks.size(); i ++) {
		cout << avail_blocks[i] << " ";
	}
	cout << " | avail_blocks\n";	
}

void play_hr(human_ai &first_player, random_ai &second_player) {
	game_state state = initial_game_state();
	game_state next_state = initial_game_state();
	bool is_game_over = false;
	pair<int, int> move;
	cout << state.graphic_output();
	while (!is_game_over) {
		move = first_player.ai_move(state);
		if (move.first == -1 && move.second == 16) {
			cout << "first player (" << first_player.get_name() << ") surrender.\n";
			is_game_over = true;
			cout << "second player (" << second_player.get_name() << ") wins!\n";
			break;
		}
		next_state = state.move_any(move);		
		if (next_state.game_turn == state.game_turn) {
			cout << "first player (" << first_player.get_name() << ") made a bad move.\n";
			is_game_over = true;
			cout << "second player (" << second_player.get_name() << ") wins!\n";
			break;
		}
		cout << "first player (" << first_player.get_name() << ") move: <" << move.first << ", " << move.second << ">\n";
		state = next_state;
		cout << state.graphic_output();
		if (state.is_tip()) {
			cout << "first player (" << first_player.get_name() << ") made a tip.\n";
			is_game_over = true;
			cout << "second player (" << second_player.get_name() << ") wins!\n";
			break;			
		}
		
		move = second_player.ai_move(state);
		if (move.first == -1 && move.second == 16) {
			cout << "second player (" << second_player.get_name() << ") surrender.\n";
			is_game_over = true;
			cout << "first player (" << first_player.get_name() << ") wins!\n";
			break;
		}
		next_state = state.move_any(move);
		if (next_state.game_turn == state.game_turn) {
			cout << "second player (" << second_player.get_name() << ") made a bad move.\n";
			is_game_over = true;
			cout << "first player (" << first_player.get_name() << ") wins!\n";
			break;
		}
		cout << "second player (" << second_player.get_name() << ") move: <" << move.first << ", " << move.second << ">\n";
		state = next_state;
		cout << state.graphic_output();
		if (state.is_tip()) {
			cout << "second player (" << second_player.get_name() << ") made a tip.\n";
			is_game_over = true;
			cout << "first player (" << first_player.get_name() << ") wins!\n";
			break;			
		}
	}
	cout << "game is over.\n";
}

void play_rr(random_ai &first_player, random_ai &second_player) {
	game_state state = initial_game_state();
	game_state next_state = initial_game_state();
	bool is_game_over = false;
	pair<int, int> move;
	while (!is_game_over) {
		move = first_player.ai_move(state);
		if (move.first == -1 && move.second == 16) {
			if (is_print_info)
			cout << "first player (" << first_player.get_name() << ") surrender.\n";
			is_game_over = true;
			if (is_print_info)
			cout << "second player (" << second_player.get_name() << ") wins!\n";
			break;
		}
		next_state = state.move_any(move);
		if (next_state.game_turn == state.game_turn) {
			if (is_print_info)
			cout << "first player (" << first_player.get_name() << ") made a bad move.\n";
			is_game_over = true;
			if (is_print_info)
			cout << "second player (" << second_player.get_name() << ") wins!\n";
			break;
		}
		if (is_print_info)
		cout << "first player (" << first_player.get_name() << ") move: <" << move.first << ", " << move.second << ">\n";
		state = next_state;
		if (cache.find(state.get_hash_code()) == cache.end()) {
			cache[state.get_hash_code()] = 1;
		}
		else {
			cache[state.get_hash_code()] ++;
		}
		if (state.is_tip()) {
			if (is_print_info)
			cout << "first player (" << first_player.get_name() << ") made a tip.\n";
			is_game_over = true;
			if (is_print_info)
			cout << "second player (" << second_player.get_name() << ") wins!\n";
			break;
		}
		
		move = second_player.ai_move(state);
		if (move.first == -1 && move.second == 16) {
			if (is_print_info)
			cout << "second player (" << second_player.get_name() << ") surrender.\n";
			is_game_over = true;
			if (is_print_info)
			cout << "first player (" << first_player.get_name() << ") wins!\n";
			break;
		}
		next_state = state.move_any(move);
		if (next_state.game_turn == state.game_turn) {
			if (is_print_info)
			cout << "second player (" << second_player.get_name() << ") made a bad move.\n";
			is_game_over = true;
			if (is_print_info)
			cout << "first player (" << first_player.get_name() << ") wins!\n";
			break;
		}
		if (is_print_info)
		cout << "second player (" << second_player.get_name() << ") move: <" << move.first << ", " << move.second << ">\n";
		state = next_state;
		if (cache.find(state.get_hash_code()) == cache.end()) {
			cache[state.get_hash_code()] = 1;
		}
		else {
			cache[state.get_hash_code()] ++;
		}
		if (state.is_tip()) {
			if (is_print_info)
			cout << "second player (" << second_player.get_name() << ") made a tip.\n";
			is_game_over = true;
			if (is_print_info)
			cout << "first player (" << first_player.get_name() << ") wins!\n";
			break;	
		}
	}
	if (is_print_info)
	cout << "game is over.\n";
}

void play_sr(alpha_beta_ai &first_player, random_ai &second_player) {
	game_state state = initial_game_state();
	game_state next_state = initial_game_state();
	bool is_game_over = false;
	pair<int, int> move;
	while (!is_game_over) {
		cout << state.graphic_output() << '\n';
		move = first_player.ai_move(state);
		cout << move.first << ":" << move.second << endl;
		if (move.first == -1 && move.second == 16) {
			if (is_print_info)
				cout << "first player (" << first_player.get_name() << ") surrender.\n";
			is_game_over = true;
			if (is_print_info)
				cout << "second player (" << second_player.get_name() << ") wins!\n";
			break;
		}
		next_state = state.move_any(move);
		if (next_state.game_turn == state.game_turn) {
			if (is_print_info)
				cout << "first player (" << first_player.get_name() << ") made a bad move.\n";
			is_game_over = true;
			if (is_print_info)
				cout << "second player (" << second_player.get_name() << ") wins!\n";
			break;
		}
		if (is_print_info)
			cout << "first player (" << first_player.get_name() << ") move: <" << move.first << ", " << move.second << ">\n";
		state = next_state;
		if (cache.find(state.get_hash_code()) == cache.end()) {
			cache[state.get_hash_code()] = 1;
		}
		else {
			cache[state.get_hash_code()] ++;
		}
		if (state.is_tip()) {
			if (is_print_info)
				cout << "first player (" << first_player.get_name() << ") made a tip.\n";
			is_game_over = true;
			if (is_print_info)
				cout << "second player (" << second_player.get_name() << ") wins!\n";
			break;
		}
		
		move = second_player.ai_move(state);
		if (move.first == -1 && move.second == 16) {
			if (is_print_info)
				cout << "second player (" << second_player.get_name() << ") surrender.\n";
			is_game_over = true;
			if (is_print_info)
				cout << "first player (" << first_player.get_name() << ") wins!\n";
			break;
		}
		next_state = state.move_any(move);
		if (next_state.game_turn == state.game_turn) {
			if (is_print_info)
				cout << "second player (" << second_player.get_name() << ") made a bad move.\n";
			is_game_over = true;
			if (is_print_info)
				cout << "first player (" << first_player.get_name() << ") wins!\n";
			break;
		}
		if (is_print_info)
			cout << "second player (" << second_player.get_name() << ") move: <" << move.first << ", " << move.second << ">\n";
		state = next_state;
		if (cache.find(state.get_hash_code()) == cache.end()) {
			cache[state.get_hash_code()] = 1;
		}
		else {
			cache[state.get_hash_code()] ++;
		}
		if (state.is_tip()) {
			if (is_print_info)
				cout << "second player (" << second_player.get_name() << ") made a tip.\n";
			is_game_over = true;
			if (is_print_info)
				cout << "first player (" << first_player.get_name() << ") wins!\n";
			break;	
		}
	}
	if (is_print_info)
		cout << "game is over.\n";
}

int main() {
	bool debug = true;
	human_ai hai = human_ai("human");
	random_ai rai = random_ai("random_ai");
	alpha_beta_ai sai = alpha_beta_ai("super_ai");
	
	if (debug) {
		play_sr(sai, rai);
		return 0;
	}
	
	srand((int) time(NULL));
	time_t time_s = time(NULL);
	int n_game = 0;
	while (true) {
		play_rr(rai, rai);
		n_game ++;
		if (n_game % 100 == 0) {
			cout << "+";
			cout.flush();
		}
		if (n_game % 10000 == 0) {
			cout << " | " << cache.size();
			int sum = 0;
			for (map<string, int>::iterator iter = cache.begin(); iter != cache.end(); iter ++) {
				sum += iter->second;
			}
			time_t time_e = time(NULL);
			cout << "/" << sum << " : " << 100 - 100.0*cache.size()/sum << "% | " << time_e - time_s << "seconds \n";
			cout.flush();
		}
	}
	return 0;
}