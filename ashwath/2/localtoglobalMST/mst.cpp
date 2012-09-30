#include "kruskal.h"
#include <list>
#include "kmeans.h"
#include <sstream>
#include <map>

using namespace std;

map<int,int> local_to_global;

void dfs(int v, list<int> &path, vector<vector<int> > adj, int);
void getMSTCycle(string mstFileName, int);
int constructGraph(string, string, int);
float score(float **, list<int> path);
void findConnection(int , vector<vector<point> > , vector<int> , int , int , string );
void connecting_clusters(int , vector<vector<point> > , string , vector<int> , string );
void findGlobalMst(int, string , vector<int> , vector<string> , string );

void printlist(list<int> s) {	
	for(list<int>::iterator i = s.begin(); i != s.end(); i++)
		cout << *i << " ";
	cout << endl;
}


int main() {
	
	// construct graph
	string inFileName = "cities.txt";
	string globalMst = "globalMst.txt";
	string cluster_centeroid = "cluster_centeroid.txt";
	string edges_centeroid = "egdes_centeroid.txt";
	string mstedge_centeroid = "mstedge_centeroid.txt";	
	string connectionsfile = "connectionsfile.txt";
	int k, ncities;
 	point node;
 	vector<point> initcluster;
 	vector<vector<point> > clusterList;

 	ifstream myfile ("cities.txt");
 	if (myfile.is_open()){
    	while ( myfile.good() ){   
      		myfile >> node.id;
      		myfile >> node.x;
      		myfile >> node.y;
      		myfile >> node.z;
  
      		initcluster.push_back(node);
    	}
    	myfile.close();
    	
    	vector<int> leftover_index;
    	vector<int> leftover_record;

    	cout<< "Enter K value: ";
    	cin>>k;
		
		string s0=".txt";
    	
    	//mstFile
    	vector<string> mstFileName;
    	string s1="mstEdges";
		for (int i = 0; i < k; ++i){
			string s = static_cast<ostringstream*>( &(ostringstream() << i) )->str();
			mstFileName.push_back(s1 + s + s0);
		}
		
		//clusterFile
		vector<string> clusterFile;
    	string s2="clusterFile";
		for (int i = 0; i < k; ++i){
			string s = static_cast<ostringstream*>( &(ostringstream() << i) )->str();
			clusterFile.push_back(s2 + s + s0);
		}

		//edgesFile
		vector<string> edgesFileName;
    	string s3="edges";
		for (int i = 0; i < k; ++i){
			string s = static_cast<ostringstream*>( &(ostringstream() << i) )->str();
			edgesFileName.push_back(s3 + s + s0);
		}

    	// call for kmeans clustering
    	clusterList = kmeans(initcluster,k);
		
    	// Creates MST for each cluster in  clusterList
		for (int i = 0; i < k ; ++i){			

			if(clusterList.at(i).size() == 1) {
				leftover_index.push_back(i);
				leftover_record.push_back(clusterList[i][0].id);
				continue;
			}
			// saves the clusterlist[i] in clusterfile[i]
			ofstream outfile(clusterFile.at(i).c_str()); 
			for (int j = 0; j < clusterList.at(i).size(); ++j){
				outfile << clusterList[i][j].id << " " << clusterList[i][j].x << " " << clusterList[i][j].y << " " << clusterList[i][j].z << endl;
			}
			outfile.close();
			
			ncities = clusterList.at(i).size();
			
			
				cout << "begin Construct Graph" << endl;
				int nedges = constructGraph(clusterFile.at(i), edgesFileName.at(i), ncities);
			
				//cout<<" i: "<<i<<" NUm Of edges: "<< nedges<<endl;

				// do Kruskal's algorithm 
				cout << "begin Kruskal" << " i: "<<i<<endl;
				kruskal(edgesFileName.at(i), mstFileName.at(i), ncities, nedges, local_to_global);
			
		}

		ofstream outfile(cluster_centeroid.c_str());
		for (int i = 0; i < clusterList.at(k).size(); ++i)// for centeroids
		{	
			outfile << clusterList[k][i].id << " " << clusterList[k][i].x << " " << clusterList[k][i].y << " " << clusterList[k][i].z << endl;
		}
		outfile.close();	
		ncities = clusterList.at(k).size();
			
		cout << "begin Construct Graph" << endl;
		int nedges = constructGraph(cluster_centeroid, edges_centeroid, ncities);
			
		// do Kruskal's algorithm 
		cout << "begin Kruskal" <<" for centeroids"<<endl;
		kruskal(edges_centeroid, mstedge_centeroid, ncities, nedges, local_to_global);

		connecting_clusters(k,clusterList, mstedge_centeroid, leftover_record, connectionsfile);

		findGlobalMst(k, globalMst,leftover_index,mstFileName,connectionsfile);

		// get the cycle from the MST and score it
		getMSTCycle(globalMst, 1000);


	}

  	else cout << "Unable to open input file"; 

	return 0;

}

