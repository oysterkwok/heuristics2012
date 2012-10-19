#include "board.h"

	// Divides the game arena into 4 quadrants and call initialize for each of them
	void board::initializeBoard(){
		int ratio = 10;
		initializeQuadrant(leftTop, 0, 0, ratio);
		initializeQuadrant(rightTop, 500, 0, ratio);
		initializeQuadrant(leftBottom, 0, 500, ratio);
		initializeQuadrant(rightBottom, 500, 500, ratio);
	
	} 

	// Initializes the quadrant. Stores center of each square into 2-dimentional array and initializes 
	// the value to -1. Here ratio is the size of each square.
	void board::initializeQuadrant(square quad[50][50], int xstart, int ystart, int ratio){
		square old;
		old.y = ystart + (ratio/2);
		old.value = -1;
		
		for (int i = 0; i < 50; ++i)
		{	
			old.x = xstart + (ratio/2);
			quad[i][0].x = old.x;
			quad[i][0].y = old.y;
			quad[i][0].value = old.value;

			for (int j = 1; j < 50; ++j){
				quad[i][j].x = old.x + ratio;
				quad[i][j].y = old.y;
				quad[i][j].value = old.value;
				old.x = quad[i][j].x;
			}
			old.y = old.y + ratio;
		}
		
	}


	void board::display(){
		for (int i = 0; i < 50; ++i)
		{
			for (int j = 0; j < 50; ++j)
			{
				//cout << rightBottom[i][j].x <<","<<rightBottom[i][j].y<<"  ";
				cout << leftTop[i][j].value <<", ";
			}
			cout<<"\n";
		}
	}

	
	// Updates the intrinsic value for each square center.
	// Performs B.F.S
	// if pram is 0.3 that means the square approximately 30% from the upper left hand 
	// corner has the highest value.
	void board::initializeValue(float param){  
		int intrinsic_val = 100; // highest intrinsic value a center can have
		int diff = 2;  // the diff b/w intrinsic value of two neighbours 
    		   		
	  	queue<coordinate> myqueue;
        coordinate pair;
        pair.x = param * 50;
        pair.y = param * 50;
        pair.value = intrinsic_val;

        myqueue.push(pair);

        // B.F.S
        while(!myqueue.empty()){
        	
        	pair = myqueue.front();
        	if( (pair.x < 50) && (pair.y < 50) && (leftTop[pair.x][pair.y].value == -1) && (pair.x >= 0) && (pair.y >= 0) ){
        		
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