#include "queryfinder.h"



double QueryFinder::elapsed ()
{
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return  tv.tv_sec + tv.tv_usec * 1e-6;
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

bool QueryFinder::readDict(){

  cout << "Reading dictionary" << "\n";
  string dictItem;
  std::vector<std::string> ve;
  try{
    std::ifstream dict;
    dict.open(INVERTED_PATH+DICT_NAME);
    if(dict.fail()){
      cout <<" Some errors reading queries\n" <<endl;
      return false;
    }

    while(getline(dict,dictItem)){
      std::istringstream iss(dictItem);
      int i = 0;
      int currTerm,start,end;
      float idf;
      string str;
      for(std::string ret; iss >> ret; ){
        if(i == 0){
          currTerm = atoi(ret.c_str());
        }else{

          //str = ret;
          
          strtk::parse(ret,",",ve);
          start = atoi(ve[0].c_str());
          end = atoi(ve[1].c_str());
          idf = stof(ve[2]);
          ve.clear();
        }
        
        i++;
      }

      DictData *dd = new DictData(start,end,idf);

      dictionaryMap.insert(make_pair(currTerm,*dd));

    }

  
  }catch (int e){
      std::cout << "Error on reading" << endl;
      return false;
  }

  return true;

}

bool QueryFinder::writeDocNames(){
  cout << "writing doc links" << "\n";
  ifstream file(COLLECTION_PATH + COLLECTION_NAME,ios::binary);
  ofstream doclinks(INVERTED_PATH+DOCLINKS_NAME);
  string jsontxt;

  if(file.fail()){
      cout <<" Some errors reading docs\n" <<endl;
      return false;
  }
  if(doclinks.fail()){
    cout <<" Some errors on output file\n" <<endl;
    return false;
  }

  int docNum = 0;
  while(getline(file,jsontxt)){
    //std::string contents;
    

    auto cont = json::parse(jsontxt);

    string curr_url = cont["url"];
    //string curr_html_cont = cont["html_content"];
    //docNames.insert(make_pair(docNum,curr_url));
    doclinks << docNum << " " << curr_url << "\n";

    docNum++;
  }

  //cout << docNum <<"\n";
  //cout << docNames[2] <<"\n";

}

bool QueryFinder::readDocNames(){
  cout << "Reading doc links" << "\n";
  ifstream file(INVERTED_PATH + DOCLINKS_NAME,ios::binary);
  string line,url;
  std::vector<std::string> ve;
  int num;

  if(file.fail()){
      cout <<" Some errors reading docs\n" <<endl;
      return false;
    }
  int docNum = 0;
  while(getline(file,line)){
    //std::string contents;
    strtk::parse(line," ",ve);
    num = atoi(ve[0].c_str());
    url = ve[1];

    docNames.insert(make_pair(num,url));
    ve.clear();
    //cout << line<<"\n";
  }

  //cout << docNames.size() <<"\n";
  //cout << docNames[2] <<"\n";

}

bool QueryFinder::readVocab(){
  cout << "Reading Vocabulary" << "\n";
  ifstream file(INVERTED_PATH + VOCAB_NAME,ios::binary);
  string line;
  int num;
  string term;
  std::vector<std::string> ve;
  if(file.fail()){
      cout <<" Some errors reading vocab\n" <<endl;
      return false;
  }

  while(getline(file,line)){
    //std::string contents;
    strtk::parse(line," ",ve);
    num = atoi(ve[0].c_str());
    term = ve[1];
    vocabMap.insert(make_pair(term,num));
    ve.clear();

  }


}

bool QueryFinder::preStart(){
  double t0 = elapsed();

  cout << "Loading some data for memory...\n";

  readDict();
  readVocab();
  readDocNames();

  double t1 = elapsed();
  std::cout << "Total prestart time : "<< t1-t0 << "s" << endl << endl;

  return true;
}



bool QueryFinder::startSearch(){
  cout << "Starting search for queries on "+QUERIES_NAME+"\n";
  double t0 = elapsed();
  int numQueries = 0; 
  string query;
  try{

    std::ifstream queries;
    queries.open(QUERIES_NAME);
    if(queries.fail()){
      cout <<" Some errors reading queries\n" <<endl;
      return false;
    }
    // saves index and vocabulary
    
    while(getline(queries,query)){

      std::vector<std::string> terms; 
      //std::istringstream iss(query); 
      strtk::parse(query," ",terms);
      
      cout << "query: "<< query << endl;
      for(int q = 0; q < terms.size();q++){
        cout << terms[q] <<endl;
      }
      processQuery(terms);

      numQueries++;
      terms.clear();
    }

  }catch (int e){
      std::cout << "Error on saving" << endl;
      return false;
  }

  double t1 = elapsed();
  std::cout << "Total number of queries: "<< numQueries << endl;
  std::cout << "Total searching time : "<< t1-t0 << "s" << endl << endl;

  return true;


}

bool QueryFinder::processQuery(vector<string> terms){
  return true;
}

int main(int argc, char **argv){
    QueryFinder p;

    //p.preStart();
    //p.readDocNames();
    
    p.startSearch();


    



    return 0;
}
