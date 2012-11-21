//
//  dating_person.cpp
//  HeuristicProblemSolving
//
//  Created by Haoquan Guo on 11/20/12.
//  Copyright 2012 NYU. All rights reserved.
//

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include "dating_person.h"

dating_person::dating_person(int _n_attr) : n_attr(_n_attr), n_turn(0) {
	srand((unsigned int) time(NULL));
}

vector<float> dating_person::generate_weights() {
	if (n_turn > 0) {
		vector<float> f_weights;
		for (int i = 0; i < n_attr; i ++) {
			f_weights.push_back(weights[i] * 0.01);
		}
		return f_weights;
	}
	int used_pos = 0;
	int used_neg = 0;
	int used_attr = 0;
	
	int n_small = min(7, n_attr-3);
	int n_small_pos = rand() % (n_small+1);
	int n_small_neg = n_small - n_small_pos;
	for (int i = 0; i < n_small_pos; i ++) {
		int small_pos_num = rand() % 3 + 1;
		weights.push_back(1 * small_pos_num);
		used_pos += small_pos_num;
		used_attr ++;
	}
	for (int i = 0; i < n_small_neg; i ++) {
		float small_neg_num = rand() % 3 + 1;
		weights.push_back(-1 * small_neg_num);
		used_neg += small_neg_num;
		used_attr ++;
	}
	
	int n_large = min(5, n_attr-used_attr);
	int n_large_pos = rand() % (n_large-2) + 2;
	int n_large_neg = n_large - n_large_pos;
	for (int i = 0; i < n_large_pos; i ++) {
		int large_pos_num = rand() % (100-n_attr/2 - used_pos - 5 * (n_large_pos-i)) + 5;
		large_pos_num = min(20, large_pos_num);
		weights.push_back(1 * large_pos_num);
		used_pos += large_pos_num;
		used_attr ++;
	}
	for (int i = 0; i < n_large_neg; i ++) {
		int large_neg_num = rand() % (100-n_attr/2 - used_neg - 5 * (n_large_neg-i)) + 5;
		large_neg_num = min(20, large_neg_num);
		weights.push_back(-1 * large_neg_num);
		used_neg += large_neg_num;
		used_attr ++;
	}
	
	int n_avg = n_attr - used_attr;
	int n_avg_pos = rand() % (n_avg-1) + 1;
	n_avg_pos = max(n_avg/2, n_avg_pos);
	int n_avg_neg = n_avg - n_avg_pos;
	
	int div_pos = (100-used_pos) / n_avg_pos;
	int mod_pos = (100-used_pos) % n_avg_pos;
	for (int i = 0; i < mod_pos; i ++) {
		weights.push_back(1 * (div_pos+1));
	}
	for (int i = mod_pos; i < n_avg_pos; i ++) {
		weights.push_back(1 * div_pos);
	}
	
	int div_neg = (100-used_neg) / n_avg_neg;
	int mod_neg = (100-used_neg) % n_avg_neg;
	for (int i = 0; i < mod_neg; i ++) {
		weights.push_back(-1 * (div_neg+1));
	}
	for (int i = mod_neg; i < n_avg_neg; i ++) {
		weights.push_back(-1 * div_neg);
	}
		
	// shuffle
	vector<int> shuffle_order;
	for (int i = 0; i < n_attr; i ++) {
		shuffle_order.push_back(rand() % 746);
	}
	for (int i = 0; i < n_attr-1; i ++) {
		for (int j = i+1; j < n_attr; j ++) {
			if (shuffle_order[i] > shuffle_order[j]) {
				shuffle_order[i] += shuffle_order[j];
				shuffle_order[j] = shuffle_order[i] - shuffle_order[j];
				shuffle_order[i] -= shuffle_order[j];
				float temp = weights[i];
				weights[i] = weights[j];
				weights[j] = temp;
			}
		}
	}
	
	last_noise.resize(n_attr, 0);
	
	n_turn = 1;
	
	vector<float> f_weights;
	for (int i = 0; i < n_attr; i ++) {
		f_weights.push_back(weights[i] * 0.01);
	}
	return f_weights;
}

vector<float> dating_person::generate_noise() {
	int n_noise = n_attr / 20;
	vector<int> current_noise;
	current_noise.resize(n_attr, 0);
	int ptr = n_turn;
	while (n_noise > 0) {
		if (abs(weights[ptr]) >= 5) {
			if (current_noise[ptr] != 0) {
				break;
			}
			else {
				if (last_noise[ptr] != 0) {
					current_noise[ptr] = -last_noise[ptr];
					last_noise[ptr] = current_noise[ptr];
					n_noise --;
				}
				else {
					int max_noise = weights[ptr] / 5;
					int update_noise = rand() % max_noise + 1;
					if (rand() % 2 == 0) {
						update_noise *= -1;
					}
					if (weights[ptr] + update_noise > 100) {
						update_noise = 100 - weights[ptr];
					}
					if (weights[ptr] + update_noise < -100) {
						update_noise = -100 - weights[ptr];
					}
					current_noise[ptr] = update_noise;
					last_noise[ptr] = current_noise[ptr];
					n_noise --;
				}
			}
		}
		ptr = (ptr + 1) % n_attr;
	}
	
	n_turn ++;
	vector<float> noise;
	for (int i = 0; i < n_attr; i ++) {
		//noise.push_back(0.00 * current_noise[i]);
		noise.push_back(0.01 * current_noise[i]);
	}
	return noise;
}
