#include "kruskal.h"
#include <map>
#include <list>
#include <stack>
#include <vector>
#include "time.h"

struct city {
	int id;
	float xcoord;
	float ycoord;
	float zcoord;
};
struct Graph {
	int nedges;
	vector<vector<int> > adj;
	float** edgeWeights;
};

list<int> swap_checking(float ** weights, list<int> ham);
list<int> shortcut(stack<int> circuit, int nVerts);
stack<int> eulerCircuit(string mstFileName, string mcmFileName, string ECFileName, int ncities, map<int,int> nodeMap);
map<int,int> constructOddDegreeGraph(string mstFileName, string edgesFileName, string OFileName, int nvert, int nMSTedges, int nGedges);
void dfs(int v, list<int> &path, vector<vector<int> > adj, int);
void getMSTCycle(string mstFileName, int, int);
Graph constructGraph(string, string, int);
float score(float **, list<int> path);
void printlist(list<int> s) {
	
	for(list<int>::iterator i = s.begin(); i != s.end(); i++)
		cout << *i << " ";
	cout << endl;
}
time_t t_start;

int main() {
	t_start = time (NULL);
	// construct graph
	string inFileName = "tsp_input.txt";
	string edgesFileName = "edges.txt";
	string mstFileName = "mstEdges.txt";
	int ncities = 1000;
	cout << "begin Construct Graph" << endl;
	Graph G = constructGraph(inFileName, edgesFileName, ncities);
	int nedges = G.nedges;

	
	// *** PART 1: MST BASED PATH ****
	// do Kruskal's algorithm 
	cout << "begin Kruskal" << endl;
	kruskal(edgesFileName, mstFileName, ncities, nedges);

	// get the cycle from the MST and score it
	getMSTCycle(mstFileName, ncities-1, ncities);

	// *** PART 2: Cristofides algorithm ***
	// (1) construct graph O of all odd-degree vertices in MST (include all edges from G)
	string OFileName = "OddGraph.txt";
	map<int,int> reverseNodeMap = constructOddDegreeGraph(mstFileName, edgesFileName, OFileName, ncities, ncities-1, nedges);

	// (2) get the min cost matching on O
	string McmFileName = "MCMEdges.txt";
	string blossomCmd = "./blossom5-v2.03.src/blossom5 -e " + OFileName + " -w " + McmFileName;
	system(blossomCmd.c_str());

	// (3) find Eulerian circuit from MST + MCM
	// combine MST + MCM
	string ECFileName = "EulerEdges.txt";
	stack<int> eulerTour = eulerCircuit(mstFileName, McmFileName, ECFileName, ncities, reverseNodeMap);

	// (4) Convert Eulerian circuit into Hamiltonian circuit by removing redundant vertices
	list<int> ham = shortcut(eulerTour, ncities);


	// (5) score the hamiltonian 
	ofstream outHam("HAMILTONIAN.TXT");
	for (list<int>::iterator it = ham.begin(); it != ham.end(); it++)
		outHam << *it << " ";
	cout << "HAM SCORE:" << score(G.edgeWeights, ham) << endl;

	// (Oyster) check if any swap can make it better
	list<int> swap_checked = swap_checking(G.edgeWeights, ham);

	// (Oyster) score the hamiltonian 
	ofstream outSwap("tsp_output.txt");
	for (list<int>::iterator it = swap_checked.begin(); it != swap_checked.end(); it++)
		outSwap << ((*it)+1) << "\n";
	outSwap << "1\n";
	cout << "SWAP SCORE:" << score(G.edgeWeights, swap_checked) << endl;
	cout << "check:" << G.edgeWeights[0][5] << endl;
	cout << "check2:" << G.edgeWeights[17][719] << endl;
	return 0;
}

