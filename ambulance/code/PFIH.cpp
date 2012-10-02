/** Push-Forward Insertion Heuristic (PFIH) 
 * Author: Ilya Korsunsky 
 * Date: 9/27/12
 *
 * Computes routes for ambulances using some cost function
 *
 */

#include"PFIH.h"

bool comparePatients::operator() (patient& p1, patient& p2){
	return p1.lowCost > p2.lowCost;
}

// INPUT: 
	// (1) parameter for cost computation
	// (2) patient and ambulance info
	// (3) hospital positions
// OUTPUT: 
	// (1) route for each ambulance
	// (2) total number of patients saved
	// (3) cpu time for full computation
void PFIH(float weight, string inputDataFileName, string outputFileName, string hospitalLocationsFileName) {

	// input set of patients
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

//	cout << "number of patients: " <<  patients.size() << endl;

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



	in.close();

	// read in hospital locations
	ifstream hlin(hospitalLocationsFileName.c_str());
	getline(hlin, line);
	vector<pair<int,int> > hospitalLocations(5);
	int xloc, yloc;
	for (int i = 0; i < 5; i++) {
		hlin >> xloc >> yloc;
		hospitalLocations[i] = pair<int,int>(xloc, yloc);
	}
	hlin.close();

//	cout << "hospital locations " << endl;
//	for (int i = 0; i < 5; i++) 
//		cout << "\t" << hospitalLocations[i].first << "," << hospitalLocations[i].second << endl;

	// initialize costs of all the patients to each hospital
	// also store the ID of the nearest hospital (to tell the EMTs)
	pair<float,int> temp;
	for (unsigned int i = 0; i < patients.size(); i++) {
		patients[i].cost = computeCost(patients[i].x, patients[i].y, patients[i].t, weight, hospitalLocations);
		temp = min(patients[i].cost);
		patients[i].lowCost = temp.first;
		patients[i].nearestHospital = temp.second + 1000 + 1;
	}

//	cout << "begin to test routes" << endl;

	// LOOP: add patient to set of routes
	priority_queue<patient,vector<patient>,comparePatients> pq(patients.begin(), patients.end());
	vector<vector<int> > bestRoutes = routes;
	float bestScore = computeScore(routes, patients, hospitalLocations);
	for (unsigned int i = 0; i < patients.size(); i++) {

		// find next best patient (lowest cost to closest hospital?)
		newPatient = pq.top();
		pq.pop();

//		cout << "inserting patient " << newPatient.id << endl;

		// try to insert patient into all new routes
		// that is, create new possible routes and evaluate each one
		vector<vector<int> > newRoutes;
		vector<int> newRoute;
		for (int j = 0; j < routes.size(); j++) {	// consider route j
			for (int k = 0; k < routes[j].size(); k++) {		// look at position k in route j
				newRoute = routes[j];
				newRoute.insert(newRoute.begin() + k + 1, newPatient.id);


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
		routes = bestRoutes;
	}

//	cout << "Rescued " << bestScore << " patients " << endl;	
	writeRoutesToFileUnformatted(outputFileName.c_str(), routes, hospitalLocations, nambulances, patients);

}

void writeRoutesToFile(string fileName, vector<vector<int> > routes, vector<pair<int,int> > hospLocs, int * nambulances, vector<patient> patients) {
	ofstream out(fileName.c_str());

	out << "POSITION" << endl;
	for (int i = 0; i < 5; i++) 
		out << "H1," << hospLocs[i].first << "," << hospLocs[i].second << "," << nambulances[i] << endl;

	out << "ROUTINE" << endl;
	for (int i = 0; i < routes.size(); i++) {
		out << "H" <<  routes[i][0] - 1000 << "," << hospLocs[routes[i][0] - 1000 - 1].first << "," << hospLocs[routes[i][0] - 1000 - 1].second;
		for (int j = 1; j < routes[i].size(); j++) {
			int id = routes[i][j];
			if (id < 1000) 
				out << ",P" << id << "," << patients[ id - 1 ].x << "," << patients[ id - 1 ].y << "," << patients[ id - 1 ].t;
			else
				out << ",H" <<  id - 1000 << "," << hospLocs[id - 1000 - 1].first << "," << hospLocs[id - 1000 - 1].second;
		}
		out << endl;
	}

	out.close();
}


void writeRoutesToFileStd(string fileName, vector<vector<int> > routes, vector<pair<int,int> > hospLocs, int * nambulances, vector<patient> patients) {
//	ofstream out(fileName.c_str());

	cout << "POSITION" << endl;
	for (int i = 0; i < 5; i++) 
		cout << "H1," << hospLocs[i].first << "," << hospLocs[i].second << "," << nambulances[i] << endl;

	cout << "ROUTINE" << endl;
	for (int i = 0; i < routes.size(); i++) {
		cout << "H" <<  routes[i][0] - 1000 << "," << hospLocs[routes[i][0] - 1000 - 1].first << "," << hospLocs[routes[i][0] - 1000 - 1].second;
		for (int j = 1; j < routes[i].size(); j++) {
			int id = routes[i][j];
			if (id < 1000) 
				cout << ",P" << id << "," << patients[ id - 1 ].x << "," << patients[ id - 1 ].y << "," << patients[ id - 1 ].t;
			else
				cout << ",H" <<  id - 1000 << "," << hospLocs[id - 1000 - 1].first << "," << hospLocs[id - 1000 - 1].second;
		}
		cout << endl;
	}

//	out.close();
}

void writeRoutesToFileUnformatted(string fileName, vector<vector<int> > routes, vector<pair<int,int> > hospLocs, int * nambulances, vector<patient> patients) {
	ofstream out(fileName.c_str());

	for (int i = 0; i < routes.size(); i++) {
		for (int j = 0; j < routes[i].size(); j++) {
			int id = routes[i][j];
			if (id < 1000) 
				out << "P" << id << " ";
			else
				out << "H" <<  id - 1000 << " ";
		}
		out << endl;
	}

	out.close();
}


pair<float,int> min(vector<float> vec) {
	float val = vec[0];
	int ind = 0;
	for (unsigned int i = 1; i < vec.size(); i++) 
		if (vec[i] < val) {
			val = vec[i];
			ind = i;
		}

	return pair<float,int>(val,ind);
}

vector<float> computeCost(int xloc, int yloc, int ptime, float weight, vector<pair<int,int> > hospLocs){
	vector<float> res(5);
	for (int i = 0; i < 5; i++)
		res[i] = abs(xloc - hospLocs[i].first) + abs(yloc - hospLocs[i].second) + weight*ptime;

	return res;
}

float computeScore(vector<vector<int> > routes, vector<patient> patients, vector<pair<int,int> > hospLocs) {
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

				// count living bodies
				for (int k = 0; k < nPatientsLoaded; k++) 
					if (timesToLive[k] >= time)
						nSurvivors++;
				nPatientsLoaded = 0;
			}
			

			// set up for next iteration
			x0 = xf;
			y0 = yf;
		}
	}
	return (float)nSurvivors;
}


void printRoutes(vector<vector<int> > routes) {
	cout << "***** ROUTES *****" << endl;
	for (int i = 0; i < routes.size(); i++) {
		for (int j = 0; j < routes[i].size(); j++) {
			if (routes[i][j] < 1000) 
				cout << "P" << routes[i][j] << " ";
			else
				cout << "H" <<  routes[i][j] - 1000 << " ";
		}
		cout << endl;
	}
	cout << "******************" << endl;

}

void printRoute(vector<int > route) {
	for (int j = 0; j < route.size(); j++) {
		if (route[j] < 1000) 
			cout << "P" << route[j] << " ";
		else
			cout << "H" <<  route[j] - 1000 << " ";
	}
	cout << endl;

}




















