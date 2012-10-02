
// Function definitions are in same order as declared in hospital_loc.h(alphabetically)

#include "hospital_loc.h"

bool sortfunction (vector<point> v1 , vector<point> v2) { 
		return (v1.size()>v2.size()); 
	}


// Checks if old_centeroids and  new_centeroids are same or not, condition for kmeans loop termination
bool check_change(vector<point> old_centeroids, vector<point> new_centeroids){
	if(old_centeroids.size()!=new_centeroids.size()){
//		cout<< "Error";
		//return;
	}
	
	for(int i=0 ; i < old_centeroids.size(); ++i){
		if((old_centeroids[i].x != new_centeroids[i].x) || (old_centeroids[i].y != new_centeroids[i].y))
			return 1;
	}
	// else case when all old_centeroids == new_centeroids
	return 0;
}



// Find s Distance between two points
float find_distance(point a, point b){
	return pow(pow((a.x-b.x),2) + pow((a.y-b.y),2),0.5);
}




// Performs K-means clustering and return the sorted vector of clusters 
vector<point>  kmeans(vector<point> initcluster, int k){		

	vector<point> centeroids, old_centeroids; // vector maintaining k centeroids
  	vector<vector<point> > clusterList; // for storing the list of clusters
  	float distance1; // temp vector, used for finding the min_distance
  	int centeroids_change = 1; //Flag which helps the loop to terminate 
  	float min;
  	int destination,num = 0;

  	// Intial centeroids are random
	for(int i = 0; i < k; ++i){
		centeroids.push_back(initcluster[i]);
	}
	// loop for finding the optimal clusters and centeroids
	while(centeroids_change){
		clusterList.clear(); // clearing the datastructure, after last loop.
		
		// initializing the clusterList
		vector<point> v;
		for (int i = 0; i < k; ++i)
		{
			clusterList.push_back(v);
		}
	
		for (int i = 0; i < initcluster.size()-1; ++i){	
			
			min = find_distance(initcluster[i], centeroids[0]); //assumption, will improve while running the code
			destination = 0;
		
			for (int j = 0; j < centeroids.size(); ++j){
				distance1 = find_distance(initcluster[i], centeroids[j]);
				if (distance1 <= min){  
					min = distance1;
					destination = j;
				}	
			}	
			clusterList.at(destination).push_back(initcluster[i]);						
		}
	
		num ++;
		old_centeroids = centeroids;
	
		centeroids = mean_of_cluster(clusterList);
		centeroids_change = check_change(old_centeroids, centeroids);
	}

//	cout<< "Number of loops iterated for finding Optimal Solution: "<< num<<"\n";

/*
	// Printing final K centeroids
	cout << "Initial Centeroids:"<<"\n";
	for(int i=0; i <centeroids.size();++i){
		cout<<centeroids[i].x<<", "<<centeroids[i].y<<"; "<<"\n";
	}
*/
	// merging clusters which overlap
	vector<vector<point> > updatedClusterList = unionClusters(clusterList, centeroids);

	// sorting the cluster List
	sort (updatedClusterList.begin(), updatedClusterList.end(), sortfunction);

//	cout<<endl<<"Sorted Clusters";
//	print(updatedClusterList);

	updatedClusterList.erase(updatedClusterList.begin()+5, updatedClusterList.end());

//    cout<<endl<<endl<<"Final 5 Clusters"; 
//    print(updatedClusterList);
	
	centeroids = mean_of_cluster(updatedClusterList);

	return  centeroids;
}




// returns the nodes closest to mean for each of the clusters in clusterlist
vector<point> mean_of_cluster(vector<vector<point> > clusterList){
	point sum, mean,candidate;
	float distance;
	float min_distance;
	vector<point> new_centeroids;


	for (int i = 0; i < clusterList.size(); ++i){	
		sum.x = 0;
		sum.y = 0;

		for (int j = 0; j < clusterList.at(i).size(); ++j){
			sum.x = sum.x + clusterList[i][j].x;
			sum.y = sum.y + clusterList[i][j].y;
		}

		mean.x = sum.x / clusterList.at(i).size();
		mean.y = sum.y / clusterList.at(i).size();

		// have to find the closest node to this mean
		min_distance = find_distance(mean, clusterList[i][0]);// assumption
		candidate = clusterList[i][0];
		for (int n = 0; n < clusterList.at(i).size(); ++n)
		{
			distance = find_distance(mean, clusterList[i][n]);
			if(distance <= min_distance){
				min_distance = distance;
				candidate = clusterList[i][n];
			} 
		}

		new_centeroids.push_back(candidate);		
	}
	return new_centeroids;
}



void print(vector<vector<point> > clusterList)
{
		for (int i = 0; i < clusterList.size(); ++i){
			cout<<endl<<"Cluster Number " << i+1 <<": ";
			for (int j = 0; j < clusterList[i].size(); ++j)
			{
				cout<<"("<<clusterList[i][j].x<<","<<clusterList[i][j].y<<") ";
			}
		}
}


// Merge clusters which are close to each other
vector<vector<point> > unionClusters(vector<vector<point> > clusterList, vector<point> centeroids) {
	
	vector<double> radius;
	double centeroid_dist;

	for (int i = 0; i < centeroids.size()-1; ++i){	
		for (int j = i+1; j < centeroids.size(); ++j){ 	
			centeroid_dist = find_distance(centeroids.at(i), centeroids.at(j));			
			radius = updateradius(clusterList, centeroids); // finding radius of each cluster and storing in radius vector
			
			if(centeroids.size()<=5) continue; // will not merge if total num of clusters have reduced to 5
			
			else if(centeroid_dist < (radius.at(i) + radius.at(j)/1.5)){ // merging of clusters 		
					// merging the two clusters
					for(int n=0; n < clusterList.at(j).size(); ++n){
						clusterList.at(i).push_back(clusterList[j][n]);
					}
					// clean up
					clusterList.erase(clusterList.begin()+j);// erasing the extra cluster
					centeroids.erase(centeroids.begin()+j);// erasing the extra centroid
					// updating the centroids
					centeroids = mean_of_cluster(clusterList);
					break;
			}
			
		}
	}

	return clusterList;
}



//finding radius of each cluster and storing in radius vector
vector<double> updateradius(vector<vector<point> > clusterList, vector<point> centeroids ){
		double max, radius_dist;
		point p1;
		vector<point> chk;
		vector<double> radius;

		for ( int i = 0; i < clusterList.size(); ++i)
		{	//cout<<"hello2.5"<<endl;
			max = find_distance(centeroids.at(i), clusterList[i][0]); // assumption
			p1 = clusterList[i][0];
			for (int j = 0; j < clusterList.at(i).size(); ++j)
			{ //cout<<"hello2.75"<<endl;
				radius_dist = find_distance(centeroids.at(i),clusterList[i][j]);
				if(radius_dist > max){
					max = radius_dist;
					p1 = clusterList[i][j];
				}
			}
			chk.push_back(p1);
			radius.push_back(max);
		}

		return radius;
	}	

