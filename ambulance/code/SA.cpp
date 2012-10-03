#include"PFIH.h"
#include "hospital_loc.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <algorithm>


int multinomial5(float*, float);
vector<vector<int> > mutation_swap(vector<vector<int> >, vector<patient>);
vector<vector<int> > mutation_insert(vector<vector<int> >, vector<patient>);
vector<vector<int> > mutation_scramble(vector<vector<int> >, vector<patient>);
vector<vector<int> > mutation_inversion(vector<vector<int> >, vector<patient>);
vector<vector<int> > mutation_pfih(vector<vector<int> > routes, vector<patient> patients, vector<pair<int,int> > hospitalLocations);
float computeScoreIfValid(vector<vector<int> > routes, vector<patient> patients, vector<pair<int,int> > hospLocs);



int main(int argc, char * argv[]) {


	// PREPROCESSING FOR SUBMISSION 
	// ALL INPUT IS GIVEN THROUGH STANDARD INPUT
	string inputDataFileName = "input_data.txt";
	ofstream inData(inputDataFileName.c_str());
	string inputString;
	while (cin >> inputString) {
		inData << inputString << "\n";
	}
	inData.close();

	// compute hospital locations
	findHospitalLocations(inputDataFileName);

	// do initial PFIH computation
	float timeWeight = 500;
	string outputFileName = "initial_routes.txt";
	string hospitalLocationsFileName = "hospitalLocations.txt";

	PFIH(timeWeight, inputDataFileName, outputFileName, hospitalLocationsFileName);	


	// input patient information
	vector<patient> patients;
	ifstream in(inputDataFileName.c_str());
	string line;
	char c;
	in >> line;
	getline(in, line);
	patient newPatient;
	int count = 1;
	while (strcmp(line.substr(0,8).c_str(), "hospital") && patients.size() < 300 ) {
		getline(in, line);
		istringstream readLine(line);
		readLine >> newPatient.x >> c >> newPatient.y >> c >> newPatient.t;
		newPatient.id = count++;
		patients.push_back(newPatient);
	}

	while (strcmp(line.substr(0,8).c_str(), "hospital")) // skip ahead to hospital data
		getline(in, line);

	// input sets of hospitals and ambulances
	// NOTE: no empty lines between "hospital" and number of ambulances or between lines of data
	int nambulances[5];
	vector<vector<int> > routes; 	// each route is a sequence of indices
									// 1003 denotes hospital 3
									// and 57 denotes patient 57
	for (int i = 0; i < 5; i++) {
		in >> nambulances[i];
		for (int j = 0; j < nambulances[i]; j++) 
			routes.push_back(vector<int> (1,1000+i+1));
	}



	// input hospital locations
//	string hospitalLocationsFileName = "hospitalLocations.txt";
	ifstream hlin(hospitalLocationsFileName.c_str());
	getline(hlin, line);
	vector<pair<int,int> > hospitalLocations(5);
	int xloc, yloc;
	for (int i = 0; i < 5; i++) {
		hlin >> xloc >> yloc;
		hospitalLocations[i] = pair<int,int>(xloc, yloc);
	}
	hlin.close();

	// initialize costs of all the patients to each hospital
	// also store the ID of the nearest hospital (to tell the EMTs)
	pair<float,int> temp;
	float weight = 500;
	for (unsigned int i = 0; i < patients.size(); i++) {
		patients[i].cost = computeCost(patients[i].x, patients[i].y, patients[i].t, weight, hospitalLocations);
		temp = min(patients[i].cost);
		patients[i].lowCost = temp.first;
		patients[i].nearestHospital = temp.second + 1000 + 1;
	}



	srand ( time(NULL) );

	// input initial solution
	vector<vector<int> > s, sPrime; 
	float score, newScore;

	string initFileName = "initial_routes.txt";
	string word;
	int entry;
	ifstream inInit(initFileName.c_str());
	while (getline(inInit, line)) {
		vector<int> newRoute;
		istringstream inLine(line);
		while (inLine >> word) {
			if (word[0] == 'H')
				entry = 1000 + atoi( word.substr( 1, 100).c_str());
			else if (word[0] == 'P')
				entry = atoi( word.substr( 1, 100).c_str());
			else {
				cout << "ERROR: #4" << endl;
				exit(1);
			}
			newRoute.push_back(entry);
		}
		s.push_back(newRoute);
	}

	inInit.close();

	bool sp[500];
	for (int i = 0; i < s.size(); i ++) {
		for (vector<int>::iterator iter = s[i].begin(); iter != s[i].end(); iter ++) {
			if (sp[*iter]) {
				s[i].erase(iter);
			}
			else {
				sp[s[i][*iter]] = true;
			}
		}
	}
	
	score = computeScore(s, patients, hospitalLocations);
//	cout << "initial score: " << score << endl;

	// write out this solution 
	writeRoutesToFileStd(outputFileName.c_str(), s, hospitalLocations, nambulances, patients);

/*

	float mutProbs[5]; 	// contains "probability" masses for selecting 1 of 5 mutation operartors
	float mutProbTot = 5;
	for (int i = 0; i < 5; i++) 
		mutProbs[i] = 1.0;
	mutProbs[4] = 2.5;

	// for some fixed number of iterations (should use convergence criteria?)
	int maxIter = 3.0*pow(10,4);			// maximum number of iterations for SA
	int coolingPeriod = 100; 		// number of iterations before changing temperature
	float coolingRate = 1 - 0.05; 	// by how much does T decrease (or increase)?
	float T = 100;
	int index;
	float delta;
	for (int i = 0; i < maxIter; i++) {
		if (i % 10000 == 0)
			cout << "begin iteration " << i+1 << "/" << maxIter << endl;
		// set T 
		if (i > 0 && i % coolingPeriod == 0)
			T = T * coolingRate;

		// perform some mutation
		index = multinomial5(mutProbs, mutProbTot);
		switch(index) {
			case 0:
				sPrime = mutation_swap(s, patients);
				break;
			case 1:
				sPrime = mutation_insert(s, patients);
				break;
			case 2:
				sPrime = mutation_scramble(s, patients);
				break;
			case 3:
				sPrime = mutation_inversion(s, patients);
				break;
			case 4:
				sPrime = mutation_pfih(s, patients, hospitalLocations);
				break;
			default:
				cout << "ERROR: type 3" << endl;
				exit(1);
		}




		// compute Delta (-1 if route is invalid)
		newScore = computeScoreIfValid(sPrime, patients, hospitalLocations);

		if (newScore > -1) {
			// decide whether to keep this new solution or not
			delta = newScore - score;

			if (delta > 0) {
				s = sPrime;
				score = newScore;
				cout << newScore << endl;
				cout << delta << endl;

			}
			else 
				if ( exp(-delta / T) > ((float) rand() / (RAND_MAX)) ){
					s = sPrime;
					score = newScore;
//					cout << newScore << endl;
//					cout << delta << endl;
				}
		}
	}

	cout << "New Best Score: " << computeScore(s, patients, hospitalLocations) << endl;
	*/
	return 0;
}



