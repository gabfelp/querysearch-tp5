#include "queryfinder.h"



double QueryFinder::elapsed ()
{
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return  tv.tv_sec + tv.tv_usec * 1e-6;
}

// from low to high numbers
bool comp2(const pair<string,int> &a,const pair<string,int> &b)
{
  return a.second < b.second;
}

//second method
bool compSortR(const pair<vector<int>,int> &a,const pair<vector<int>,int> &b)
{
  return a.first.at(0) > b.first.at(0);
}


bool compData(const Data &a, const Data &b)
{
  return a.term < b.term;
}

// for navigate on file's lines
std::fstream& GotoLine(std::fstream& file, unsigned int num){
    file.clear();
    file.seekg(0,std::ios::beg);
    string dummy;
    for(int i = 0; i < num; i++){
        getline(file,dummy);//file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }
    return file;
}

std::fstream& GotoLine2(std::fstream& file, int place){
    file.seekg(place);
    return file;
}

std::string QueryFinder::cleanText(GumboNode* node) 
{
  if (node->type == GUMBO_NODE_TEXT) {
    return std::string(node->v.text.text);
  } else if (node->type == GUMBO_NODE_ELEMENT &&
              node->v.element.tag != GUMBO_TAG_SCRIPT &&
              node->v.element.tag != GUMBO_TAG_STYLE) {
    std::string contents = "";
    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
      const std::string text = cleanText((GumboNode*) children->data[i]);
      if (i != 0 && !text.empty()) {
        contents.append(" ");
      }
      contents.append(text);
    }
    return contents;
  } else {
    return "";
  }
}

std::vector<std::string> QueryFinder::preProcessing(std::string content){
  CkString obj;
  
  obj.appendUtf8(content.c_str());
  // eliminate line break
  obj.eliminateChar('\n',0);
  // put everything to lower case
  obj.toLowerCase();
  std::string ret = obj.getStringUtf8();
  
  // remove punctuation
  //std::replace_if(ret.begin() , ret.end() , [] (const char& c) { return std::ispunct(c) ;},' ');

  // chars to remove
  char chars_to_remove[] = "\“\”|'?#.,/!:)(\";�*<>";
  for(int j = 0; j < strlen(chars_to_remove);j++){
    ret.erase(std::remove(ret.begin(), ret.end(), chars_to_remove[j]), ret.end());
  }
  
  // for remove separated -
  int idx = 0;
  while(true){
    idx = ret.find("- ",idx);
    if (idx == std::string::npos) break;
    ret.replace(idx,1," ");
    idx+=1;
  }
  
  // put terms into array
  std::vector<std::string> result; 
  std::istringstream iss(ret); 
  for(std::string ret; iss >> ret; ) 
    result.push_back(ret); 

  return result;

}

void QueryFinder::feedMap(int docNumber,std::vector<std::string> doc){
  int wordCount = 0;
  int curr_word = -1;
  string words = "";
  for(string re : doc){

    if (vocabMap.find(re) == vocabMap.end() ) {
      //first occurrence of word
      vocabMap.insert(pair<std::string,int>(re,WORD_ID));
      curr_word = WORD_ID;

      WORD_ID++;

    }else{
      //word found
      curr_word = vocabMap[re];

    }
    words+=to_string(curr_word)+" "+to_string(docNumber)+" "+to_string(wordCount)+"\n";
    //saveTemp(curr_word,docNumber,wordCount);

    wordCount++;
  }

  saveTemp(words);
  TOTAL_WORDS_COUNT+= wordCount;
}

bool QueryFinder::saveTemp(string words){
  // create out folder
  int nError = 0;
  #if defined(_WIN32)
      nError = _mkdir(OUT_FOLDER.c_str()); // can be used on Windows
  #else 
      nError = mkdir(OUT_FOLDER.c_str(),0777); // can be used on non-Windows
  #endif

  try{
    std::ofstream outTemp;
    outTemp.open(TEMP_PATH,std::ofstream::app | std::ofstream::binary);
    
    if(outTemp.fail()){
      cout << "Some errors creating the files" <<endl;
      return false;
    }
    // saves on temp file
    outTemp << words;
    outTemp.close();

  }catch (int e){
      std::cout << "Error on saving" << endl;
      return false;
  }

  return true;

}

