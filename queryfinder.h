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


#include "gumbo.h"
#include <CkString.h>
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

string OUT_FOLDER = "./out";
//string RUN_FOLDER = OUT_FOLDER + "/runs";
//string VOCAB_PATH = OUT_FOLDER+"/vocabulary.txt"; // where the vocabulary will be placed
//string INDEX_PATH = OUT_FOLDER+"/index.txt"; 
//string TEMP_PATH = OUT_FOLDER+"/tempfile.txt";
//string INV_PATH = OUT_FOLDER+"/invfileoneline.txt";
//string DIC_PATH = OUT_FOLDER+"/dictionaryoneline.txt";
//string RUN_PATH = RUN_FOLDER+"/run";
//int NUM_DOCS = 1000068;


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

int WORD_ID = 0;
double TOTAL_TIME_PARSING = 0;
double TOTAL_TIME_SORTING = 0;
int TOTAL_WORDS_COUNT = 0;
int TOTAL_DOCTERM_COUNT = 0;
int TOTAL_VOCABULARY_SIZE = 0;
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

        bool readDict();

        bool readVocab();

        bool readDocNames();

        bool readDocWij();

        bool readDocPosi();

        bool saveQuery(string query, string msg);

        bool readInvFile(map<int,int> id_terms,string query);

        bool processQuery(map<string,int> terms,string query);


        

};
