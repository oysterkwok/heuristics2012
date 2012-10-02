#include "hospital_loc.h"

int main()
{	
	int k;
 	point node;
 	vector<point> initcluster;
 	vector<point> hospitals;

 	ifstream infile ("input.txt");
 	if (infile.is_open()){
 		string line;
 
  		while(infile.good()){
			getline(infile, line);		
				
				getline(infile, line, ',');
				if(line.empty()) break;
				stringstream iss(line);
      			while(iss){
        			iss >> node.x;
        		}

        		getline(infile, line, ',');
				if(line.empty()) break;
        		stringstream iss1(line);
      			while(iss1){
        			iss1 >> node.y;
        		}
      			initcluster.push_back(node);
		}		
    
    	infile.close();

    	cout<< "Enter the number of initial clusters you want(I won't recommend more than 30)";
    	cin>>k;
    	
    	// call for kmeans clustering
    	hospitals = kmeans(initcluster,k);

    	cout<<endl<<endl<<"Hospital Locations"<<endl;
    	for (int i = 0; i < hospitals.size(); ++i)
    	{
    		cout<<hospitals.at(i).x <<", "<< hospitals.at(i).y <<endl;
    	}
    	cout << endl;
	}

  	else cout << "Unable to open input file"; 

	return 0;
}
