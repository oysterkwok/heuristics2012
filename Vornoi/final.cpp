#include <stdio.h>
#include <iostream>
#include <fstream>
#include <map>
#include "board.cpp"

#define MAXTMPARRLEN 1000

std::pair<int, int> string_test(std::string inp_str) {
	int sp_pos = inp_str.length();
	for (int i = 0; i < inp_str.length()-1; i ++) {
		if (inp_str[i] == ' ') {
			sp_pos = i;
			break;
		}
	}
	if (inp_str.length() <= 1)
		return std::pair<int, int>(-1,-1);
	string sub = inp_str.substr(0, sp_pos);
	int x, y;
	sscanf(inp_str.c_str(), "%d,%d", &x, &y);
	return std::pair<int, int>(x, y);
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		puts("Usage: ./a.out TEAM_NAME");
		return 1;
	}
	
	char tmpstr[MAXTMPARRLEN];
	
	setvbuf(stdin, NULL, _IONBF, 0); 
	setvbuf(stdout, NULL, _IONBF, 0);
	
	printf("%s\n", argv[1]);
	
	vector<pair<int,int> > newTokens;
	// initialize the gameboard
	// and compute intrinsic score of each square
	board b1(50, 0.3, 2);
	
	while (1) {
		if (fgets(tmpstr, MAXTMPARRLEN, stdin) != NULL) {
			std::pair<int, int> nextMove = string_test(tmpstr);
			newTokens.clear();
			if (nextMove.first >= 0)
				newTokens.push_back(nextMove);
			// play the next move
			// if x and y has values
			b1.updateColors(newTokens);
			b1.updateOccupied(newTokens);
			newTokens.clear();
			nextMove = b1.getNextMove1();
			newTokens.push_back(nextMove);
			printf("%d,%d\n", nextMove.first, nextMove.second);
		}
	}
	
	return 0;
}
