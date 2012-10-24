#include "board.h"

// Divides the game arena into 4 quadrants and call initialize for each of them
board::board(int newRatio, float newIVP, int num_players){
	// size parameters
	ratio = newRatio;
	quadSize = 500/ratio;
	gridSize = 1000/ratio;
	myColor = 0;
	firstToMove = 0;
	intrinsicValueParam = newIVP;
	
	// initialize quadrants and intrinsic values
	for (int i = 0; i < quadSize; i++) {
		vector<square> newVec;
		for (int j = 0; j < quadSize; j++) {
			square emptySquare;
			newVec.push_back(emptySquare);
		}				
		leftTop.push_back(newVec);
		rightTop.push_back(newVec);
		leftBottom.push_back(newVec);
		rightBottom.push_back(newVec);
	}
	
	
	initializeQuadrant(leftTop, 0, 0, ratio);
	initializeQuadrant(rightTop, 500, 0, ratio);
	initializeQuadrant(leftBottom, 0, 500, ratio);
	initializeQuadrant(rightBottom, 500, 500, ratio);
	computeIntrinsicValues(intrinsicValueParam); 
	
	// copy quadrant information into grid
	for (int i = 0; i < gridSize; i++) {
		vector<square> newVec;
		vector<bool> newVec1;
		for (int j = 0; j < gridSize; j++) {
			square emptySquare;
			newVec.push_back(emptySquare);
			newVec1.push_back(false);
		}
		grid.push_back(newVec);
		isOccupied.push_back(newVec1);
	}
	
	for (int i = 0; i < quadSize; i++) {
		for (int j = 0; j < quadSize; j++) {
			grid[i         ][j         ] = leftTop[i][j];
			grid[i         ][j+quadSize] = rightTop[i][j];
			grid[i+quadSize][j         ] = leftBottom[i][j];
			grid[i+quadSize][j+quadSize] = rightBottom[i][j];
		}
	}
	
	// initialize tokens
	for (int i = 0; i < num_players; i++) {
		vector<pair<int,int> > emptyTokens;
		tokens.push_back(emptyTokens);
	}
	nPlayers = num_players;
} 

// Initializes the quadrant. Stores center of each square into 2-dimentional array and initializes 
// the value to -1. Here ratio is the size of each square.
// Also initialize color and distance
void board::initializeQuadrant(vector<vector<square> > & quad, int xstart, int ystart, int ratio){
	square old;
	old.y = ystart + (ratio/2);
	old.value = -1;
	
	for (int i = 0; i < quadSize; ++i)
	{	
		old.x = xstart + (ratio/2);
		quad[i][0].x = old.x;
		quad[i][0].y = old.y;
		quad[i][0].value = old.value;
		quad[i][0].color = -1;
		quad[i][0].nearestTokenDist = numeric_limits<float>::infinity();
		
		for (int j = 1; j < quadSize; ++j){
			quad[i][j].x = old.x + ratio;
			quad[i][j].y = old.y;
			quad[i][j].value = old.value;
			quad[i][j].color = -1;
			quad[i][j].nearestTokenDist = numeric_limits<float>::infinity();
			
			old.x = quad[i][j].x;
		}
		old.y = old.y + ratio;
	}
}


void board::displayIntrinsicValues(){
	for (int i = 0; i < quadSize; ++i)
	{
		for (int j = 0; j < quadSize; ++j)
		{
			//cout << rightBottom[i][j].x <<","<<rightBottom[i][j].y<<"  ";
			cout << leftTop[i][j].value <<", ";
		}
		cout<<"\n";
	}
}

float euclideanDistance(pair<int,int> p1, pair<float,float> p2) {
	return sqrt( pow( (float)p1.first - (float)p2.first , 2) + pow( p2.second - p1.second, 2 ) );
}

float euclideanDistance(pair<float,float> p1, pair<float,float> p2) {
	return sqrt( pow( p1.first - p2.first , 2) + pow( p2.second - p1.second, 2 ) );
}

