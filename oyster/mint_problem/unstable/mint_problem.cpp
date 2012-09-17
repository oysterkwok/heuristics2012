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

/*
 output the result of mint problem to the output file
 arguments:
    int * exact_denomination - the denomination set of exact change problem, size is 5
    int * exchange_denomination - the denomination set of exchange problem, size is 5
    string output_file_name - the output file path and name
 procedure:
    recalculate the number of coins of each price once again, and then output as the format given by the sample
 */
void mint_output(int * exact_denomination, int * exchange_denomination, string output_file_name);
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
    fout.close();
}

/*
 This is only an example entry for testing the output function
 */
int main (int argc, const char * argv[])
{
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
    exchange[3] = 35;
    exchange[4] = 40;
    string path = "mint_out.txt";
    mint_output(den, exchange, path);
    return 0;
}