list<int> swap_checking(float ** weights, list<int> ham) {
	vector<int> v_ham_o;
	list<int>::iterator li;
	for (li = ham.begin(); li != ham.end(); li ++) {
		v_ham_o.push_back(*li);
	}
	v_ham_o.push_back(0);

	vector<vector<int> > possibles;
	for (int i = 1; i < v_ham_o.size() - 2; i ++) {
		for (int j = i + 1; j < v_ham_o.size() - 1; j ++) {
			float score_ham = weights[v_ham_o[i-1]][v_ham_o[i]] + weights[v_ham_o[j]][v_ham_o[j+1]];
			float score_swap = weights[v_ham_o[i-1]][v_ham_o[j]] + weights[v_ham_o[i]][v_ham_o[j+1]];
			if (score_ham > score_swap) {
				vector<int> item;
				item.push_back(i);
				item.push_back(j);
				possibles.push_back(item);
			}
		}
	}

	list<int> best;
	for (int i = 0; i < v_ham_o.size() - 1; i ++) {
		best.push_back(v_ham_o[i]);
	}
	float best_score = score(weights, best);

cout << possibles.size() << "\n";	
	int poi = 0;
	time_t t_now = time(NULL);
	while ((t_now - t_start) < 160 && poi < possibles.size()) {
cout << t_now-t_start << ": on " << poi << " score:" << best_score <<"\n";
		vector<int> v_ham = v_ham_o;
		for (int k = 0; k*2 < possibles[poi][1]-possibles[poi][0]; k ++) {
			v_ham[possibles[poi][0]+k] += v_ham[possibles[poi][1]-k];
			v_ham[possibles[poi][1]-k] = v_ham[possibles[poi][0]+k] - v_ham[possibles[poi][1]-k];
			v_ham[possibles[poi][0]+k] -= v_ham[possibles[poi][1]-k];
		}
		
		bool have_update = true;
		while (have_update) {
			have_update = false;
			for (int i = 1; i < v_ham.size() - 2; i ++) {
				for (int j = i + 1; j < v_ham.size() - 1; j ++) {
					float score_ham = weights[v_ham[i-1]][v_ham[i]] + weights[v_ham[j]][v_ham[j+1]];
					float score_swap = weights[v_ham[i-1]][v_ham[j]] + weights[v_ham[i]][v_ham[j+1]];
					if (score_ham > score_swap) {
						have_update = true;
						// reverse between i and j
						for (int k = 0; k*2 < j-i; k ++) {
							v_ham[i+k] += v_ham[j-k];
							v_ham[j-k] = v_ham[i+k] - v_ham[j-k];
							v_ham[i+k] -= v_ham[j-k];
						}
						break;
					}
				}
			}
		}
		list<int> current;
		for (int i = 0; i < v_ham.size() - 1; i ++) {
			current.push_back(v_ham[i]);
		}
		float current_score = score(weights, current);
		if (current_score < best_score) {
			best_score = current_score;
			best = current;
		}
		poi ++;
		t_now = time(NULL);
	}
	
	return best;
}



list<int> shortcut(stack<int> circuit, int nVerts) {
	// find the hamiltonian circuit by taking
	// "triangle inequality" shortcuts in euler tour
	bool visited[nVerts];
	vector<int> hamiltonian;
	for (int i = 0; i < nVerts; i++) 
		visited[i] = false;
	while(!circuit.empty()) {
		if (hamiltonian.size() >= nVerts) 
			break;
		if (!visited[circuit.top()]) {
			hamiltonian.push_back(circuit.top());
			visited[circuit.top()] = true;;
		}
		circuit.pop();
	}
	
	// verify that circuit is hamiltonian
	// test for size
	if (hamiltonian.size() != nVerts) {
		cout << "ERROR: hamiltonian does not have enough vertices" << endl;
		exit(1);
	}
	bool included[nVerts];
	for (int i = 0; i < nVerts; i++) 
		included[i] = false;

	// test for uniqueness
	for (int i = 0; i < nVerts; i++) {
		if (included[hamiltonian[i]]) {
			cout << "ERROR: node " << hamiltonian[i] <<  " in the hamiltonian cycle are visited twice" << endl;
			exit(1);
		}
		included[hamiltonian[i]] = true;
	}
	
	list<int> result;
	for (int i = 0; i < hamiltonian.size(); i++) 
		result.push_back(hamiltonian[i]);

	return result;
}