void board::computeIntrinsicValues(float param) {
	int maxVal = 100; // highest intrinsic value a center can have
	float maxDist = sqrt(pow(500.0,2) + pow(500.0,2));
	float diff = maxVal/maxDist;	 // the diff b/w intrinsic value of two neighbours 
	pair<float,float> ltMaxPt = pair<float,float>(1000*param,1000*param);
	pair<float,float> lbMaxPt = pair<float,float>(1000*(1 - param),1000*param);
	pair<float,float> rtMaxPt = pair<float,float>(1000*param,1000*(1 - param));
	pair<float,float> rbMaxPt = pair<float,float>(1000*(1 - param),1000*(1 - param));
	
	/*
	 cout << "LT max: " << 1000*param << " " << 1000*param << endl;
	 cout << "RT max: " << 1000*(1-param) << " " << 1000*param << endl;
	 cout << "LB max: " << 1000*param << " " << 1000*(1-param) << endl;
	 cout << "RB max: " << 1000*(1-param) << " " << 1000*(1-param) << endl;
	 cout << "diff: " << diff << endl;
	 cout << endl;
	 */
	for (int i = 0; i < quadSize; i++) {
		for (int j = 0; j < quadSize; j++) {
			leftTop[i][j].value = maxVal - diff*euclideanDistance(ltMaxPt, pair<float,float>(leftTop[i][j].y, leftTop[i][j].x));
			rightTop[i][j].value = maxVal - diff*euclideanDistance(rtMaxPt, pair<float,float>(rightTop[i][j].y, rightTop[i][j].x));
			leftBottom[i][j].value = maxVal - diff*euclideanDistance(lbMaxPt, pair<float,float>(leftBottom[i][j].y, leftBottom[i][j].x));
			rightBottom[i][j].value = maxVal - diff*euclideanDistance(rbMaxPt, pair<float,float>(rightBottom[i][j].y, rightBottom[i][j].x));
		}
	}
}

// note: we add and remove tokens to avoid copying data so many times
// IMPORTANT: assumes that coloring is already done correctly going in
pair<int,int> board::getNextMove() {
	float bestScore = -numeric_limits<float>::infinity(), newScore;
	pair<int,int> bestMove = pair<int,int>(-1,-1);
	
//	cout << "current move: " << firstToMove << endl;
	
	if (firstToMove < 1) {
//		cout << "move: " << firstToMove << endl;
		vector<pair<int, int> > cand;
		cand.push_back(pair<int, int> (gridSize*(intrinsicValueParam+.1), gridSize*(intrinsicValueParam+.1)));
		cand.push_back(pair<int, int> (gridSize*(1-intrinsicValueParam), gridSize*intrinsicValueParam));
		cand.push_back(pair<int, int> (gridSize*intrinsicValueParam, gridSize*(1-intrinsicValueParam)));
		cand.push_back(pair<int, int> (gridSize*(1-intrinsicValueParam), gridSize*(1-intrinsicValueParam)));
		
		cand.push_back(pair<int, int> (gridSize*intrinsicValueParam-1, gridSize*intrinsicValueParam-1));
		cand.push_back(pair<int, int> (gridSize*(1-intrinsicValueParam)+1, gridSize*intrinsicValueParam-1));
		cand.push_back(pair<int, int> (gridSize*intrinsicValueParam-1, gridSize*(1-intrinsicValueParam)+1));
		cand.push_back(pair<int, int> (gridSize*(1-intrinsicValueParam)+1, gridSize*(1-intrinsicValueParam)+1));
		
		bool ok = false;
		for (int i = 0; i < 4; i ++) {
			if (!isOccupied[cand[i].first][cand[i].second]) {
				bestMove = pair<int, int>(grid[cand[i].first][cand[i].second].y, grid[cand[i].first][cand[i].second].x);
				ok = true;
				break;
			}
		}
		if (!ok) {
			for (int i = 0; i < 4; i ++) {
				if (myColor != grid[cand[i].first][cand[i].second].color) {
					bestMove = pair<int, int>(grid[cand[4+i].first][cand[4+i].second].y, grid[cand[4+i].first][cand[4+i].second].x);
					ok = true;
					break;
				}
			}			
		}
	}	
	else {
	 	// for each square
		pair<int,int> newMove;
		for (int i = 0; i < gridSize; i++) {
			for (int j = 0; j < gridSize; j++) {
				if (isOccupied[i][j]) continue;
				
				// place a (hypothetical) token there
				newMove = pair<int,int>(grid[i][j].y, grid[i][j].x);
				
				// get the new score
				newScore = computeNetScore(newMove);
				if ( newScore > bestScore ) {
					bestMove = newMove;
					bestScore = newScore;
				}
			}
		}
	}
	
	
	// update board colors to reflect this move
	updateColor(myColor, bestMove);
	updateOccupied(bestMove);
	return bestMove;
}


