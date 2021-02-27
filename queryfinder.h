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
#include "utils/json.hpp"
#include "data.h"
#include "dictdata.h"
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

string OUT_FOLDER = "./out";
string RUN_FOLDER = OUT_FOLDER + "/runs";
string VOCAB_PATH = OUT_FOLDER+"/vocabulary.txt"; // where the vocabulary will be placed
string INDEX_PATH = OUT_FOLDER+"/index.txt"; 
string TEMP_PATH = OUT_FOLDER+"/tempfile.txt";
string INV_PATH = OUT_FOLDER+"/invfileoneline.txt";
string DIC_PATH = OUT_FOLDER+"/dictionaryoneline.txt";
string RUN_PATH = RUN_FOLDER+"/run";
int PAGES_TO_PARSE = 100;
int LIMIT = 2000000000;//1.5 gb
int LINES_BATCH_SIZE_INV = LIMIT;


// represents the term and its id
map<std::string, int> vocabMap;
// represents the dictionary
map<int,DictData> dictionaryMap;

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

        // runs the parsing for collection
        void startParsing();

        // sorts the temporary inverted file based on runs
        void sortingInvFile();

        // sorts the runs to build the inverted file - second approach from report
        bool sortingWholeFile(vector<pair<int,int>> posRuns);

        // sorts the runs to build the inverted file - second approach from report
        bool dividingRuns();

        bool sortingAll(int numRuns);

        // sorts the runs to build the inverted file - first approach from report
        bool sortingWholeFile2(vector<pair<int,int>> posRuns);

        // updates IDF on dictionary
        void updateIDF(int nMaxDocs);

        // do the parsing on the html
        static std::string cleanText(GumboNode* node);

        // pre process before token creation
        static std::vector<std::string> preProcessing(std::string content);

        // add words in the vocabulary
        void feedMap(int docNumber, std::vector<std::string> doc);

        // save the vocabulary, true if sucess
        static bool saveVocabulary();

        // save occurrences on temp file, true if sucess
        static bool saveTemp(string words);

        // saving the dictionary on file
        static bool saveDictionary();


        

};
