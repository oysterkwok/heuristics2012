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

int price_max = 100;                            // maximum price, by default is 1-99, so should set to 100 here
int unreachable_cost = price_max * price_max;   // a really big score for some price cannot be reach
bool debug = true;                              // if true, would print the detail result for each price

double get_exchange_cost_o(set<int> denominations, double N, double local_minimum) {
    
    double score = 0;               // sum of the score, the returning value
    
    bool * reachable = new bool[price_max]; // reachability for each price on the current denomination, e.g. cost_each_price[43] = true means 43 cents can be reach at the current iteration
    for (int i = 1; i < price_max; i ++)
        reachable[i] = false;               // initially setting all to false (unreachable)
    reachable[0] = true;
    
    int n_coins = 0;                // number of coins for each iteration, start from 0
    set<int> modified_ids;          // newly added reachable prices, initially on price 0 and price_max
    modified_ids.insert(0);
    
    while (modified_ids.size() > 0) {   // if there is no newly reachable prices, iteration end
        n_coins ++;                 // for each iteration, increse the coin number by 1
        set<int> next_modified_ids; // newly added reachable prices, and this is the update to the modified_ids
        set<int>::iterator si, sj;  // set iterator
        
        for (si = modified_ids.begin(); si != modified_ids.end(); si ++) {          // for each reached prices in last iteration
            for (sj = denominations.begin(); sj != denominations.end(); sj ++) {    // for each denomination
                int dst;                                    // the destination (newly reachable price)
                
                dst = *si + *sj;                            // when equals to last price plus one of the denomination
                while (dst >= price_max)                    // reduce to the range window
                    dst -= price_max;
                
                if (!reachable[dst]) {                      // if is not reached before
                    reachable[dst] = true;                      // turn reachable for this price to true
                    next_modified_ids.insert(dst);              // add to the newly reachable price set
                    if (dst % 5 == 0)
                        score += N * n_coins;                   // add score by N * n_coins if the destination price is multiples of 5
                    else
                        score += n_coins;                       // otherwise, add score by n_coins
                    
                    if (local_minimum > 0 && score > local_minimum) // purning, if local_minimum is defined and smaller than the current score, return failure
                        return -1;
                }
                
                dst = *si - *sj;                            // when equals to last price minus one of the denomination
                while (dst < 0)                             // increase to the range window
                    dst += price_max;
                
                if (!reachable[dst]) {                      // if is not reached before
                    reachable[dst] = true;                      // turn reachable for this price to true
                    next_modified_ids.insert(dst);              // add to the newly reachable price set
                    if (dst % 5 == 0)
                        score += N * n_coins;                   // add score by N * n_coins if the destination price is multiples of 5
                    else
                        score += n_coins;                       // otherwise, add score by n_coins
                    
                    if (local_minimum > 0 && score > local_minimum) // purning, if local_minimum is defined and smaller than the current score, return failure
                        return -1;
                }
                
            }
        }
        modified_ids = next_modified_ids;   // update modified_ids with newly reachable price set
    }
    
    for (int i = 0; i < price_max; i ++) {  // return failure if any price is not reachable
        if (!reachable[i]) {
            return -1;
        }
    }
    
    return score;
}

float get_exchange_cost(int * denominations, float optScore, float N) {
    
    float score = 0;               // sum of the score, the returning value
    
    int reachable[100]; // reachability for each price on the current denomination, e.g. cost_each_price[43] = true means 43 cents can be reach at the current iteration
    int visited[10];
    int v_tail = 0;
    memset(reachable, -1, sizeof(reachable));    // initially setting all to false (unreachable)        
    reachable[0] = 0;
    int assigned = 1;

    // iter 1
    int n_coins = 1;                // number of coins for each iteration, start from 0    
    for (int i = 0; i < 5; i ++) {
        if (denominations[i] > 0) {
            int dst = denominations[i];
            if (reachable[dst] < 0) {
                reachable[dst] = n_coins;
                visited[v_tail] = dst;
                v_tail ++;
                assigned ++;
                if (dst % 5 == 0)
                    score += N * n_coins;
                else
                    score += n_coins;
                //cout << "[+]\t" << dst << "\t" << score << '\n';
            }
            dst = 100 - denominations[i];
            if (reachable[dst] < 0) {
                reachable[dst] = n_coins;
                visited[v_tail] = dst;
                v_tail ++;
                assigned ++;
                if (dst % 5 == 0)
                    score += N * n_coins;
                else
                    score += n_coins;
               // cout << "[+]\t" << dst << "\t" << score << '\n';
            }
        }
    }
    
    // iter 2
    n_coins ++;
    for (int i = 0; i < v_tail; i ++) {
        for (int j = i; j < v_tail; j ++) {
            int dst = (visited[i] + visited[j]) % 100;
            if (reachable[dst] < 0) {
                reachable[dst] = n_coins;
                assigned ++;
                if (dst % 5 == 0)
                    score += N * n_coins;
                else
                    score += n_coins;
                //cout << "[+]\t" << dst << "\t" << score << '\n';
            }
        }
    }
    
    bool has_update = true;
    while (has_update) {
        n_coins ++;
        has_update = false;
        for (int dst = 1; dst < 100; dst ++) {
            if (reachable[dst] < 0) {
                for (int i = 0; i < v_tail; i ++) {
                    int src = (dst + visited[i]) % 100;
                    if (reachable[src] == n_coins - 1) {
                        reachable[dst] = n_coins;
                        assigned ++;
                        if (dst % 5 == 0)
                            score += N * n_coins;
                        else
                            score += n_coins;
                        has_update = true;
                        //cout << "[+]\t" << dst << "\t" << score << '\n';
                        break;
                    }
                }
            }
        }
        if (optScore > 0 && score+(n_coins+1)*(100-assigned) >= optScore) // purning, if local_minimum is defined and smaller than the current score, return failure
            return -1;
    }
    
    for (int i = 1; i < 100; i ++) {  // return failure if any price is not reachable
        if (reachable[i] < 0) {
            return -1;
        }
    }
    return score;
}