void find_tour(int u, stack<int> &tour, vector<list<int> >adj, bool** edgeExists){
	// for each edge e = (u,v) in E
	for (list<int>::iterator it = adj[u].begin(); it != adj[u].end(); it++) {
		if (edgeExists[u][*it]) {
			// remove e from E
			edgeExists[u][*it] = false;
			edgeExists[*it][u] = false;

			// call recursively on child
			find_tour( *it , tour, adj, edgeExists);
		}
	}

	// prepend u to tour
	tour.push(u);
}


stack<int> eulerCircuit(string mstFileName, string mcmFileName, string ECFileName, int ncities, map<int,int> nodeMap) {
	//  combine MST and MCM into one graph
	ifstream inMST(mstFileName.c_str());
	ifstream inMCM(mcmFileName.c_str());
	ofstream out(ECFileName.c_str());	
	vector<pair<int,int> > combinedGraph;
	bool * edgeExists[ncities];
	for (int i = 0; i < ncities; i++) {
		edgeExists[i] = new bool[ncities];
		for (int j = 0; j < ncities; j++) 
			edgeExists[i][j] = false;
	}
	vector<list<int> > adj(ncities);
	

	// get MCM graph and convert its node indices to original
	int nVertsMCM;
	int nEdgesMCM;
	inMCM >> nVertsMCM >> nEdgesMCM;
	int v1, v2;
	float weight;
	for (int i = 0; i < nEdgesMCM; i++) {
		inMCM >> v1 >> v2;
		v1 = nodeMap[v1];
		v2 = nodeMap[v2];
		edgeExists[v1][v2] = true;
		edgeExists[v2][v1] = true;
		combinedGraph.push_back(pair<int,int>(v1,v2));
		adj[v1].push_back(v2);
		adj[v2].push_back(v1);
//		cout << "\tMCM edge: " << v1 << " " << v2 << endl;
	}


	// add the edges from the MST (omit duplicates from MCM)
	for (int i = 0; i < ncities-1; i++) { 
		inMST >> v1 >> v2 >> weight;
		if (!edgeExists[v1][v2]) {
			combinedGraph.push_back(pair<int,int>(v1,v2));
			adj[v1].push_back(v2);
			adj[v2].push_back(v1);
			edgeExists[v1][v2] = true;
			edgeExists[v2][v1] = true;
		}
	}

//	cout << "combined graph edges:" << endl;
//	for (int i = 0; i < combinedGraph.size(); i++) 
//		cout << "\t" <<   combinedGraph[i].first << " " << combinedGraph[i].second << endl;
//	cout << endl;

	// find euler path, starting at vertex 0
	stack<int> tour;
	find_tour(0, tour, adj, edgeExists);


/*
	// then print it (or return it)
	cout << "Euler Tour: ";
	while (!tour.empty()) {
		cout << tour.top() << " ";
		tour.pop();
	}
	cout << endl;
*/
	inMCM.close();
	inMST.close();

	return tour;
}




// NOTE: format for O file name needs to be appropriate to input into BlossomV
map<int,int> constructOddDegreeGraph(string mstFileName, string edgesFileName, string OFileName, int nverts, int nMSTedges, int nGedges) {
	// read in MST (and get indices of odd degree vertices)
	ifstream inMST(mstFileName.c_str());
	vector<int> degree(nverts);
	bool isOdd[nverts];
	int v1, v2;
	float weight;
	int nOddVerts = 0;
	map<int,int> nodeMap;
	map<int,int> reverseMap;
	// count degree
	for (int i = 0; i < nMSTedges; i++) { 
		inMST >> v1 >> v2 >> weight;
		degree[v1]++;
		degree[v2]++;
	}
	// keep odd degree nodes
	int nextInd = 0;
	for (int i = 0; i < nverts; i++) 
		if (degree[i] % 2 != 0) {
			isOdd[i] = true;
			nOddVerts++;
			nodeMap.insert(pair<int,int>(i,nextInd));
			reverseMap.insert(pair<int,int>(nextInd++,i));
		}
		else
			isOdd[i] = false;

	inMST.close();


	// read in original graph
	// only keep edges whose endpoints are both in O
	ifstream inG(edgesFileName.c_str());
	vector<edge> Oedges;
	edge e;
	for (int i = 0; i < nGedges; i++) {
		inG >> e.v1 >> e.v2 >> e.weight;
		if (isOdd[e.v1] && isOdd[e.v2]){
			e.v1 = nodeMap[e.v1];
			e.v2 = nodeMap[e.v2];
			Oedges.push_back(e);
		}
	}
	inG.close();


	ofstream out(OFileName.c_str());
	out << nOddVerts << " " << Oedges.size() << endl;
	for (unsigned int i = 0; i < Oedges.size(); i++)
		out << Oedges[i].v1 << " " << Oedges[i].v2 << " " << Oedges[i].weight << endl;
	out.close();

	return reverseMap;
}


