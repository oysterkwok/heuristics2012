#include "board.cpp" 


int main(int argc, char const *argv[])
{
	board b1;
	b1.initializeBoard();

	// 0.3 means the square approximately 30% from the upper left hand corner has the highest value
	b1.initializeValue(0.3); 


	b1.display();

	return 0;
}