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

//pair<int,int> getNextMove();

int main(int argc, char* argv[]) {
	//ofstream fout("client.log");
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
//	theBoard.ComputeIntrinsicScores(0.3);

//	while (1) {
	int i = 0;
		while (i < 7 && fgets(tmpstr, MAXTMPARRLEN, stdin) != NULL) {
			i ++;
			//fout << "I got [" << tmpstr << "]\n";
			//fout.flush();
			//sscanf(tmpstr, "%d,%d", &x, &y);
			if (!tmpstr)
				break;
			std::pair<int, int> nextMove = string_test(tmpstr);
			//fout << "I parsed " << nextMove.first << "," << nextMove.second << "\n";
			//fout.flush();
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
			//fout << " I move " << nextMove.first << "," << nextMove.second << "\n";
			//fout.flush();
		} 
		//else {
		//	break;
		//}
	//}
	return 0;
}

/*
pair<int,int> getNextMove(board b, int ratio) {
	// for each square
		// place a token there
		// color the board
		// get the new score

	// return the move that yields the highest score
}

*/

























