//
//  cost_calc.h
//  HeuristicProblemSolving
//  mint problem
//
//  Created by Haoquan Guo on 9/14/12.
//  Copyright 2012 NYU. All rights reserved.
//

#include <vector>
#include <map>
#include <set>

using namespace std;

double get_exact_cost(int * denominations, double N, double local_minimum);

float get_exchange_cost(int * denominations, float optScore, float N);
double get_exchange_cost_o(set<int> denominations, double N, double optScore);

vector<map<int, int> > get_exact_coins(int * denominations);

vector<map<int, int> > get_exchange_coins(int * denominations);
