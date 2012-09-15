//
//  cost_calc.h
//  HeuristicProblemSolving
//  mint problem
//
//  Created by Haoquan Guo on 9/14/12.
//  Copyright 2012 NYU. All rights reserved.
//

#include <set>
#include <vector>
#include <map>
using namespace std;

double get_exact_cost(set<int> denominations, double N);

double get_exchange_cost(set<int> denominations, double N);

vector<map<int, int> > get_exact_coins(set<int> denominations, double N);

vector<map<int, int> > get_exchange_coins(set<int> denominations, double N);