bool QueryFinder::saveVocabulary(){
  // create out folder
  int nError = 0;
  #if defined(_WIN32)
      nError = _mkdir(OUT_FOLDER.c_str()); // can be used on Windows
  #else 
      nError = mkdir(OUT_FOLDER.c_str(),0777); // can be used on non-Windows
  #endif

  try{

    // sorting for save
    vector<pair<string,int>> sortedTerms;
    for(auto iter = vocabMap.begin(); iter != vocabMap.end(); iter++){
      sortedTerms.push_back(make_pair(iter->first,iter->second));
    }
    std::sort(sortedTerms.begin(), sortedTerms.end(),comp2);

    std::ofstream outVocab;
    outVocab.open(VOCAB_PATH,std::ofstream::trunc);
    if(outVocab.fail()){
      cout <<" Some errors creating the files\n" <<endl;
      return false;
    }
    // saves index and vocabulary
    for(auto iter = sortedTerms.begin(); iter != sortedTerms.end(); iter++){
      outVocab << iter->second << " " << iter->first << endl;
    }
    outVocab.close();

  }catch (int e){
      std::cout << "Error on saving" << endl;
      return false;
  }

  return true;

}
void QueryFinder::startParsing(){

    double t0 = elapsed();
    int COLLECTION_SIZE = 0;
    ifstream file(COLLECTION_PATH + COLLECTION_NAME,ios::binary);
    

    for(int i = 0; i < PAGES_TO_PARSE; i ++){
      //std::string contents;
      if((i%10000) == 0){
        //cout << "Already Parsed "<< i << " pages"<< endl;
      }
      std::string jsontxt;
      getline(file,jsontxt);
      

      if(!file){
        cout << "File " << i <<" not found, stopping."<<endl;
        PAGES_TO_PARSE = i;
        exit(0);
      }

      //cout << jsontxt << endl;

      //json cont = jsontxt_json
      auto cont = json::parse(jsontxt);

      string curr_url = cont["url"];
      string curr_html_cont = cont["html_content"];

      //cout << curr_url << endl;
      //cout << curr_html_cont << endl;

      COLLECTION_SIZE += curr_html_cont.size();//file.tellg();
      /*
        file.seekg(0,std::ios::end);
        contents.resize(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(&contents[0], contents.size());
        file.close();
      */

      
      GumboOutput* out = gumbo_parse(curr_html_cont.c_str());
      std::string cleanWebsite = cleanText(out->root);
      gumbo_destroy_output(&kGumboDefaultOptions, out);
      
      std::vector<std::string> preProcessedDoc = preProcessing(cleanWebsite);
      feedMap(i, preProcessedDoc);
      
    }


    


    double t1 = elapsed();

    TOTAL_TIME_PARSING = t1-t0;
    float col_size_kb = COLLECTION_SIZE/1000.0;


    // saving vocabulary
    saveVocabulary();

    TOTAL_VOCABULARY_SIZE = vocabMap.size();
    
    // printing results
    cout << "Collection Number of Pages : " << PAGES_TO_PARSE << endl;
    std::cout << "Size of the vocabulary (number of distinct terms) : " << TOTAL_VOCABULARY_SIZE << endl;
    std::cout << "Total terms count : "  << TOTAL_WORDS_COUNT << endl;
    std::cout << "Average time for parsing each page : "<< TOTAL_TIME_PARSING/PAGES_TO_PARSE << "s" << endl;
    std::cout << "Total parsing time : "<< TOTAL_TIME_PARSING << "s" << endl << endl;

    // already saved vocabulary map on File
    vocabMap.clear();
}

