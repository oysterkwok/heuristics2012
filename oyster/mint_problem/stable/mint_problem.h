//
//  mint_problem.h
//  HeuristicProblemSolving
//
//  Created by Haoquan Guo on 9/17/12.
//  Copyright 2012 NYU. All rights reserved.
//

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

void exact_cost(int * denominations, float N);
void exchange_cost(int * denominations, float N);

void run_exact(float N);
void run_exchange(float N);
