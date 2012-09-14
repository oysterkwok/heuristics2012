//
//  cost_calc.cpp
//  HeuristicProblemSolving
//  mint problem
//
//  Created by Haoquan Guo on 9/14/12.
//  Copyright 2012 NYU. All rights reserved.
//

#include <iostream>
#include "cost_calc.h"

using namespace std;

int price_max = 100;
bool debug = true;

double get_exact_cost(set<int> denominations, double N) {
    
    int * cost_each_price = new int[price_max];
    
    for (int i = 0; i < price_max; i ++) {
        cost_each_price[i] = -1;
    }
    
    set<int> modified_ids;
    int n_coins = 0;
    cost_each_price[0] = n_coins;
    modified_ids.insert(0);
    
    while (modified_ids.size() > 0) {
        n_coins ++;
        set<int> next_modified_ids;
        set<int>::iterator si, sj;
        for (si = modified_ids.begin(); si != modified_ids.end(); si ++) {
            for (sj = denominations.begin(); sj != denominations.end(); sj ++) {
                int dst = *si + *sj;
                if (dst < price_max && cost_each_price[dst] < 0) {
                    cost_each_price[dst] = n_coins;
                    next_modified_ids.insert(dst);
                }
            }
        }
        modified_ids = next_modified_ids;
    }
    
    double score = 0;
    for (int i = 0; i < price_max; i ++) {
        if (debug) {
            cout << i << ": " << cost_each_price[i] << '\n'; 
        }
        if (cost_each_price[i] < 0) {
            return -1;
        }
        if (i % 5 == 0) {
            score += N * cost_each_price[i];
        }
        else {
            score += cost_each_price[i];
        }
    }
    return score;
}

double get_exchange_cost(set<int> denominations, double N) {
    return -1;
}

