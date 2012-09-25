/** Kruskal's MST algorithm
 * 
 */

#include<DisjSets.h>

struct edge{
	int v1;
	int v2;
	float weight;
};

class compareEdges {
public:
  bool operator() (edge& e1, edge& e2);
};

vector<edge> getEdges(string filename, int);

void kruskal(string, string, int, int);

bool verifyMst(string fileName, int nverts, int);