// note: we add and remove tokens to avoid copying data so many times
// IMPORTANT: assumes that coloring is already done correctly going in
pair<int,int> board::getNextMove1() {
	float bestScore = -numeric_limits<float>::infinity(), newScore;
	pair<int,int> bestMove = pair<int,int>(-1,-1);

//	cout << "current move: " << firstToMove << endl;

	if (firstToMove < 1) {
//		cout << "move: " << firstToMove << endl;
		vector<pair<int, int> > cand;
		cand.push_back(pair<int, int> (gridSize*(intrinsicValueParam+.1), gridSize*(intrinsicValueParam+.1)));
		cand.push_back(pair<int, int> (gridSize*(1-intrinsicValueParam), gridSize*intrinsicValueParam));
		cand.push_back(pair<int, int> (gridSize*intrinsicValueParam, gridSize*(1-intrinsicValueParam)));
		cand.push_back(pair<int, int> (gridSize*(1-intrinsicValueParam), gridSize*(1-intrinsicValueParam)));
		
		cand.push_back(pair<int, int> (gridSize*intrinsicValueParam-1, gridSize*intrinsicValueParam-1));
		cand.push_back(pair<int, int> (gridSize*(1-intrinsicValueParam)+1, gridSize*intrinsicValueParam-1));
		cand.push_back(pair<int, int> (gridSize*intrinsicValueParam-1, gridSize*(1-intrinsicValueParam)+1));
		cand.push_back(pair<int, int> (gridSize*(1-intrinsicValueParam)+1, gridSize*(1-intrinsicValueParam)+1));
		
		bool ok = false;
		for (int i = 0; i < 4; i ++) {
			if (!isOccupied[cand[i].first][cand[i].second]) {
				bestMove = pair<int, int>(grid[cand[i].first][cand[i].second].y, grid[cand[i].first][cand[i].second].x);
				ok = true;
				break;
			}
		}
		if (!ok) {
			for (int i = 0; i < 4; i ++) {
				if (myColor != grid[cand[i].first][cand[i].second].color) {
					bestMove = pair<int, int>(grid[cand[4+i].first][cand[4+i].second].y, grid[cand[4+i].first][cand[4+i].second].x);
					ok = true;
					break;
				}
			}			
		}
	}	
	else {
	 	// for each square
		pair<int,int> newMove;
		for (int i = 0; i < gridSize; i++) {
			for (int j = 0; j < gridSize; j++) {
				if (isOccupied[i][j]) continue;
				
				// place a (hypothetical) token there
				newMove = pair<int,int>(grid[i][j].y, grid[i][j].x);
				
				// get the new score
				newScore = computeNetScore1(newMove);
				if ( newScore > bestScore ) {
					bestMove = newMove;
					bestScore = newScore;
				}
			}
		}
	}
	
	
	// update board colors to reflect this move
	updateColor(myColor, bestMove);
	updateOccupied(bestMove);
	return bestMove;
}


void board::updateOccupied(vector<pair<int,int> > newMoves) {
	for (unsigned int i = 0; i < newMoves.size(); i++) {
		updateOccupied(newMoves[i]);
	}
}


void board::updateOccupied(pair<int,int> newMove) {
	//	int xloc = newMove.first / ratio;
	//	int yloc = newMove.second / ratio;
	int yloc = newMove.first / ratio;
	int xloc = newMove.second / ratio;
	isOccupied[yloc][xloc] = true;
}

// returns my score minus the best opponent's score
float board::computeNetScore(pair<int,int> newMove) {
	vector<float> scores = computeScores(newMove);
	float bestOppScore = 0;
	for (int i = 0; i < nPlayers; i++) {
		if (scores[i] > bestOppScore && i != myColor)
			bestOppScore = scores[i];
	}
	
	return scores[myColor] - bestOppScore;
}

// returns my score minus the best opponent's score
float board::computeNetScore1(pair<int,int> newMove) {
	vector<float> scores = computeScores1(newMove);
	float bestOppScore = 0;
	for (int i = 0; i < nPlayers; i++) {
		if (scores[i] > bestOppScore && i != myColor)
			bestOppScore = scores[i];
	}
	
	return scores[myColor] - bestOppScore;
}


