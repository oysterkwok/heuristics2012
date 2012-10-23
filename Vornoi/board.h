#include <iostream>
#include <queue>
using namespace std;


class square{
public:
	int x;
	int y;
	float value;
};

class coordinate{
public:
	int x;
	int y;
	float value;
};


class board{
	// Assumption size of square is 10 * 10, thus size of this quadrant is 500 * 500
	square leftTop[50][50];
	square rightTop[50][50];
	square leftBottom[50][50];
	square rightBottom[50][50];

	public:

	void initializeBoard();


	void initializeQuadrant(square quad[50][50], int, int, int );


	void display();

	
	void initializeValue(float param);
};