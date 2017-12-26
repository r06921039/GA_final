#include <string>
#include <vector>


//building constant define
#define BL  0
#define EE1 1
#define EE2 2
#define MD  3
#define CSIE 4

//student group constant define
#define NONE 0
#define Y1 1
#define Y2 2
#define Y3 3
#define Y4 4
#define Y5 5
#define Y6 6
#define Y7 7
#define Y8 8

using namespace std;

struct room{
    string id;
    int building;
    int student; // most student number
};

struct course{
    string course_name;
    vector<int> group;
    vector<int> year;
    string prof_name; // course name
    int building;// professor building
    vector<int> time; // professor name
    int student; // at least student number
};

void course_parse(string filename, vector<course> &);
void room_parse(string filename, vector<room> &);