vector<float> board::computeScores(pair<int,int> newMove){
	vector<float> scores;
	for (int i = 0; i < nPlayers; i++)
		scores.push_back(0);
	
	//	cout << endl << "NEW MOVE " << newMove.first << " " << newMove.second << endl;
	
	
	float dist;
	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			
			dist = euclideanDistance(pair<int,int>( grid[i][j].y, grid[i][j].x ), newMove);
			if (dist < grid[i][j].nearestTokenDist) 
				scores[myColor] += grid[i][j].value;
			else 
				if (grid[i][j].color >= 0)
					scores[grid[i][j].color] += grid[i][j].value;
			
			
		}
	}	
	
	//	exit(1);
	
	return scores;	
}


vector<float> board::computeScores1(pair<int,int> newMove){
	vector<float> scores;
	for (int i = 0; i < nPlayers; i++)
		scores.push_back(0);
	
	//	cout << endl << "NEW MOVE " << newMove.first << " " << newMove.second << endl;
	
	
	float dist;
	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			for (int i1 = 0; i1 < gridSize; i1+=10) {
				for (int j1 = 0; j1 < gridSize; j1+=10) {
					for (int i2 = 0; i2 < gridSize; i2+=10) {
						for (int j2 = 0; j2 < gridSize; j2+=10) {
							
							dist = euclideanDistance(pair<int,int>( grid[i][j].y, grid[i][j].x ), newMove);
							if (dist < grid[i][j].nearestTokenDist) 
								scores[myColor] += grid[i][j].value;
							else 
								if (grid[i][j].color >= 0)
									scores[grid[i][j].color] += grid[i][j].value;
							
							dist = euclideanDistance(pair<int,int>( grid[i1][j1].y, grid[i1][j1].x ), newMove);
							if (dist < grid[i1][j1].nearestTokenDist) 
								scores[1-myColor] += grid[i1][j1].value;
							else 
								if (grid[i1][j1].color >= 0)
									scores[grid[i1][j1].color] += grid[i1][j1].value;

							dist = euclideanDistance(pair<int,int>( grid[i2][j2].y, grid[i2][j2].x ), newMove);
							if (dist < grid[i2][j2].nearestTokenDist) 
								scores[myColor] += grid[i2][j2].value;
							else 
								if (grid[i2][j2].color >= 0)
									scores[grid[i2][j2].color] += grid[i2][j2].value;

						}
					}}}}
	}	
	
	//	exit(1);
	
	return scores;	
}

// ASSUMPTION: new tokens will be given in order of player color
void board::updateColors(vector<pair<int,int> > newTokens) {
	for (unsigned int i = 0; i < newTokens.size(); i++) {
		updateColor(i+1, newTokens[i]);
		firstToMove ++;
	}
}


// given a colored grid and a new token
// update the owning token if it closer than the previous one
void board::updateColor(int tokenColor, pair<int,int> newToken) {
	tokens[tokenColor].push_back(newToken);
	
	float dist;
	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			
			dist = euclideanDistance(pair<int,int>(grid[i][j].y, grid[i][j].x), newToken);
			if (dist < grid[i][j].nearestTokenDist) {
				grid[i][j].nearestTokenDist = dist;
				grid[i][j].color = tokenColor;
			}
			else if (dist == grid[i][j].nearestTokenDist && grid[i][j].color != tokenColor)
				grid[i][j].color = -1;
		}
	}
}


void board::printGrid(){

	int counts[nPlayers];
	for (int i = 0; i < nPlayers; i ++) {
		counts[i] = 0;
	}
	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {	
			if (isOccupied[i][j])
				cout << (char)(grid[i][j].color + 65) << " ";
			else
				cout << (char)(grid[i][j].color + 97) << " ";
			counts[grid[i][j].color] ++;
		}
		cout << endl;
	}
	cout << endl;
	
	for (int i = 0; i < nPlayers; i ++) {
		cout << "ply " << i << " : " << counts[i] << endl;
	}
	/*
	 for (int i = 0; i < gridSize; i++) {
	 for (int j = 0; j < gridSize; j++) {	
	 cout << grid[i][j].color << " ";
	 }
	 cout << endl;
	 }
	 cout << endl;
	 */
}

void board::printColors(){
	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {	
			cout << grid[i][j].color << " ";
		}
		cout << endl;
	}
	cout << endl;
	
}