Data transformInput(string input){
  Data vecInt;
  std::istringstream iss(input); 
  //std::vector<std::string> ve;
  //strtk::parse(input," ",ve);
  int t = 0;
  for(std::string inp; iss >> inp; ){
  //for(int q = 0; q < ve.size();q++){
    if (t == 0){
      vecInt.term = atoi(inp.c_str());
    }
    if (t == 1){
      vecInt.document = atoi(inp.c_str());
    }
    if (t == 2){
      vecInt.position = atoi(inp.c_str());
    }
    t++;
    
  }

  return vecInt;
}

bool saveFile(vector<Data> lines, ofstream& run){
  string words = "";
  for(auto i : lines){
    words += to_string(i.term)+" "+to_string(i.document)+" "+to_string(i.position)+"\n";
  }

  run << words;

  run.close();


}

bool QueryFinder::dividingRuns(){
  double t0 = elapsed();

  int nError = 0;
  #if defined(_WIN32)
      nError = _mkdir(RUN_FOLDER.c_str()); // can be used on Windows
  #else 
      nError = mkdir(RUN_FOLDER.c_str(),0777); // can be used on non-Windows
  #endif


  string input;
  int size = 0;
  int line = 0;
  int runnum = 0;
  string words = "";

  vector<Data> lines;

  std::fstream file;
  file.open(TEMP_PATH, ios_base::in | ios_base::out | ios_base::binary | ios_base::ate);
  
  GotoLine(file, 0);

  while(true){
    getline(file, input);

    if(file.fail()){

      sort(lines.begin(),lines.end(),compData);

      std::ofstream run;
      run.open(RUN_PATH+to_string(runnum));
      if(run.fail()){
        cout <<"Error creating run" << endl;
      }
      saveFile(lines,run);
      
      runnum++;
      size = 0;
      lines.clear();

      break;
    }
    //cout << input;
    lines.push_back(transformInput(input));
    size += input.size();

    if(size > LIMIT){

      sort(lines.begin(),lines.end(),compData);

      std::ofstream run;
      run.open(RUN_PATH+to_string(runnum));
      if(run.fail()){
        cout <<"Error creating run" << endl;
      }
      saveFile(lines,run);

      runnum++;
      size = 0;
      lines.clear();
    }
  }
  double t1 = elapsed();

  cout << "Time for semi order :"<<t1-t0<<"s"<<endl<<endl;
  
  file.close();

  sortingAll(runnum);


}

