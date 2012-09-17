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
#include <boost/array.hpp>
#include "mint_problem.h"

using namespace std;

void mint_output(int * exact_denomination, int * exchange_denomination, string output_file_name) {
    
    ofstream fout(output_file_name.c_str());
    int coins[100][7]; // 0-99: price; 0: total coin number (exclude 100), 1-5: d1-d5, 6: 100

    // exact change
    fout << "EXACT_CHANGE_NUMBER:\n";
    fout << "COIN_VALUES: ";
    for (int i = 0; i < 4; i ++) {
        fout << exact_denomination[i] << ',';
    }
    fout << exact_denomination[4] << '\n';
    
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
    
    // exchange
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
                    // case 1: forword
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
                    
                    // case 2: backword
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
                    coin_value = exchange_denomination[j-1];
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
    fout.close();
}

float exact_cost(int * denominations, float optScore, float N) {
    float score = 0;
    int coins[100]; // 0-99: price
    memset(coins, 10000, sizeof(coins));
    coins[0] = 0;
    
    for (int dst = 1; dst < 100; dst ++) {
        for (int i = 0; i < 5; i ++) {
            int src = dst - denominations[i];
            if (src >= 0 && coins[dst] > coins[src] + 1) {
                coins[dst] = coins[src] + 1;
            }
        }
        if (coins[dst] == 10000) {
            return optScore;
        }
        if (dst % 5 == 0) {
            score += N * coins[dst];
        }
        else {
            score += coins[dst];
        }
        if (optScore > 0 && score + 99 - dst >= optScore) // purning, if optScore is defined and smaller than the current score, return failure
            return optScore;

    }
    return score;
}

float exchange_cost(int * denominations, float optScore, float N) {
    
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
            return optScore;
    }
    
    for (int i = 1; i < 100; i ++) {  // return failure if any price is not reachable
        if (reachable[i] < 0) {
            return optScore;
        }
    }
    return score;
}

int * run_exact(float N) {
    float min_score = 10000 * 100 * N;
    int * exact_denomination = new int[5];
    int deno[5];
    deno[0] = 1;
    for (deno[1] = deno[0] + 1; deno[1] < 97; deno[1] ++) {
        for (deno[2] = deno[1] + 1; deno[2] < 98; deno[2] ++) {
            for (deno[3] = deno[2] + 1; deno[3] < 99; deno[3] ++) {
                for (deno[4] = deno[3] + 1; deno[4] < 100; deno[4] ++) {
                    float score = exact_cost(deno, min_score, N);
                    if (score < min_score) {
                        min_score = score;
                        for (int i = 0; i < 5; i ++) {
                            exact_denomination[i] = deno[i];
                        }
                    }
                }
            }
        }
    }
    return exact_denomination;
}

int * run_exchange(float N) {
    float min_score = 10000 * 100 * N;
    int * exchange_denomination = new int[5];
    int deno[5];
    deno[0] = 1;
    for (deno[0] = 1; deno[0] < 47; deno[0] ++) {
        for (deno[1] = deno[0] + 1; deno[1] < 48; deno[1] ++) {
            for (deno[2] = deno[1] + 1; deno[2] < 49; deno[2] ++) {
                for (deno[3] = deno[2] + 1; deno[3] < 50; deno[3] ++) {
                    for (deno[4] = deno[3] + 1; deno[4] < 51; deno[4] ++) {
                        float score = exchange_cost(deno, min_score, N);
                        if (score < min_score) {
                            min_score = score;
                            for (int i = 0; i < 5; i ++) {
                                exchange_denomination[i] = deno[i];
                            }
                        }
                    }
                }
            }
        }
    }

    return exchange_denomination;
}

/*
 This is only an example entry for testing, N is hardcoded
 */
int main (int argc, const char * argv[])
{
    clock_t t_start = clock();
    float N = 2.49;
    int * exact_denomination = run_exact(N);
    int * exchange_denomination = run_exchange(N);
    string output_path = "mint_out.txt";
    mint_output(exact_denomination, exchange_denomination, output_path);
    clock_t t_end = clock();
    cout << "Time Elapsed: " << 1.0 * (t_end-t_start) / CLOCKS_PER_SEC << " seconds\n";
    return 0;
}

