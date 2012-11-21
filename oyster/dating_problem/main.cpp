#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "dating_person.h"
#include "match_maker.h"

#define DEBUG 1

using namespace std;

int main(int argc, char *argv[])
{
    int sockfd = 0, n = 0;
    char recvBuff[1024];
    char sendBuff[1024];
    struct sockaddr_in serv_addr; 
	
    if(argc != 2) {
        printf("\n Usage: %s <port of server> \n", argv[0]);
        return 1;
    } 
	
    memset(recvBuff, '0', sizeof(recvBuff));
    memset(sendBuff, '0', sizeof(sendBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 
	
    memset(&serv_addr, '0', sizeof(serv_addr)); 
	
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1])); 
	
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\n inet_pton error occured\n");
        return 1;
    } 
	
	if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("\n Error : Connect Failed \n");
		return 1;
    } 
	
	char role;
	int n_attr = 0;
	if (DEBUG) {
		snprintf(sendBuff, sizeof(sendBuff), "Kangroo-%ld\n", time(NULL));
	}
	else {
		snprintf(sendBuff, sizeof(sendBuff), "Kangroo\n");
	}
	cout << "[PUT]" << sendBuff;
	write(sockfd, sendBuff, strlen(sendBuff));
	
	dating_person *ai_p;
	match_maker *ai_m;
	
    string readstr;
    while ( (n = (int) read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0) {
        recvBuff[n] = 0;
        for (int i = 0; i < n; i ++) {
            if (i < n && recvBuff[i] != '\n') {
                readstr += recvBuff[i];
				continue;
			}
			// print on screen all the messages from server
			cout << "[GET]" << readstr << '\n';
			if (n_attr == 0) {
				role = readstr[0];
				n_attr = atoi(readstr.substr(2).c_str());
				cout << "[START] " << role << " " << n_attr << "\n";
				if (role == 'P') {
					ai_p = new dating_person(n_attr);
				}
				if (role == 'M') {
					ai_m = new match_maker(n_attr);
				}
				readstr.clear();
			}
			else {
				if (readstr.size() == 0) {
					readstr.clear();
					continue;
				}
				if (strcmp(readstr.c_str(), "WEIGHTS") == 0) {
					vector<float> weights = ai_p->generate_weights();
					ostringstream out(ostringstream::out);
					out << "[" << weights[0];
					for (int j = 1; j < n_attr; j ++) {
						out << ", " << weights[j];
					}
					out << "]";
					snprintf(sendBuff, sizeof(sendBuff), "%s\n", out.str().c_str());
					cout << "[PUT]" << sendBuff;
					write(sockfd, sendBuff, strlen(sendBuff));
					readstr.clear();
					continue;
				}
				if (readstr[0] == '[') {
					ai_m->read_init_candidates(readstr);
					readstr.clear();
					continue;
				}
				if (strcmp(readstr.c_str(), "CANDIDATE") == 0) {
					vector<float> candidate = ai_m->generate_candidate();
					ostringstream out(ostringstream::out);
					out << "[" << candidate[0];
					for (int j = 1; j < n_attr; j ++) {
						out << ", " << candidate[j];
					}
					out << "]";
					snprintf(sendBuff, sizeof(sendBuff), "%s\n", out.str().c_str());
					cout << "[PUT]" << sendBuff;
					write(sockfd, sendBuff, strlen(sendBuff));
					readstr.clear();
					continue;
				}
				if (strcmp(readstr.c_str(), "NOISE") == 0) {
					vector<float> noise = ai_p->generate_noise();
					ostringstream out(ostringstream::out);
					out << "[" << noise[0];
					for (int j = 1; j < n_attr; j ++) {
						out << ", " << noise[j];
					}
					out << "]";
					snprintf(sendBuff, sizeof(sendBuff), "%s\n", out.str().c_str());
					cout << "[PUT]" << sendBuff;
					write(sockfd, sendBuff, strlen(sendBuff));
					readstr.clear();
					continue;
				}
				if (readstr[0] == 'G') {
					return 0;
				}
				if (readstr[0] != 'O' && readstr[0] != 'E') {
					float score = atof(readstr.c_str());
					ai_m->read_candidate_score(score);
					readstr.clear();
					continue;
				}
				readstr.clear();
			}
        }
    }
	
    if(n < 0) {
        printf("\n Read error \n");
    }
	
    return 0;
}