bool QueryFinder::sortingAll(int numRuns){
  double t0 = elapsed();
  //cout << "Sorting whole file" <<endl;
  int alreadyFinish = 0;
  vector<Data> term_run;

  string input;
  Data vecInt;
  vector<pair<int,fstream>> runs;
  vector<bool> ended;
  for(int i = 0; i < numRuns; i++){
    std::fstream run;
    run.open(RUN_PATH+to_string(i),ios_base::in);
    runs.push_back(make_pair(0,std::move(run)));
    ended.push_back(false);
  }
  
  std::ofstream invfile;
  invfile.open(INV_PATH, ios_base::app);
  
 
  std::set<int> termDocs;
  int currTerm = 0, startRange = 1, currLine = 0;
  int maxNDocs = 0;
  string write = "";
  int ini,fim;
  while((alreadyFinish != numRuns)){
    bool keepTerm = false;

    for(int k = 0; k < numRuns; k++){
      if(ended[k]){
        continue;
      }
      try{

        ini = runs.at(k).first;

        GotoLine2(runs.at(k).second,ini);
      
          
        while(true){
          getline(runs.at(k).second, input);
          if(runs.at(k).second.fail()){
            if(!ended[k]){
              alreadyFinish++;
              ended[k] = true;
            }
            break;//cabou
          }

          vecInt = transformInput(input);

          if(vecInt.term != currTerm || (term_run.size() > LINES_BATCH_SIZE_INV)){
            if(term_run.size() > LINES_BATCH_SIZE_INV){
              cout << "CHEGOU AQUI TÁ !!!\n"; 
              keepTerm = true;
            }
            break;
          }else{
            term_run.push_back(vecInt);
            ini = runs.at(k).second.tellg();//get line 
          }
        }

          
        runs.at(k).first = ini;
        
      }catch(int e){
        cout << "Error on run sorting" << endl;
      }
    }

    int size_ter = term_run.size();
    write.clear();
    
    write+=to_string(currTerm);
    for(int j = 0; j < size_ter ; j++){

      Data current = term_run.back();
      termDocs.insert(current.document);
      term_run.pop_back();
      //printing on invfile
      currLine++;
      write+= " "+to_string(current.document)+" "+to_string(current.position);
      //invfile << current.first.at(0) << " " << current.first.at(1)  << " " << current.first.at(2) << endl;
      
    }
    write+="\n";
    invfile << write;//maybe change it 
    
    if(keepTerm == false){
      int nDocs = termDocs.size();
      TOTAL_DOCTERM_COUNT += nDocs;
      // higher frequency found
      if(maxNDocs < nDocs){
        maxNDocs = nDocs;
      }
      DictData *dd = new DictData(startRange,currLine,nDocs);

      dictionaryMap.insert(make_pair(currTerm,*dd));

      startRange = currLine+1;
      termDocs.clear();

      currTerm++;
    }
  }


  for(int i = 0; i < numRuns; i++){
    runs[i].second.close();
  }
  runs.clear();
  invfile.close();

  updateIDF(maxNDocs);




  double t1 = elapsed();
  TOTAL_TIME_SORTING = t1-t0;

  // saving the dictionary
  saveDictionary();
  
  std::cout << "Average size of each inverted list : " << TOTAL_WORDS_COUNT/((float)TOTAL_VOCABULARY_SIZE) << endl;
  std::cout << "Average number of documents for each term : "<< TOTAL_DOCTERM_COUNT/((float)TOTAL_VOCABULARY_SIZE) <<endl;
  std::cout << "Defined number of lines (from inverted file) to be sorted each time (must fit on RAM) : "<< LINES_BATCH_SIZE_INV  << endl;
  std::cout << "Total time for sorting the inverted file : "<< TOTAL_TIME_SORTING << "s" << endl;

}

void QueryFinder::updateIDF(int maxNDocs){
  for(auto iter = dictionaryMap.begin(); iter != dictionaryMap.end(); iter++){
    iter->second.idf = log(maxNDocs/(iter->second.idf));
  }
}

bool QueryFinder::saveDictionary(){
  // create out folder
  int nError = 0;
  #if defined(_WIN32)
      nError = _mkdir(OUT_FOLDER.c_str()); // can be used on Windows
  #else 
      nError = mkdir(OUT_FOLDER.c_str(),0777); // can be used on non-Windows
  #endif

  try{

    std::ofstream outDict;
    outDict.open(DIC_PATH,std::ofstream::trunc);
    if(outDict.fail()){
      cout <<" Some errors creating the dictionary file\n" <<endl;
      return false;
    }
    // saves index and vocabulary
    for(auto iter = dictionaryMap.begin(); iter != dictionaryMap.end(); iter++){
      outDict << iter->first << " " << iter->second.start << "," << iter->second.end << "," << iter->second.idf <<"\n";
    }
    outDict.close();

  }catch (int e){
      std::cout << "Error on saving" << endl;
      return false;
  }

  return true;

  
  //cout <<dictionaryMap.size();

}


int main(int argc, char **argv){
    QueryFinder p;

    //p.startParsing();

    TOTAL_WORDS_COUNT = 778345670;
    TOTAL_VOCABULARY_SIZE = 3382552;
    
    //p.dividingRuns();
    p.sortingAll(12);


    //std::cout << "Total time : "<< TOTAL_TIME_PARSING + TOTAL_TIME_SORTING << "s" << endl;



    return 0;
}
