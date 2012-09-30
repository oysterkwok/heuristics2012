#include <iostream>
#include <vector>
#include <fstream>
#include <math.h>
#include <sstream>
#include <algorithm>
using namespace std;

struct point{
	float x;
	float y;
};


bool check_change(vector<point> , vector<point> );

float find_distance(point , point );

vector<point> kmeans(vector<point> , int );

vector<point> mean_of_cluster(vector<vector<point> >);

void print(vector<vector<point> > );

bool sortfunction (vector<point>  , vector<point> );  

vector<vector<point> > unionClusters(vector<vector<point> >, vector<point>);	

vector<double> updateradius(vector<vector<point> > , vector<point> );

