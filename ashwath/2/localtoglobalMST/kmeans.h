#include<DisjSets.h>


//#include <iostream>
//#include <vector>
//#include <fstream>
#include <math.h>
//using namespace std;

struct point{
	int id;
	int x;
	int y;
	int z;
};


double find_distance(point , point );

bool check_change(vector<point> , vector<point> );

vector<point> mean_of_cluster(vector<vector<point> > , int );

vector<vector<point> > kmeans(vector<point> , int );

