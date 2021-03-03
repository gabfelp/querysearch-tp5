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


class DictData
{
    public:
        int start;
        int end;
        float idf;
        DictData(int s, int e, float i) { // Constructor with parameters
            start = s;
            end = e;
            idf = i;
        }
        DictData(){
            
        }

};
