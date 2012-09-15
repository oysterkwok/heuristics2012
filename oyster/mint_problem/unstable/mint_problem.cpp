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
    bool test_exact = false;
    bool test_exchange = true;
    
    if (test_exact) {
        set<int> dom;
        dom.insert(1);
        dom.insert(5);
        dom.insert(10);
        double N = 3.5;
        
        double cost = get_exact_cost(dom, N, -1);
        cout << "first: " << cost << '\n';
        
        vector<map<int, int> > coins = get_exact_coins(dom);
        
        dom.insert(25);
        
        double cost2 = get_exact_cost(dom, N, 100);
        cout << "second: " << cost2 << '\n';
        
        coins = get_exact_coins(dom);
        
        dom.insert(50);
        
        double cost3 = get_exact_cost(dom, N, cost);
        cout << "third: " << cost3 << '\n';
        
        coins = get_exact_coins(dom);
    }
    
    if (test_exchange) {
        set<int> dom;
        dom.insert(1);
        dom.insert(5);
        dom.insert(10);
        double N = 3.5;
        
        double cost = get_exchange_cost(dom, N, -1);
        cout << "first: " << cost << '\n';
        
        vector<map<int, int> > coins = get_exchange_coins(dom);
        
        dom.insert(25);
        
        double cost2 = get_exchange_cost(dom, N, 100);
        cout << "second: " << cost2 << '\n';
        
        coins = get_exchange_coins(dom);
        
        dom.insert(50);
        
        double cost3 = get_exchange_cost(dom, N, cost);
        cout << "third: " << cost3 << '\n';
        
        coins = get_exchange_coins(dom);
    }
    return 0;
}