Graph constructGraph(string inFileName, string outFileName, int ncities) {

	int nedges = 0;
	Graph G;
	vector<vector<int> > adj(ncities);
	float ** edgeWeights = new float*[ncities];
	for (int i = 0; i < ncities; i++)
		edgeWeights[i] = new float[ncities];


	// read in city coordinates
	ifstream infile (inFileName.c_str(), ifstream::in);
	city cities[ncities];
	for (int i = 0; i < ncities; i++)
		infile >> cities[i].id >> cities[i].xcoord >> cities[i].ycoord >> cities[i].zcoord;
	
	// write out edges of graph
	ofstream outfile(outFileName.c_str());
	double dist;
	for (int i = 0; i < ncities; i++) {
		for (int j = i+1; j < ncities; j++) {

			// compute Euclidian distance
			dist = sqrt( pow(cities[i].xcoord - cities[j].xcoord,2) 
						+ pow(cities[i].ycoord - cities[j].ycoord,2) 
						+ pow(cities[i].zcoord - cities[j].zcoord,2) );
		
			// write new edge out to file
			outfile << i << " " << j << " " << dist << endl;
			adj[i].push_back(j);
			adj[j].push_back(i);
			edgeWeights[i][j] = dist;
			edgeWeights[j][i] = dist;

			nedges++;
			

		}
	}

	infile.close();
	outfile.close();
	
	G.nedges = nedges;
	G.adj = adj;
	G.edgeWeights = edgeWeights;
	return G;
}

// get tsp path from mst
void getMSTCycle(string mstFileName, int nedges, int nverts) {
	ifstream in(mstFileName.c_str());

	int v1, v2;
	float weight;

	// get adjacency lists
	// and edge weight matrix (for score function)
	vector<vector<int> > adj(nverts);
	float ** edgeWeights = new float*[nverts];
	for (int i = 0; i < nverts; i++)
		edgeWeights[i] = new float[nverts];

	for (int i = 0; i < nedges; i++) {
		in >> v1 >> v2 >> weight;
		adj[v1].push_back(v2);
		adj[v2].push_back(v1);
		edgeWeights[v1][v2] = weight;
		edgeWeights[v2][v1] = weight;
	}
	
	// do DFS to construct path
	list<int> path;
	dfs(0, path, adj, -1);

	// print path and score
	printlist(path);
	cout << "score: " << score(edgeWeights, path) << endl;
	

	in.close();
}


void dfs(int v, list<int> &path, vector<vector<int> > adj, int parent) {
	// include v in path on the way down
	path.push_back(v);

	// explore children of v
	for (unsigned int i = 0; i < adj[v].size(); i++) {
		if (adj[v][i] != parent) {
			dfs(adj[v][i], path, adj, v);
			path.push_back(v); // include v on the way up
		}
	}

	return;
}

// scores the path
float score(float ** weights, list<int> path) {
	float result = 0;
	vector<int> v_path;
	list<int>::iterator li;
	for (li = path.begin(); li != path.end(); li ++) {
		v_path.push_back(*li);
	}
	v_path.push_back(0);
	for (int i = 0; i < v_path.size() - 1; i ++) {
		result += weights[v_path[i]][v_path[i+1]];
	}
	return result;
}





























