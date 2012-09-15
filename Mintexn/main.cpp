#include <iostream>
#include <math.h>
using namespace std; 


float score(int*, float, float);

int MAXSCORE = 1000;

int main(int argc, char * argv[]) {

	// user defined parameters
	int N;
	if (argc == 2) 
		N = atoi (argv[1]);
	else {
		cout << "Please input correct number of parameters" << endl;
		exit(1);
	}
	
	clock_t start, end;
	double cpuTime;
	start = clock();


	int denom[5];
	int optDenom[5];
	float optScore = numeric_limits<float>::infinity();	
	
	int maxdenom = 50;


	// NOTE: optScore is shared
	// Also, #pragma directives are ignored on Ilya's computer
    #pragma omp parallel
    {
    		#pragma omp for
		for (int d1 = 1; d1 <= maxdenom; d1++) {
//		for (int d1 = 1; d1 <= 1; d1++) {
			for (int d2 = d1+1; d2 <= maxdenom; d2++) {
				for (int d3 = d2+1; d3 <= maxdenom; d3++) {
					for (int d4 = d3+1; d4 <= maxdenom; d4++) {
						for (int d5 = d4+1; d5 <= maxdenom; d5++) {
							denom[0] = d1;
							denom[1] = d2;
							denom[2] = d3;
							denom[3] = d4;
							denom[4] = d5;

							int newScore = score(denom,optScore,N);

							if (newScore < optScore) {
								optScore = newScore; 
								optDenom[0] = d1;
								optDenom[1] = d2;
								optDenom[2] = d3;
								optDenom[3] = d4;
								optDenom[4] = d5;
							}
						}
					}
				}
			}
		}
     }

	end = clock();
	cpuTime= (end-start)/ (CLOCKS_PER_SEC);

	cout << optScore << endl;
	cout << "denomiations: " << optDenom[0] << "," << optDenom[1] << "," << optDenom[2] << "," << optDenom[3] << "," << optDenom[4] << endl;
	printf("%f second\n", cpuTime);

	return 0;

}

// Uses dynamic programming to compute weighted score
// Stops if the result cannot be better than optimal score so far (a la A*)
float score(int *denom, float optScore, float N) {


	// (1) Get exact change number for all costs 1...199
	int en[199];
	std::fill_n(en, 99, MAXSCORE);
	for (int i = 0; i < 5; i++) {
		en[denom[i]-1] = 1;
	}

	// for each possible cost (1...99)
	for (int cost = 1; cost <= 99; cost++) {

		// test each possible denomination
		for (int d = 0; d < 5; d++) {
			if ( (cost > denom[d]) && (en[ (cost-1) - denom[d] ] + 1 < en[(cost-1)]) ) {
				en[(cost-1)] = 1 + en[(cost-1) - denom[d]];
			}
		}
	}

	// get exact change number for $1.01 to $1.99
	// ecn for $1.04 is the same as for $0.04
	en[99] = 0;
	for (int i = 100; i < 199; i++)
		en[i] = en[i-100];


	// (2) Compute exchange number going backwards from $1.99
	// en cost is one coin going backwards from $1.00 or $2.00
	float result = 0;
	for (int i = 0; i < 5; i++) {
		en[99 - denom[i]] = 1;
		en[199 - denom[i]] = 1;
	}
	for (int cost = 199; cost > 0; cost--) {
		for (int d = 0; d < 5; d++) {
			if ( (cost + denom[d] < 200) && (en[(cost-1)  + denom[d]] + 1  < en[(cost-1)]) )
				en[(cost-1)] = 1 + en[(cost-1) + denom[d]];
		}

		// only count $0.01 to $0.99 in the final score
		if (cost < 100) {
			// update the score
			if (cost % 5 == 0)
				result += N*en[cost-1];
			else
				result += en[cost-1];

			// check if this path is worth pursuing (A*)
			int costsleft = cost; 
			int multiplesof5left = floor(costsleft/5);
			float minscoreleft = (costsleft-multiplesof5left) + N*multiplesof5left;
			if (result +  minscoreleft >= optScore) {
				return MAXSCORE;
			}
		}
	}

	return result;
}
































