#include <iostream>
#include <cmath>
#include <queue>
#include <limits>
#include <stdio.h>
using namespace std;


class square{
public:
	int x;
	int y;
	float value;
	int color;
	float nearestTokenDist;
};

class coordinate{
public:
	int x;
	int y;
	float value;
};


class board{
public:

	// DATA STRUCTURES:
	vector<vector<square> > leftTop;
	vector<vector<square> > leftBottom;
	vector<vector<square> > rightTop;
	vector<vector<square> > rightBottom;
	vector<vector<square> > grid;
	vector<vector<pair<int,int> > > tokens; // vector of token locations
	vector<vector<bool> > isOccupied;

	int ratio; // length of each square
	int quadSize; // length of each quad
	int gridSize; // length of whole grid
	int myColor; // color of this player (e.g. Red, Blue etc.)
	int nPlayers;
	int firstToMove;
	float intrinsicValueParam;

	// METHODS: 
	void initializeQuadrant(vector<vector<square> > & quad, int, int, int );
	void computeIntrinsicValues(float param);
	float computeNetScore(pair<int,int>);
	vector<float> computeScores(pair<int,int>);
	void updateColor(int tokenColor, pair<int,int> newToken);
	void updateOccupied(pair<int,int> newMove);
	
	float computeNetScore1(pair<int,int>);
	vector<float> computeScores1(pair<int,int>);

//public:

	board(int ratio, float, int);
	void displayIntrinsicValues();
	void updateColors(vector<pair<int,int> > newTokens);
	void updateOccupied(vector<pair<int,int> > newMoves);
	pair<int,int> getNextMove();
	pair<int,int> getNextMove1();
	void printGrid();
	void printColors();
	void printGridCenters();
	void printQuandrantCenters();
	void printIntrinsicValues();
	void printIsOccupied();
	void printTokens();
	void printNearestTokenDist();

};





