void findGlobalMst(int k, string globalMst, vector<int> leftover_index, vector<string> mstFileName, string connectionsfile ){
	ofstream outfile(globalMst.c_str());
  
	for (int i=0; i< k; ++i){
		int flag = 0;
		for (int j = 0; j < leftover_index.size(); ++j)
		{
			if(i == leftover_index.at(j)){ 
				flag=1;
				break;
			}
		}
		if(flag == 1) continue;

		string line;
		ifstream infile (mstFileName.at(i).c_str());
	//	outfile.open (globalMst.c_str(), ofstream::app);
  		if (infile.is_open())
  		{
    		while ( infile.good() )
    		{
      			getline (infile,line);
      			outfile << line<< endl;
    		}
    		infile.close();
    //		outfile.close();
  		}
	}

	// now for the connections	
	string line;
	ifstream infile (connectionsfile.c_str());
  	if (infile.is_open())
  	{
    	while ( infile.good() )
    	{
      		getline (infile,line);
      		outfile << line << endl;
    	}
    	infile.close();
  	}
  	outfile.close();		
}

void connecting_clusters(int k, vector<vector<point> > clusterList, string mstedge_centeroid, vector<int> leftover_record, string connectionsfile){
	//ifstream infile (mstedge_centeroid.c_str(), ifstream::in);
	int id1, id2; 
	double weight;
	double distance,min;
	int flag1 =0; 
	int flag2 =0;
	int m,n;
	point final1, final2;
	vector<int> edge1, edge2;
	ofstream outfile(connectionsfile.c_str());
	ifstream infile (mstedge_centeroid.c_str());
 	if (infile.is_open()){
    	while(infile.good()){  
    
      		infile >> id1;
      		infile >> id2;
      		infile >> weight;
      		edge1.push_back(id1);
    		edge2.push_back(id2);   		
    	}
    	infile.close();
    	
    	edge1.pop_back();
		edge2.pop_back();

		while(!edge1.empty()){
			id1 = edge1.back();
    		id2 = edge2.back();
     		for(int i=0; i < leftover_record.size(); ++i){
				if(id1 == leftover_record.at(i)) flag1=1;
				else if (id2 == leftover_record.at(i)) flag2=1;
			}

			if(flag1 == 1 && flag2 ==0){
				for (int i = 0; i < clusterList.at(k).size(); ++i)
				{
					if(id2 == clusterList[k][i].id) m = i;
				}

				for (int i = 0; i < clusterList.at(k).size(); ++i)
					{
						if(id1 == clusterList[k][i].id) n = i;
					}

				min = find_distance(clusterList[m][0],clusterList[n][0]); //assumption
				final1 = clusterList[m][0];
				final2 = clusterList[n][0];
				for (int i = 0; i < clusterList.at(m).size(); ++i)
				{
					distance = find_distance(clusterList[m][i],clusterList[n][0]);
					if(distance < min){
						min = distance;
						final1 = clusterList[m][i];
						final2 = clusterList[n][0];
					}
				}
				outfile << final1.id << " " << final2.id << " " << min << endl;
			}

			else if(flag2 == 1 && flag1== 0){
				for (int i = 0; i < clusterList.at(k).size(); ++i)
				{
					if(id1 == clusterList[k][i].id) m = i;
				}

				for (int i = 0; i < clusterList.at(k).size(); ++i)
				{
					if(id2 == clusterList[k][i].id) n = i;
				}

				min = find_distance(clusterList[m][0],clusterList[n][0]); //assumption
				final1 = clusterList[m][0];
				final2 = clusterList[n][0];
				for (int i = 0; i < clusterList.at(m).size(); ++i)
				{
					distance = find_distance(clusterList[m][i],clusterList[n][0]);
					if(distance < min){
						min = distance;
						final1 = clusterList[m][i];
						final2 = clusterList[n][0];
					}
				}
				outfile << final1.id << " " << final2.id << " " << min << endl;
			}

			else if(flag1 == 1 && flag2 == 1){
				for (int i = 0; i < clusterList.at(k).size(); ++i)
				{
					if(id2 == clusterList[k][i].id) m = i;
				}

				for (int i = 0; i < clusterList.at(k).size(); ++i)
				{
					if(id1 == clusterList[k][i].id) n = i;
				}

				min = find_distance(clusterList[m][0],clusterList[n][0]); //assumption
				final1 = clusterList[m][0];
				final2 = clusterList[n][0];
				outfile << final1.id << " " << final2.id << " " << min << endl;
			}

			else{ 
				for (int i = 0; i < clusterList.at(k).size(); ++i)
				{	
					//cout << "clusterList i:" << i << " " << clusterList[k][i].id << endl;
					if(id1 == clusterList[k][i].id){ 
						m = i;
						break;
					}
				}

				for (int i = 0; i < clusterList.at(k).size(); ++i)
				{	//cout <<"hi4" <<endl;
					if(id2 == clusterList[k][i].id) {
						n = i;
						break;
					}
				}
				min = find_distance(clusterList[m][0],clusterList[n][0]); //assumption
				final1 = clusterList[m][0];
				final2 = clusterList[n][0];
				for (int i = 0; i < clusterList.at(m).size(); ++i)
				{
					distance = find_distance(clusterList[m][i],clusterList[n][0]);
					if(distance < min){
						min = distance;
						final1 = clusterList[m][i]; 
						final2 = clusterList[n][0];/// final2 done
					}
				}
		
				//now again but id1 should be fixed.
				min = find_distance(clusterList[m][0],clusterList[n][0]); //assumption
				final1 = clusterList[m][0];
				final2 = clusterList[n][0];
				for (int i = 0; i < clusterList.at(n).size(); ++i)
				{
					distance = find_distance(clusterList[m][0],clusterList[n][i]);
					if(distance < min){
						min = distance;
						final1 = clusterList[m][0];
						final2 = clusterList[n][i];
					}
				}

				min = find_distance(final1, final2);

				outfile << final1.id << " " << final2.id << " " << min << endl;
    		}
    		edge2.pop_back();
    		edge1.pop_back();  
    		
		}
		
	}
	outfile.close();
	cout <<"connectionsfile created";
}



