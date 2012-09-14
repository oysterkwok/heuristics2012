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
    double cost = get_exact_cost(dom, N);
    cout << cost << '\n';
    return 0;
}