int multinomial5(float* probs, float total) {
	float r = total * (float) rand() / (RAND_MAX+1);

	float val = 0;
	int i;
	for (i = 0; i < 5; i++) {
		val += probs[i];
		if (val > r)
			return i;
	}
	
	return i;
}

vector<vector<int> > mutation_swap(vector<vector<int> > routes, vector<patient> patients) {
	vector<vector<int> > newRoutes = routes;
	int r1 = rand() % routes.size();		// route 1
	int r2 = rand() % routes.size();		// route 2	
	int p1 = 1 + rand() % (routes[r1].size()-1); // position 1 (exclude initial hospital)
	int p2 = 1 + rand() % (routes[r2].size()-1); // position 2 (exclude initial hospital)


	// swap the positions
	newRoutes[r1][p1] = routes[r2][p2];
	newRoutes[r2][p2] = routes[r1][p1];

	// if routes end in a patient, let the ambulance go to the nearest hospital
	// this gives us an invariant: all routes begin and end at hospitals
	if (newRoutes[r1][newRoutes[r1].size()-1] < 1000) 
		newRoutes[r1].push_back( patients[ newRoutes[r1][ newRoutes[r1].size() - 1] - 1].nearestHospital );
	if (newRoutes[r2][newRoutes[r2].size()-1] < 1000) 
		newRoutes[r2].push_back( patients[ newRoutes[r2][ newRoutes[r2].size() - 1] - 1].nearestHospital );

	return newRoutes;
	
}
vector<vector<int> > mutation_insert(vector<vector<int> > routes, vector<patient> patients) {
	vector<vector<int> > newRoutes = routes;
	int r1 = rand() % routes.size();		// route 1
	int r2 = rand() % routes.size();		// route 2	
	int p1 = 1 + rand() % (routes[r1].size()-1); // position 1 (exclude initial hospital)
	int p2 = 1 + rand() % (routes[r2].size()-1); // position 2 (exclude initial hospital)

	// move p1 from r1 to p2 of r2  
	newRoutes[r2].insert( newRoutes[r2].begin() + p2, routes[r1][p1]);
	newRoutes[r1].erase(newRoutes[r1].begin() + p1);

	// if routes end in a patient, let the ambulance go to the nearest hospital
	// this gives us an invariant: all routes begin and end at hospitals
	if (newRoutes[r1][newRoutes[r1].size()-1] < 1000) 
		newRoutes[r1].push_back( patients[ newRoutes[r1][ newRoutes[r1].size() - 1] - 1].nearestHospital );
	if (newRoutes[r2][newRoutes[r2].size()-1] < 1000) 
		newRoutes[r2].push_back( patients[ newRoutes[r2][ newRoutes[r2].size() - 1] - 1].nearestHospital );

	return newRoutes;

}
vector<vector<int> > mutation_scramble(vector<vector<int> > routes, vector<patient> patients) {
	
	vector<vector<int> > newRoutes = routes;
	int r = rand() % routes.size();		// the route
	int p1 = 1 + rand() % (routes[r].size()-1); // position 1 (exclude initial hospital)
	int p2 = 1 + rand() % (routes[r].size()-1); // position 2 (exclude initial hospital)

	// scrambe all elements between p1 and p2, inclusive

	random_shuffle ( newRoutes[r].begin() + p1, newRoutes[r].begin() + p2 );

	// if routes end in a patient, let the ambulance go to the nearest hospital
	// this gives us an invariant: all routes begin and end at hospitals
	if (newRoutes[r][newRoutes[r].size()-1] < 1000) 
		newRoutes[r].push_back( patients[ newRoutes[r][ newRoutes[r].size() - 1] - 1].nearestHospital );


	return newRoutes;

}
vector<vector<int> > mutation_inversion(vector<vector<int> > routes, vector<patient> patients) {

	vector<vector<int> > newRoutes = routes;
	int r = rand() % routes.size();		// the route
	int p1 = 1 + rand() % (routes[r].size()-1); // position 1 (exclude initial hospital)
	int p2 = 1 + rand() % (routes[r].size()-1); // position 2 (exclude initial hospital)

	// reverse all elements between p1 and p2, inclusive
	reverse ( newRoutes[r].begin() + p1, newRoutes[r].begin() + p2 );

	// if routes end in a patient, let the ambulance go to the nearest hospital
	// this gives us an invariant: all routes begin and end at hospitals
	if (newRoutes[r][newRoutes[r].size()-1] < 1000) 
		newRoutes[r].push_back( patients[ newRoutes[r][ newRoutes[r].size() - 1] - 1].nearestHospital );

	return newRoutes;	

}

