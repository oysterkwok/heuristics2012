#include "board.cpp" 


int main(int argc, char const *argv[])
{
//	vector<vector<pair<int,int> > > tokens;

	int nPlayers = 2;
	int nRounds = 7;
	board b1(50, 0.3, nPlayers);
//	b1.printGrid();

//	b1.printGridCenters();
//	b1.printQuandrantCenters();
//	b1.printIntrinsicValues();

	board b2(50, 0.3, nPlayers);


	vector<pair<int,int> > newTokens; // tokens placed by all the opponents in the last round
	pair<int,int> move;
	for (int i = 0; i < nRounds; i++) {

		// have player 1 move
		b1.updateColors(newTokens);
		b1.updateOccupied(newTokens);
		newTokens.clear();
		move = b1.getNextMove1();
		newTokens.push_back(move);
		cout << "b1: " << move.first << " " << move.second << endl;

		b1.printGrid();
//		b1.printColors();
//		b1.printTokens();
//		b1.printIntrinsicValues();
//		b1.printIsOccupied();


		// have player 2 move
		b2.updateColors(newTokens);
		b2.updateOccupied(newTokens);
		newTokens.clear();
		move = b2.getNextMove();
		newTokens.push_back(move);
		cout << "b2: " << move.first << " " << move.second << endl;

		b2.printGrid();
//		b2.printNearestTokenDist();
//		b2.printColors();
//		b2.printTokens();
//		b2.printIsOccupied();


	}	

	return 0;
}
