#include <iostream>
#include <vector>
#include <fstream>
#include <math.h>
using namespace std;

struct point{
	int id;
	double x;
	double y;
	double z;
};

// Find s Distance between two points
double find_distance(point a, point b){
	return pow((pow((a.x-b.x),2) + pow((a.y-b.y),2) + pow((a.z-b.z),2)),0.5);
}


// Checks if old_centeroids and  new_centeroids are same or not
bool check_change(vector<point> old_centeroids, vector<point> new_centeroids){
	if(old_centeroids.size()!=new_centeroids.size()){
		cout<< "Error";
		//return;
	}
	
	for(int i=0 ; i < old_centeroids.size(); ++i){
		if((old_centeroids[i].x != new_centeroids[i].x)  &&  (old_centeroids[i].y != new_centeroids[i].y)  &&  (old_centeroids[i].z != new_centeroids[i].z))
			return 1;
	}
	// else case when all old_centeroids == new_centeroids
	return 0;
}

// returns the nodes closest to mean for each of the clusters in clusterlist
vector<point> mean_of_cluster(vector<vector<point> > clusterList, int k){
	point sum, mean,candidate;
	vector<double> distance;
	double min_distance;
	vector<point> new_centeroids;


	for (int i = 0; i < clusterList.size(); ++i)
	{	
		sum.x = 0;
		sum.y = 0;
		sum.z = 0;

		for (int j = 0; j < clusterList[i].size(); ++j)
		{
			sum.x = sum.x + clusterList[i][j].x;
			sum.y = sum.y + clusterList[i][j].y;
			sum.z = sum.z + clusterList[i][j].z;
		}

		mean.x = sum.x / k;
		mean.y = sum.y / k;
		mean.z = sum.z / k;

		// have to find the closest node to this mean
		distance.clear();
		min_distance = find_distance(mean, clusterList[i][0]);// assumption
		candidate = clusterList[i][0];
		for (int n = 0; n < clusterList[i].size(); ++n)
		{
			distance.push_back(find_distance(mean, clusterList[i][n]));
			if(distance[n] < min_distance){
				min_distance = distance[n];
				candidate.x = clusterList[i][n].x;
				candidate.y = clusterList[i][n].y;
				candidate.z = clusterList[i][n].z;
				candidate.id = clusterList[i][n].id;
			} 
		}

		new_centeroids.push_back(candidate);		
	}
	return new_centeroids;
}

void print(vector<vector<point> > clusterList)
{
	cout << "Printing" << endl;
		for (int i = 0; i < clusterList.size(); ++i){
			cout<< "\n" << i <<":";
			for (int j = 0; j < clusterList[i].size(); ++j)
			{
				cout<<clusterList[i][j].id<<" ";
			}
		}


}
// Performs K-means clustering
vector<vector<point> > kmeans(vector<point> initcluster, int k){
	
	vector<point> centeroids, old_centeroids; // vector maintaining k centeroids
  	vector<vector<point> > clusterList; // for storing the list of clusters
  	vector<double> distance; // temp vector, used for finding the min_distance
  	int centeroids_change = 1; //Flag which helps the loop to terminate 
  	double min;
  	int destination,num = 0;

  	// Intial centeroids are random
	for(int i = 0; i < k; ++i){
		centeroids.push_back(initcluster[i]);
	}
	// loop for finding the optimal clusters and centeroids
	//while(centeroids_change){
	while(num <1500){ // assumption, will consider 1250 loops as near optimal solution
		clusterList.clear(); // clearing the datastructure, after last loop.
		
		// initializing the clusterList
		vector<point> v;
		for (int i = 0; i < k; ++i)
		{
			clusterList.push_back(v);
		}
	
		for (int i = 0; i < initcluster.size(); ++i){	
			distance.clear();// will empty the distance vector for future calculations
			
			min = find_distance(initcluster[i], centeroids[0]); //assumption, will improve while running the code
			destination = 0;
		
			for (int j = 0; j < centeroids.size(); ++j){
				distance.push_back(find_distance(initcluster[i], centeroids[j]));
				if (distance[j] <= min){  
					min = distance[j];
					destination = j;
				}	
			}	
			clusterList.at(destination).push_back(initcluster[i]);						
		}
	
		num ++;
		old_centeroids = centeroids;
	
		centeroids = mean_of_cluster(clusterList, k);
		//centeroids_change = check_change(old_centeroids, centeroids);
	}

	cout<< "Number of loops iterated for finding Optimal Solution: "<< num<<"\n";
	
	print(clusterList);

	// Printing final K centeroids
	cout << "Final Centeroids:"<<"\n";
	for(int i=0; i <centeroids.size();++i){
		cout<<centeroids[i].id<<" "<<centeroids[i].x<<" "<<centeroids[i].y<<" "<<centeroids[i].z<<"\n";
	}

	return clusterList;
}


int main()
{	
	int k;
 	point node;
 	vector<point> initcluster;
 	vector<vector<point> > clusterList;

 	ifstream myfile ("input.txt");
 	if (myfile.is_open()){
    	while ( myfile.good() ){   
      		myfile >> node.id;
      		myfile >> node.x;
      		myfile >> node.y;
      		myfile >> node.z;
  
      		initcluster.push_back(node);
    	}
    	myfile.close();

    	cout<< "Enter K value: ";
    	cin>>k;
    	
    	// call for kmeans clustering
    	clusterList = kmeans(initcluster,k);
		/*
    	// printing the clusterlist
		for (int i = 0; i < clusterList.size(); ++i){
			cout<<"\n";
			for (int j = 0; j < clusterList[i].size(); ++j)
			{
				cout<<clusterList[i][j].id<<" ";
			}
		}
		*/
  	}

  	else cout << "Unable to open input file"; 

	return 0;
}