vector<map<int, int> > get_exact_coins(int * denominations) {
    vector<map<int, int> > result;  // for each price, the combination of coins
//    result.resize(price_max);       // resize to price_max
//    set<int> modified_ids;          // reached price set
//    modified_ids.insert(0);         // initially only 0
//    
//    while (modified_ids.size() > 0) {
//        set<int> next_modified_ids;
//        set<int>::iterator si, sj;
//        
//        for (si = modified_ids.begin(); si != modified_ids.end(); si ++) {
//            for (sj = denominations.begin(); sj != denominations.end(); sj ++) {
//                int dst = *si + *sj;
//                if (dst < price_max && result[dst].size() == 0) {
//                    result[dst] = result[*si];
//                    map<int, int>::iterator mi;
//                    mi = result[dst].find(*sj);
//                    if (mi == result[dst].end())
//                        result[dst][*sj] = 1;
//                    else
//                        result[dst][*sj] ++;
//                    next_modified_ids.insert(dst);
//                }
//            }
//        }
//        modified_ids = next_modified_ids;
//    }
//    
//    if (debug) {
//        for (int i = 1; i < price_max; i ++) {
//            cout << i << ": ";
//            map<int, int>::iterator mi;
//            for (mi = result[i].begin(); mi != result[i].end(); mi ++) {
//                cout << '[' << mi->first << "]*" << mi->second << ' ';
//            }
//            cout << '\n';
//        }
//    }
    return result;
}

vector<map<int, int> > get_exchange_coins(int * denominations) {
    vector<map<int, int> > result;
    result.resize(price_max);
    set<int> modified_ids;
    modified_ids.insert(0);
    result[0][price_max] = 0;
    
    while (modified_ids.size() > 0) {
        set<int> next_modified_ids;
        set<int>::iterator si;
        map<int, int>::iterator mi;
        
        for (si = modified_ids.begin(); si != modified_ids.end(); si ++) {
            for (int i = 0; i < 5; i ++) {
                int dst, counter;
                
                int sj = denominations[i];
                dst = *si + sj;                        // plus case
                counter = 0;
                while (dst >= price_max) {              // while it is out of range, reduce it by price_max until within the range
                    dst -= price_max;
                    counter --;                    
                }
                
                if (result[dst].size() == 0) {
                    result[dst] = result[*si];
                    mi = result[dst].find(sj);
                    if (mi == result[dst].end())
                        result[dst][sj] = 1;
                    else
                        result[dst][sj] ++;
                    result[dst][price_max] += counter;
                    next_modified_ids.insert(dst);
                }
                
                dst = *si - sj;                        // minus case
                counter = 0;
                while (dst < 0) {              // while it is out of range, reduce it by price_max until within the range
                    dst += price_max;
                    counter ++;
                }
                
                if (result[dst].size() == 0) {
                    result[dst] = result[*si];
                    mi = result[dst].find(sj);
                    if (mi == result[dst].end())
                        result[dst][sj] = -1;
                    else
                        result[dst][sj] --;
                    result[dst][price_max] += counter;
                    next_modified_ids.insert(dst);
                }
            }
        }
        modified_ids = next_modified_ids;
    }
    
    if (debug) {
        for (int i = 1; i < price_max; i ++) {
            int count = 0;
            cout << i << ": ";
            map<int, int>::iterator mi;
            for (mi = result[i].begin(); mi != result[i].end(); mi ++) {
                if (mi->second != 0)
                    cout << '[' << mi->first << "]*" << mi->second << ' ';
                if (mi->first == price_max)
                    continue;
                count += abs(mi->second);
            }
            
            cout << '\t' << count << '\n';
        }
    }
    return result;
}

