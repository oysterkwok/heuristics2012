/** Kruskal's MST algorithm
 * 
 */
#include "kruskal.h"
#include<algorithm>
#include <stdlib.h> 


bool compareEdges::operator() (edge &e1, edge &e2) { 
	//return e1.weight > e2.weight;
	if(e1.weight == e2.weight){
	
		int t = rand() % 2;
		if(t == 1){
			return true;
		}
		else{
			return false;
		}
	}
	else{
		return (e1.weight > e2.weight);
	}
}


vector<edge> getEdges(string filename, int nedges) {
	ifstream in(filename.c_str(), ifstream::in);
	vector<edge> e(nedges);
	for (int i = 0; i < nedges; i++)
		in >> e[i].v1 >> e[i].v2 >> e[i].weight;
	return e;
}


void kruskal(string dataFile, string outFile, int nverts,int nedges, map<int,int> local_to_global)
{
	int edgesAccepted = 0;
	DisjSets ds(nverts);
	vector<edge> edges = getEdges(dataFile, nedges);
	priority_queue<edge,vector<edge>,compareEdges> pq(edges.begin(), edges.end());
	edge e;

	ofstream out(outFile.c_str());
	while (edgesAccepted < nverts - 1) {
		e = pq.top();
		pq.pop();

		int uset = ds.find(e.v1);
		int vset = ds.find(e.v2);
		if (uset != vset) {
			out << local_to_global.find(e.v1)->second << " " << local_to_global.find(e.v2)->second << " " << e.weight << endl;
			//out << e.v1 << " " << e.v2 << " " << e.weight << endl;
			edgesAccepted++;
			ds.unionSets(uset, vset);
		}
	}
	out.close();
	
/*
	if (verifyMst(outFile, nverts, nedges) == false) {
		exit(1);
	}
*/	
}


bool verifyMst(string fileName, int nverts, int nedges) {
	ifstream in(fileName.c_str());
	int v1, v2; 
	float temp;

	bool representedVerts[nverts];
	for (int i = 0; i < nverts; i++) 
		representedVerts[i] = false;

	for (int i = 0; i < nedges; i++) {
		in >> v1 >> v2 >> temp;
		representedVerts[v1] = true;
		representedVerts[v2] = true;
	}
	in.close();

	// check for connectivity
	for (int i = 0; i < nverts; i++) 
		if (representedVerts[i] == false) {
			cout << "failed connectivity" << endl;
			return false;
		}

	return true;
}



















