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
float N = 26.37;
int t = 2;

void run_normal_exchange();

void run_normal_exchange() {
    
    float min_score = 100*100*N;
    
    clock_t c_start = clock();
    

    int best[5];
    set<int> best2;
    for (int d1 = 1; d1 < 51; d1 ++)
        for (int d2 = d1+1; d2 < 51; d2 ++)
            for (int d3 = d2+1; d3 < 51; d3 ++)
                for (int d4 = d3+1; d4 < 51; d4 ++)
                    for (int d5 = d4+1; d5 < 51; d5 ++) {
                        if (t==1) {
                        set<int> dom2;
                        dom2.insert(d1);
                        dom2.insert(d2);
                        dom2.insert(d3);
                        dom2.insert(d4);
                        dom2.insert(d5);
                        double score = get_exchange_cost_o(dom2, N, min_score);
                        
                        if (score > 0 && score < min_score) {
                            cout << score << ',' << min_score << ',';
                            min_score = score;
                            best2 = dom2;
                            for (set<int>::iterator si = dom2.begin(); si != dom2.end(); si ++) {
                                cout << *si << ' ';
                            }
                            cout << '\n';
                        }
                        }
                        else{
                        int dom[5];
                        dom[0] = d1;
                        dom[1] = d2;
                        dom[2] = d3;
                        dom[3] = d4;
                        dom[4] = d5;
                        float score = get_exchange_cost(dom, min_score, N);
                        if (score > 0 && score < min_score) {
                            cout << score << ',' << min_score << ',';
                            min_score = score;
                            for (int i = 0; i < 5; i ++) {
                                best[i] = dom[i];
                                cout << best[i] << ' ';
                            }
                            cout << '\n';
                        }
                        }
                    }
    cout << "min_score: " << min_score << '\n';
    get_exact_coins(best);
    clock_t c_end = clock();
    cout << "time_elpased: " << (c_end - c_start) * 1000.0 / CLOCKS_PER_SEC << " score: \n";    
}

void test();
void test() {
    for (int i = 1; i < 100; i ++) {
        bool ok = true;
        bool all[100];
        memset(all, false, sizeof(all));
        all[i] = true;
        int target = i*2 % 100;
        while (!all[target]) {
            all[target] = true;
            target += i;
            target %= 100;
        }
        for (int j = 1; j < 100; j ++) {
            if (!all[j]) {
                ok = false;
                break;
            }
        }
        if (ok) {
            cout << i << ',';
        }
    }
    cout << '\n';
}

int main (int argc, const char * argv[])
{
    run_normal_exchange();
    int den[5];
    den[0] = 1;
    den[1] = 5;
    den[2] = 8;
    den[3] = 25;
    den[4] = 40;
    vector<map<int, int> > res = get_exchange_coins(den);
    double score = 0;
    for (int i = 1; i < res.size(); i ++) {
        map<int, int>::iterator mi;
        for (mi = res[i].begin(); mi != res[i].end(); mi ++) {
            if (mi->first < 100) {
                if (i % 5 == 0) {
                    score += abs(mi->second) * N;
                }
                else
                    score += abs(mi->second);
            }
        }
    }
    cout << score << '\n';
    return 0;
}

