//
//  mint_problem.cpp
//  HeuristicProblemSolving
//  mint problem
//
//  Created by Haoquan Guo on 9/14/12.
//  Copyright 2012 NYU. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "cost_calc.h"

using namespace std;
float N = 26.37;
int t = 2;

void mint_output(int * exact_denomination, int * exchange_denomination, string output_file_name);
void mint_output(int * exact_denomination, int * exchange_denomination, string output_file_name) {
    
    ofstream fout(output_file_name.c_str());
    
    fout << "EXACT_CHANGE_NUMBER:\n";
    fout << "COIN_VALUES: ";
    for (int i = 0; i < 4; i ++) {
        fout << exact_denomination[i] << ',';
    }
    fout << exact_denomination[4] << '\n';
    
    int coins[100][7]; // 0-99: price; 0: total coin number (exclude 100), 1-5: d1-d5, 6: 100
    memset(coins, 0, sizeof(coins));
    for (int i = 1; i < 100; i ++) {
        coins[i][0] = 10000;
    }
    for (int dst = 1; dst < 100; dst ++) {
        for (int i = 0; i < 5; i ++) {
            int src = dst - exact_denomination[i];
            if (src >= 0 && coins[dst][0] > coins[src][0]) {
                for (int j = 0; j < 6; j ++) {
                    coins[dst][j] = coins[src][j];
                }
                coins[dst][0] ++;
                coins[dst][i+1] ++;
            }
        }
    }
    
    for (int i = 1; i < 100; i ++) {
        fout << i << ':';
        bool is_first = true;
        for (int j = 1; j < 6; j ++) {
            for (int k = 0; k < coins[i][j]; k ++) {
                if (is_first) {
                    is_first = false;
                    fout << exact_denomination[j-1];
                }
                else
                    fout << ',' << exact_denomination[j-1];
            }
        }
        fout << '\n';
    }
    fout << "//\n\n";
    
    fout << "EXCHANGE_NUMBER:\n";
    fout << "COIN_VALUES: ";
    for (int i = 0; i < 4; i ++) {
        fout << exchange_denomination[i] << ',';
    }
    fout << exchange_denomination[4] << '\n';

    memset(coins, 0, sizeof(coins));
    for (int i = 1; i < 100; i ++) {
        coins[i][0] = 10000;
    }
    int n_coins = 0;
    bool has_update = true;
    while (has_update) {
        n_coins ++;
        has_update = false;
        for (int dst = 1; dst < 100; dst ++) {
            if (coins[dst][0] > n_coins) {
                for (int i = 0; i < 5; i ++) {
                    // case 1
                    int src = dst - exchange_denomination[i];
                    int n100 = 0;
                    if (src < 0) {
                        n100 --;
                        src += 100;
                    }
                    if (coins[src][0] == n_coins - 1) {
                        has_update = true;
                        for (int j = 0; j < 7; j ++) {
                            coins[dst][j] = coins[src][j];
                        }
                        coins[dst][0] ++;
                        coins[dst][i+1] ++;
                        coins[dst][6] += n100;
                        break;
                    }
                    
                    // case 2
                    src = dst + exchange_denomination[i];
                    n100 = 0;
                    if (src >= 100) {
                        n100 ++;
                        src -= 100;
                    }
                    if (coins[src][0] == n_coins - 1) {
                        has_update = true;
                        for (int j = 0; j < 7; j ++) {
                            coins[dst][j] = coins[src][j];
                        }
                        coins[dst][0] ++;
                        coins[dst][i+1] --;
                        coins[dst][6] += n100;
                        break;
                    }
                }
            }
        }
    }
    
    for (int i = 1; i < 100; i ++) {
        fout << i << ':';
        bool is_first = true;
        for (int j = 1; j < 7; j ++) {
            for (int k = 0; k < coins[i][j]; k ++) {
                int coin_value;
                if (j < 6) {
                    coin_value = exact_denomination[j-1];
                }
                else {
                    coin_value = 100;
                }
                if (is_first) {
                    is_first = false;
                    fout << coin_value;
                }
                else {
                    fout << ',' << coin_value;
                }
            }
        }
        is_first = true;
        for (int j = 1; j < 7; j ++) {
            for (int k = 0; k < -coins[i][j]; k ++) {
                int coin_value;
                if (j < 6) {
                    coin_value = exchange_denomination[j-1];
                }
                else {
                    coin_value = 100;
                }
                if (is_first) {
                    is_first = false;
                    fout << ';' << coin_value;
                }
                else
                    fout << ',' << coin_value;
            }
        }
        fout << '\n';
    }
    fout << "//\n\n";
}

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
//    run_normal_exchange();
    int den[5];
    den[0] = 1;
    den[1] = 5;
    den[2] = 8;
    den[3] = 25;
    den[4] = 40;
    int exchange[5];
    exchange[0] = 1;
    exchange[1] = 5;
    exchange[2] = 17;
    exchange[3] = 25;
    exchange[4] = 40;
    string path = "/Users/oyster/Desktop/mint_out.txt";
    mint_output(den, exchange, path);
    return 0;
}

