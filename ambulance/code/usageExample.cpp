/** Demonstration of usage of the PFIH function, which constructs an initial route for all ambulances
 *	Author: Ilya Korsunsky
 *  Date: 9/29/12
 * 
 *  PARAMETERS: 
 * 		timeWeight: this is the relative cost of time left to live vs. distance to nearest hospital,
 * 		inputDataFileName: this file contains input given by the Professor, including
 * 			patient information and number of ambulances per hospital
 * 		hospitalLocationsFileName: this file contains the locations of hospitals
 * 			Coordinates should be white space delimited
 * 			The file should only contain the coordinates and nothing more (i.e. no explanation of data)
 * 		outputFileName: the routes computed by PFIH will be written to this file
 */

#include "PFIH.h"

int main() {
	float timeWeight = 500;
	string inputDataFileName = "data.txt";	
	string outputFileName = "initial_routes.txt";
	string hospitalLocationsFileName = "hospitalLocations.txt";

	PFIH(timeWeight, inputDataFileName, outputFileName, hospitalLocationsFileName);

	return 0;
}
