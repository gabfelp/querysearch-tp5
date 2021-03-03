#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <limits>

#include <unistd.h>
#include <set>
#include <algorithm>
#include <sys/types.h> // required for stat.h
#include <sys/stat.h>
#include <map> 
#include <vector>
#include <sstream>
#include <string>
#include <cstring>
#include <set>
#include <math.h> 

using namespace std;


class Data
{
    public:
        int term;
        int document;
        int position;
        Data(int t, int d, int p) { // Constructor with parameters
            term = t;
            document = d;
            position = p;
        }
        Data(){
            
        }

};
