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


#include "dictdata.h"
#include "utils/json.hpp"
#include "utils/strtk.hpp"

using json = nlohmann::json;
using namespace std;

// vars to change
string COLLECTION_PATH = "../collection/";
string COLLECTION_NAME = "collection.jl";

string INVERTED_PATH = "../invertedindex/";
string INVERTED_NAME = "invfile.txt";
string DICT_NAME = "dictionary.txt";
string VOCAB_NAME = "vocabulary.txt";
string QUERIES_NAME = "queries.txt";
string TERM_POSITION_NAME = "termposition.txt";
string QUERIES_RESULT_NAME = "queriesresult.txt";
string DOCLINKS_NAME = "docslinks.txt";
string DOCWIJ_NAME = "docwij.txt";
std::vector<std::string> words2avoid{"de","a","o","da","no","na","e","em","qual"};

// represents the term and its id
map<std::string, int> vocabMap;
// represents the dictionary
map<int,DictData> dictionaryMap;
// doc names
map<int,string> docNames;
// doc wij terms
map<int,float> docWij;
// doc term posi
map<int,int> termPosi;

class QueryFinder
{
    public:
        // return the time
        static double elapsed ();

        bool writeDocNames();

        bool writeDocWij();

        bool writeDocPosi();

        // runs the parsing for collection
        void startParsing();

        bool preStart();

        bool startSearch();
        //load dict
        bool readDict();
        //load vocab
        bool readVocab();
        //load doc urls
        bool readDocNames();
        //load doc wij
        bool readDocWij();
        //load doc position
        bool readDocPosi();

        bool saveQuery(string query, string msg);

        bool readInvFile(map<int,int> id_terms,string query);

        bool processQuery(map<string,int> terms,string query);


        

};
