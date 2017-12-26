#include "course.h"
#include <string.h>
#include <sstream>
#include <vector>
#include <fstream>
#include <stdlib.h>

#include <iostream>

using namespace std;

void course_parse(string filename, vector<course> &c){
    ifstream infile(filename.c_str());
    string s;
    while(getline(infile, s)){
        course c_obj;
        int credit;

        string token;
        istringstream iss(s);
        
        string year;

        int i = 0;
        while(getline(iss, token, '\t')){
            if(i == 0){ // group
                istringstream iss_token(token);
                string g;
                while(getline(iss_token, g, ' ')){
                    vector<int>::iterator it = c_obj.group.end();
                    c_obj.group.insert(it, atoi(g.c_str()));
                }
            }
            if(i == 1){ // year
                year = token;
            }
            if(i == 2){ // credit
                credit = atoi(token.c_str());            
            }
            if(i == 3){ // required
                if(token == "0"){
                    vector<int>::iterator it = c_obj.year.end();
                    c_obj.year.insert(it, 0);
                }
                else{
                    istringstream iss_token(token);
                    string y;
                    while(getline(iss_token, y, ' ')){
                        vector<int>::iterator it = c_obj.year.end();
                        c_obj.year.insert(it, atoi(y.c_str()));
                    }
                }
            }
            if(i == 4){ // student
                c_obj.student = atoi(token.c_str());
            }
            if(i == 5){ // course name
                c_obj.course_name = token;
            }
            if(i == 6){ // professor name
                c_obj.prof_name = token; 
            }
            if(i == 7){ // building
                if(token == "BL"){
                    c_obj.building = BL;
                }
                if(token == "EE1"){
                    c_obj.building = EE1;
                }
                if(token == "EE2"){
                    c_obj.building = EE2;
                }
                if(token == "MD"){
                    c_obj.building = MD;
                }
                if(token == "CSIE"){
                    c_obj.building = CSIE;
                }
            }
            if(i == 8){ // professor perfer time
                istringstream iss_token(token);
                string t;
                while(getline(iss_token, t, ' ')){
                    vector<int>::iterator it = c_obj.time.end();
                    c_obj.time.insert(it, atoi(t.c_str()));
                }
            
            }

            i++;
        }
        //for(int j = 0; j < credit; j++){
            vector<course>::iterator it = c.end();
            c.insert(it, c_obj);
        //}
    }
}

void room_parse(string filename, vector<room> &r){
    ifstream infile(filename.c_str());
    string s;
    while(getline(infile, s)){
        room r_obj;

        string token;
        istringstream iss(s);
        
        string year;

        int i = 0;
        while(getline(iss, token, '\t')){
            if(i == 0){ // id
                r_obj.id = token;
            }
            if(i == 1){ // building
                if(token == "BL"){
                    r_obj.building = BL;
                }
                if(token == "EE1"){
                    r_obj.building = EE1;
                }
                if(token == "EE2"){
                    r_obj.building = EE2;
                }
                if(token == "MD"){
                    r_obj.building = MD;
                }
                if(token == "CSIE"){
                    r_obj.building = CSIE;
                }
            }
            if(i == 2){ // student
                r_obj.student = atoi(token.c_str());            
            }
            i++;
        }
        vector<room>::iterator it = r.end();
        r.insert(it, r_obj);
    }

}