// the paper's version of this was too computationally expensive, so 
// I just used the one where you select one patient/hospital and insert 
// it into the best possible location
vector<vector<int> > mutation_pfih(vector<vector<int> > routes, vector<patient> patients, vector<pair<int,int> > hospitalLocations) {
	vector<vector<int> > newRoutes = routes;
	vector<vector<int> > bestRoutes = routes;

	int r = rand() % routes.size();		// the route
	int p_ind;
	do { p_ind = 1 + rand() % (routes[r].size()-1);} // position (exclude initial hospital) 
	 while (routes[r][p_ind] > 1000);

	float bestScore = computeScore(routes, patients, hospitalLocations);



	vector<int> newRoute;
	for (int j = 0; j < routes.size(); j++) {	// consider route j
		for (int k = 0; k < routes[j].size(); k++) {		// look at position k in route j
			newRoute = routes[j];
			newRoute.insert(newRoute.begin() + k + 1, routes[r][p_ind]);


			// if route ends in a patient, let the ambulance go to the nearest hospital
			// this gives us an invariant: all routes begin and end at hospitals
			if (newRoute[newRoute.size()-1] < 1000) 
				newRoute.push_back( patients[ newRoute[ newRoute.size() - 1] - 1].nearestHospital );
			
			// do not consider ones that violate the rules
			// (1) load capacity of ambulances (no more than 4 patients between hospitals)
			int load = 1;					
			int q;
			for (q = k; q > 0; q--){
				if (newRoute[q] > 1000) break;
				load++;
			}
			if (q == 0 && newRoute[0] < 1000) {
				cout << "ERROR: #1" << endl;
				exit(1);
			}
			for (q = k+2; q < newRoute.size(); q++){
				if (newRoute[q] > 1000) break;
				load++;
			}
			if (q == newRoute.size()-1 && newRoute[newRoute.size()-1] < 1000) {
				cout << "ERROR: #2" << endl;
				exit(1);
			}
			if (load > 4)
				break;

			// (2) do not allow dead patients on the route 
			// ????????? IS THIS A GOOD HEURISTIC?
			// Try it later

			// evaluate the route
			// if it is the best, keep it
			// If they are all worst, then forget about the patient
			newRoutes = routes;
			newRoutes[j] = newRoute;
			int newScore = computeScore(newRoutes, patients, hospitalLocations);
			if (newScore > bestScore) {
				bestRoutes = newRoutes;
				bestScore = newScore;
			}
		}
	}
	
	return bestRoutes;


}

