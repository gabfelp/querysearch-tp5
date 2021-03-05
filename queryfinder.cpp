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
        //file.ignore('\n');
        getline(file,dummy);//file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }
    return file;
}

std::fstream& GotoLine2(std::fstream& file, unsigned int place){
    file.seekg(place);
    return file;
}

bool QueryFinder::readDict(){

  cout << "Loading dictionary" << "\n";
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

bool QueryFinder::saveQuery(string query, string msg){
  ofstream file(QUERIES_RESULT_NAME,std::ofstream::app);

  if(file.fail()){
      cout <<" Some errors saving results\n" <<endl;
      return false;
  }

  file << "query: " << query <<"\n";
  file << msg;

  file.close();

  //cout << docNum <<"\n";
  //cout << docNames[2] <<"\n";

}


bool QueryFinder::writeDocWij(){
  vector<float> docs_wij(1000068);
  cout << "writing doc Wij" << "\n";
  ifstream file(INVERTED_PATH + INVERTED_NAME,ios::binary);
  ofstream docwij(INVERTED_PATH+DOCWIJ_NAME);
  string line;
  float tf,idft;
  std::vector<std::string> ve;
  int doc, posi, numTerm;
  std::map<int,int> doc_qtd;

  if(file.fail()){
      cout <<" Some errors reading docs\n" <<endl;
      return false;
  }
  if(docwij.fail()){
    cout <<" Some errors on output file\n" <<endl;
    return false;
  }

  int docNum = 0;
  int currTerm = 0;

  while(getline(file, line)){

    strtk::parse(line," ",ve);
    line = "";
    //numTerm = atoi(ve[0].c_str());
    //cout << line <<"\n";
    
    for(int i = 1; i < ve.size(); i+= 2){
      doc = atoi(ve[i].c_str());
      //posi = atoi(ve[i+1].c_str());

      if(doc_qtd.find(doc)!= doc_qtd.end()){
        //found doc
        doc_qtd[doc]+=1;

      }else{
        //not found doc
        doc_qtd.insert(make_pair(doc,1));

      }

    }
    
    ve.clear();
    
    //ended for some term
    //cout << "ARRIIIIIIIIIIIIIIVED";
    idft = dictionaryMap[currTerm].idf;
    for(auto item = doc_qtd.begin(); item != doc_qtd.end(); item++){
      tf = (1 + log(item->second)); // tf

      docs_wij[item->first] += pow(idft * tf, 2.0);
    }

    doc_qtd.clear();

    currTerm++;
    cout << currTerm <<"\n";

  }
  file.close();

  for(int i = 0; i < docs_wij.size(); i++){
    docwij << i << " " << sqrt(docs_wij[i]) << "\n";
  }


  docwij.close();
  //cout << docNum <<"\n";
  //cout << docNames[2] <<"\n";

}

bool QueryFinder::writeDocPosi(){
  cout << "writing doc term position" << "\n";
  ifstream file(INVERTED_PATH + INVERTED_NAME,ios::binary);
  ofstream termposi(INVERTED_PATH+TERM_POSITION_NAME);
  string line;
  std::vector<std::string> ve;

  if(file.fail()){
      cout <<" Some errors reading docs\n" <<endl;
      return false;
  }
  if(termposi.fail()){
    cout <<" Some errors on output file\n" <<endl;
    return false;
  }

  int docNum = 0;
  int currTerm = 0;
  int posi = file.tellg();

  termposi << currTerm << " " << posi << "\n";
  while(getline(file, line)){
    currTerm++;
    termposi << currTerm << " " << file.tellg() << "\n";

    cout << currTerm << "\n";
    
  }
  file.close();
  termposi.close();
  //cout << docNum <<"\n";
  //cout << docNames[2] <<"\n";

}

bool QueryFinder::readDocNames(){
  cout << "Loading doc urls" << "\n";
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

bool QueryFinder::readDocWij(){
  cout << "Loading doc wij" << "\n";
  ifstream file(INVERTED_PATH + DOCWIJ_NAME,ios::binary);
  string line;
  float wij;
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
    wij = stof(ve[1]);

    docWij.insert(make_pair(num,wij));
    ve.clear();
    //cout << line<<"\n";
  }

  //cout << docNames.size() <<"\n";
  //cout << docNames[2] <<"\n";

}

bool QueryFinder::readDocPosi(){
  cout << "Loading doc term position" << "\n";
  ifstream file(INVERTED_PATH + TERM_POSITION_NAME,ios::binary);
  string line;
  std::vector<std::string> ve;
  int term;
  int posi;

  if(file.fail()){
      cout <<" Some errors reading docs\n" <<endl;
      return false;
  }
  int docNum = 0;
  while(getline(file,line)){
    //std::string contents;
    strtk::parse(line," ",ve);
    term = atoi(ve[0].c_str());
    posi = atoi(ve[1].c_str());

    termPosi.insert(make_pair(term,posi));
    ve.clear();
    //cout << line<<"\n";
  }

  //cout << docNames.size() <<"\n";
  //cout << termPosi[2] <<"\n";

}


bool QueryFinder::readInvFile(map<int,int> id_terms_qtd, string query){
  //cout << "Reading lines on inv file" << "\n";
  fstream file(INVERTED_PATH + INVERTED_NAME,ios::in );
  string line,url;
  std::vector<std::string> ve;
  int num,doc,posi;
  float tf, tfquery;

  std::map<int,int> doc_qtd;
  std::map<int,float> doc_rank;

  if(file.fail()){
      cout <<" Some errors reading invFile\n" <<endl;
      return false;
  }
  
  for(auto id = id_terms_qtd.begin(); id != id_terms_qtd.end(); id++){
  //for(auto id: id_terms_qtd){
    //cout << termPosi[498]<< "\n";
    GotoLine2(file,termPosi[id->first]);
    //GotoLine(file, id->first);
    getline(file,line);
    
    strtk::parse(line," ",ve);
    //cout << line << "\n";
    line = "";
    for(int i = 1; i < ve.size(); i+= 2){
      doc = atoi(ve[i].c_str());
      //posi = atoi(ve[i+1].c_str());

      if(doc_qtd.find(doc)!= doc_qtd.end()){
        //found doc
        doc_qtd[doc]+=1;

      }else{
        //not found doc
        doc_qtd.insert(make_pair(doc,1));

      }

    }
    //ended for some term
    ve.clear();

    float idf2 = dictionaryMap[id->first].idf;

    for(auto item = doc_qtd.begin(); item != doc_qtd.end(); item++){
      tf = (1 + log(item->second)); // tf
      tfquery = (1 + log(id->second)); // quantity of terms in the query

      if(doc_rank.find(item->first) == doc_rank.end()){
        //doc not found yet
        doc_rank.insert(make_pair(item->first,0));
        //doc_rank[item->first] += dictionaryMap[id].idf * tf;
      }
        
      doc_rank[item->first] += tfquery * idf2 * tf * idf2; // numerator

      
    }

    doc_qtd.clear();

  }

  // already have the top part
  vector<pair<float,int>> vecSort;
  for(auto d = doc_rank.begin(); d != doc_rank.end(); d++){
    if(docWij[d->first] != 0){
      doc_rank[d->first] /= docWij[d->first];
    }else{
      doc_rank[d->first] = 0;
    }
    vecSort.push_back(make_pair(doc_rank[d->first],d->first));
  }

  doc_rank.clear();

  sort(vecSort.rbegin(),vecSort.rend());
  string msg = "number of results: "+to_string(vecSort.size())+"\n";
  //for (std::map<int, float>::iterator i = doc_rank.begin(); i != doc_rank.end(); i++){
  for(int i = 0; i < vecSort.size(); i++){
    if(i >=5){
      break;
    }
    msg += "page URL: "+docNames[vecSort[i].second]+"\nrank: "+to_string(vecSort[i].first)+"\n";
  }
    

  cout << msg;
  
  file.close();
  saveQuery(query, msg);
}

bool QueryFinder::readVocab(){
  cout << "Loading vocabulary" << "\n";
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
  readDocPosi();
  readDict();
  readVocab();
  readDocNames();
  readDocWij();
  

  double t1 = elapsed();
  std::cout << "Total prestart time : "<< t1-t0 << "s" << endl << endl;

  return true;
}



bool QueryFinder::startSearch(){

  cout << "Starting search for queries on "+QUERIES_NAME+"\n";
  double t0 = elapsed();
  int numQueries = 0; 
  string query;
  map<string,int> term_qtd;
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
        if (std::find(std::begin(words2avoid), std::end(words2avoid), terms[q]) != std::end(words2avoid)){
          //term is a forbidden word (article)
          continue;
        }

        if(term_qtd.find(terms[q])!= term_qtd.end()){
          //cout << "The position of " << t <<" is " << vocabMap[t] <<"\n";
          term_qtd[terms[q]] += 1;
        }else{
          term_qtd.insert(make_pair(terms[q],1));
          //cout << "Can't find " << t << "\n";
        }
        //cout << terms[q] <<endl;
      }

      terms.clear();
      processQuery(term_qtd, query);

      numQueries++;
      terms.clear();
      term_qtd.clear();
    }

  }catch (int e){
      std::cout << "Error on saving" << endl;
      return false;
  }

  double t1 = elapsed();
  double tim = t1-t0;
  std::cout << "\nTotal number of queries: "<< numQueries << endl;
  std::cout << "Average response time per query : "<< tim/numQueries << "s" << endl << endl;
  std::cout << "Total searching time : "<< tim << "s" << endl << endl;

  return true;


}

bool QueryFinder::processQuery(map<string, int> terms, string query){
  map<int, int> id_terms;
  for(auto t  = terms.begin(); t != terms.end(); t++){
    if(vocabMap.find(t->first)!= vocabMap.end()){
      //cout << "The position of " << t <<" is " << vocabMap[t] <<"\n";
      id_terms.insert(make_pair(vocabMap[t->first],t->second));
      //cout << "The position of " << t->first <<" is " << vocabMap[t->first] <<" qtd "<< t->second <<"\n";
    }
      //cout << "Can't find " << t << "\n";
  }

  if(id_terms.size() == 0){
    string msg = "no matches for your query .. \n";
    cout << msg;
    saveQuery(query, msg);
  }else{
    readInvFile(id_terms, query);
  }

  return true;
}

int main(int argc, char **argv){
    QueryFinder p;
    //p.readDocNames();
    //p.writeDocPosi();
    p.preStart();
    
    p.startSearch();


    return 0;
}