void board::printQuandrantCenters() {
	cout << "LEFT TOP" << endl;
	for (int i = 0; i < quadSize; i++) {
		for (int j = 0; j < quadSize; j++) {
			cout << "(" << leftTop[i][j].x << "," << leftTop[i][j].y << ") ";
		}
		cout << endl;
	}
	
	cout << "RIGHT TOP" << endl;
	for (int i = 0; i < quadSize; i++) {
		for (int j = 0; j < quadSize; j++) {
			cout << "(" << rightTop[i][j].x << "," << rightTop[i][j].y << ") ";
		}
		cout << endl;
	}
	
	cout << "LEFT BOTTOM" << endl;
	for (int i = 0; i < quadSize; i++) {
		for (int j = 0; j < quadSize; j++) {
			cout << "(" << leftBottom[i][j].x << "," << leftBottom[i][j].y << ") ";
		}
		cout << endl;
	}
	
	
	cout << "RIGHT BOTTOM" << endl;
	for (int i = 0; i < quadSize; i++) {
		for (int j = 0; j < quadSize; j++) {
			cout << "(" << rightBottom[i][j].x << "," << rightBottom[i][j].y << ") ";
		}
		cout << endl;
	}
	
	
}

void board::printGridCenters() {
	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			cout << "(" << grid[i][j].x << "," << grid[i][j].y << ") ";
		}
		cout << endl;
	}
	
}

void board::printIsOccupied() {
	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			if (isOccupied[i][j])
				cout << "+";
			else
				cout << "-";
		}
		cout << endl;
	}
}


void board::printIntrinsicValues() {
	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			//			cout << "(" << grid[i][j].x << "," << grid[i][j].y << ") ";
			printf("(%4.2f)", grid[i][j].value);
		}
		cout << endl;
	}			
}

void board::printNearestTokenDist(){
	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			//			cout << "(" << grid[i][j].x << "," << grid[i][j].y << ") ";
			printf("(%4.2f)", grid[i][j].nearestTokenDist);
		}
		cout << endl;
	}
	
}


void board::printTokens() {
	cout << "TOKENS: " << endl;
	for (unsigned int i = 0; i < tokens.size(); i++) {
		cout << "player "<<i+1<<": ";
		for (unsigned int j = 0; j < tokens[i].size(); j++) 
			cout << "("  << tokens[i][j].first << " " << tokens[i][j].second << ")";
		cout << endl;
	}
	cout << endl;
}



/*
 // Updates the intrinsic value for each square center.
 // Performs B.F.S
 // if pram is 0.3 that means the square approximately 30% from the upper left hand 
 // corner has the highest value.
 void board::computeIntrinsicValues(float param){  
 int intrinsic_val = 100; // highest intrinsic value a center can have
 int diff = 2;  // the diff b/w intrinsic value of two neighbours 
 
 queue<coordinate> myqueue;
 coordinate pair;
 pair.x = param * quadSize;
 pair.y = param * quadSize;
 pair.value = intrinsic_val;
 
 myqueue.push(pair);
 
 
 // B.F.S
 while(!myqueue.empty()){
 
 pair = myqueue.front();
 
 
 
 if( (pair.x < quadSize) && (pair.y < quadSize) && (pair.x >= 0) && (pair.y >= 0) && (leftTop[pair.x][pair.y].value == -1)  ){
 
 leftTop[pair.x][pair.y].value = pair.value;
 leftBottom[pair.x][pair.y].value = pair.value;
 rightTop[pair.x][pair.y].value = pair.value;
 rightBottom[pair.x][pair.y].value = pair.value;
 
 coordinate pair2;
 pair2.value = pair.value - diff;
 
 // for niegbhour squares
 pair2.x = pair.x - 1;
 pair2.y = pair.y;
 myqueue.push(pair2);
 
 pair2.x = pair.x + 1;
 pair2.y = pair.y;
 myqueue.push(pair2);
 
 pair2.x = pair.x;
 pair2.y = pair.y - 1;
 myqueue.push(pair2);
 
 
 pair2.x = pair.x;
 pair2.y = pair.y + 1;
 myqueue.push(pair2);
 
 // for diagnol squares
 pair2.value = pair.value - (diff*1.5);
 
 pair2.x = pair.x - 1;
 pair2.y = pair.y + 1;
 myqueue.push(pair2);
 
 pair2.x = pair.x + 1;
 pair2.y = pair.y - 1;
 myqueue.push(pair2);
 
 pair2.x = pair.x + 1;
 pair2.y = pair.y + 1;
 myqueue.push(pair2);
 
 pair2.x = pair.x - 1;
 pair2.y = pair.y - 1;
 myqueue.push(pair2);
 
 }
 
 myqueue.pop();
 
 
 }
 }
 
 */









