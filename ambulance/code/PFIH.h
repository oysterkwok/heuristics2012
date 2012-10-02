#include<iostream>
#include<string>
#include<string.h>
#include<fstream>
#include<cstdlib>
#include<queue>
#include<sstream>
#include<stdio.h>
#include<vector>
using namespace std;

struct patient {
	int id;
	int x;
	int y;
	int t;
	vector<float> cost;
	float lowCost;
	int nearestHospital;
};

class comparePatients {
public:
	bool operator() (patient& p1, patient& p2);
};

void PFIH(float, string, string, string);
void writeRoutesToFile(string fileName, vector<vector<int> > routes, vector<pair<int,int> >, int*, vector<patient>);
void writeRoutesToFileStd(string fileName, vector<vector<int> > routes, vector<pair<int,int> > hospLocs, int * nambulances, vector<patient> patients);
void writeRoutesToFileUnformatted(string fileName, vector<vector<int> > routes, vector<pair<int,int> > hospLocs, int * nambulances, vector<patient> patients);
vector<float> computeCost(int, int, int, float, vector<pair<int,int> >);
void findNextBestPatient();
pair<float,int> min(vector<float>);
float computeScore(vector<vector<int> > routes, vector<patient> patients, vector<pair<int,int> > hospLocs);
void printRoutes(vector<vector<int> >);
void printRoute(vector<int > route);