// if path is invalid, return -1. Path is invalid if 
	// contains dead people
	// has ambulance loaded with more than 4 people at one time
float computeScoreIfValid(vector<vector<int> > routes, vector<patient> patients, vector<pair<int,int> > hospLocs) {
	int nSurvivors = 0;
	int time;
	for (int i = 0; i < routes.size(); i++) {
		vector<int> route = routes[i];
		time = 0;
		int x0,y0,xf,yf;
		int id;
		x0 = hospLocs[ route[0] - 1000 - 1  ].first;
		y0 = hospLocs[ route[0] - 1000 - 1  ].second;
		int nPatientsLoaded = 0;
		int timesToLive[4];
		for (int j = 1; j < route.size(); j++) {
			id = route[j];
			// destination is a patient
			if ( id < 1000) {
				xf = patients[ id - 1 ].x;
				yf = patients[ id - 1 ].y;
				time++;

				time += abs(xf - x0);
				time += abs(yf - y0);
	
				timesToLive[nPatientsLoaded++] = patients[ id - 1 ].t;
			}
			// destination is a hospital
			else {
				xf = hospLocs[ route[j] - 1000 - 1  ].first;
				yf = hospLocs[ route[j] - 1000 - 1  ].second;
				if (nPatientsLoaded > 0)
					time++;

				time += abs(xf - x0);
				time += abs(yf - y0);

				if (nPatientsLoaded > 4)
					return -1;

				// count living bodies
				for (int k = 0; k < nPatientsLoaded; k++) 
					if (timesToLive[k] >= time)
						nSurvivors++;
					else
						return -1.0;
				nPatientsLoaded = 0;
			}
			

			// set up for next iteration
			x0 = xf;
			y0 = yf;
		}
	}
	return (float)nSurvivors;
}



// removes dead patients
// and also adjacent hospitals
vector<vector<int> > removeDeadPatients(vector<vector<int> > routes, vector<pair<int,int> > hospLocs, vector<patient> patients) {
	vector<vector<int> > newRoutes = routes;

	
	int time;
	for (int i = 0; i < routes.size(); i++) {
		vector<int> route = routes[i];
		vector<int> newRoute = route;
		time = 0;
		int x0,y0,xf,yf;
		int id;
		x0 = hospLocs[ route[0] - 1000 - 1  ].first;
		y0 = hospLocs[ route[0] - 1000 - 1  ].second;
		int nPatientsLoaded = 0;
		int patientIDs[4];
		int timesToLive[4];
		newRoute.push_back(route[0]); // always included initial hospital
		for (int j = 1; j < route.size(); j++) {
			id = route[j];
			// destination is a patient
			if ( id < 1000) {
				xf = patients[ id - 1 ].x;
				yf = patients[ id - 1 ].y;
				time++;

				time += abs(xf - x0);
				time += abs(yf - y0);
				patientIDs[nPatientsLoaded] = id;
				timesToLive[nPatientsLoaded++] = patients[ id - 1 ].t;
			}
			// destination is a hospital
			else {
				xf = hospLocs[ route[j] - 1000 - 1  ].first;
				yf = hospLocs[ route[j] - 1000 - 1  ].second;
				if (nPatientsLoaded > 0)
					time++;

				time += abs(xf - x0);
				time += abs(yf - y0);

				// count living bodies
				for (int k = 0; k < nPatientsLoaded; k++) 
					if (timesToLive[k] >= time) {
						newRoute.push_back(patientIDs[k]);
					}
				// only included hospital if it wasn't adjacent to another hospital
				if (nPatientsLoaded > 0)
					newRoute.push_back(route[j]);
				nPatientsLoaded = 0;
			}
			newRoutes.push_back(newRoute);

			// set up for next iteration
			x0 = xf;
			y0 = yf;
		}
	}
	return newRoutes;


}


vector<int> getUnservicedPatients (vector<vector<int> >) {
	
}











































