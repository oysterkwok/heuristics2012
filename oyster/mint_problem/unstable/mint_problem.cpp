//
//  mint_problem.cpp
//  HeuristicProblemSolving
//  mint problem
//
//  Created by Haoquan Guo on 9/14/12.
//  Copyright 2012 NYU. All rights reserved.
//

#include <iostream>
#include "cost_calc.h"

using namespace std;

int main (int argc, const char * argv[])
{
    set<int> dom;
    dom.insert(1);
    dom.insert(5);
    dom.insert(25);
    dom.insert(10);
    dom.insert(50);
    double N = 3.5;
//    double exact_cost = get_exact_cost(dom, N);
  //  double exchange_cost = get_exchange_cost(dom, N);
//    get_exact_coins(dom, N);
    get_exchange_coins(dom, N);
//    cout << exact_cost << '\n';
//    cout << exchange_cost << '\n';
    return 0;
}