// returns number of edges in graph
int constructGraph(string inFileName, string outFileName, int ncities) {
	local_to_global.clear();
	int nedges = 0;
	//cout<<" inFileName.c_str(): "<<inFileName.c_str();
	// read in city coordinates
	ifstream infile (inFileName.c_str(), ifstream::in);
	point cities[ncities];
	for (int i = 0; i < ncities; i++)
		infile >> cities[i].id >> cities[i].x >> cities[i].y >> cities[i].z;
	
	// write out edges of graph
	ofstream outfile(outFileName.c_str());
	//cout<<" outFileName.c_str(): "<<outFileName.c_str();
	double dist;
	for (int i = 0; i < ncities-1; i++) {
		for (int j = i+1; j < ncities; j++) {

			// compute Euclidian distance
			dist = sqrt( pow(cities[i].x - cities[j].x,2) 
						+ pow(cities[i].y - cities[j].y,2) 
						+ pow(cities[i].z - cities[j].z,2) );
		
			// write new edge out to file
			outfile << i << " " << j << " " << dist << endl;
			local_to_global.insert ( pair<int,int>(i,cities[i].id) );
			local_to_global.insert ( pair<int,int>(j,cities[j].id) );
			nedges++;

		}
	}
	infile.close();
	outfile.close();
	
	return nedges;
}
/*
// get tsp path from mst
void getMSTCycle(string mstFileName, int nverts) {
	ifstream in(mstFileName.c_str());

	int v1, v2;
	double weight;
	bool visited[nverts];
	for (int i = 0; i < nverts; i++) visited[i] = false;

	// get adjacency lists
	// and edge weight matrix (for score function)
	vector<vector<int> > adj(nverts);
	float ** edgeWeights = new float*[nverts];
	for (int i = 0; i < nverts; i++)
		edgeWeights[i] = new float[nverts];

	string line;
	int num = 0;
	while(in.good()) {
		getline(in, line); 
		if(line.empty()) continue;

		stringstream iss(line);
      	while(iss)
      	{
        	iss >> v1;
        	iss >> v2;
        	iss >> weight;
        }
        num ++;
		//in >> v1 >> v2 >> weight;
		cout << "v1: "<<v1 << " v2: "<<v2<<" weight: "<< weight <<" num: "<< num <<endl;
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
	cout << "score: " << score(edgeWeights, path) <<" Size: "<<path.size()<< endl;
	

	in.close();
}

*/


// get tsp path from mst
void getMSTCycle(string mstFileName, int nedges, int nverts) {
	ifstream in(mstFileName.c_str());

	int v1, v2;
	float weight;
	bool visited[nverts];
	for (int i = 0; i < nverts; i++) visited[i] = false;

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
	cout << "score: " << score(edgeWeights, path) <<" Size: "<<path.size()<< endl;
	

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
	int v1, v2;
	list<int>::iterator it = path.begin();
	v1 = *it;
	it++;	

	for (; it != path.end(); it++) {
		v2 = *it;
		result += weights[v1][v2];
		v1 = v2;
	}
	return result;
}





























