#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <map>
#include <iterator>
#include <cmath>
#include "util/SequenceFile.h"
#include "index/Doc.h"

using namespace std;

map<string,int> urls_map;

void mapURL();
void gerarFilesID();
vector<std::string> &split(const string &s, char delim, vector<std::string> &elems);
vector<std::string> split(const std::string &s, char delim);
bool ProcessArgs(const int argc, const int number_of_args, bool print_erro_message, std::string erro_message);

string consultas_filename;
string consultasDirectory;
string query;

int main(int argc, char** argv) { 

	//args
  string usage_error_message = "Usage: <complete_path_to_consultas_file> <results-directory>";
  if( ! ProcessArgs(argc, 3, true, usage_error_message) ){
    return 1; // erro while passing args
  }


  consultas_filename = argv[1];
  consultasDirectory = argv[2];
  string query;
	
	
	cerr << consultas_filename << endl;
	cerr << consultasDirectory << endl;


	mapURL();
	
	gerarFilesID();
	
	return 0;

}

void mapURL() { 
	cerr << "## Criando Map de URLS" << endl;
	SequenceFile<Doc>* url_file = new SequenceFile<Doc>("indice/urls", false);
	while(url_file->hasNext()) {
		Doc doc = url_file->read();
		urls_map[string(doc.url)] = doc.id;		
//		cout << doc.url << endl;
	}
}

void gerarFilesID() {


  ifstream input_consultas((consultas_filename).c_str());
  if( !input_consultas.is_open()){
    cerr << "ERRO  - Nao foi possivel abrir o arquivo:" <<(consultas_filename) << endl;
    exit(1);
  }

  getline( input_consultas, query );
  while(!input_consultas.eof()){
    cout << "# Processando: " << query << endl;    
      //ABRINDO ARQUIVO TEMP  
    ifstream input((consultasDirectory+query).c_str());
    if( !input.is_open()){
      cerr << "ERRO  - Nao foi possivel abrir o arquivo:" <<(consultasDirectory+query)<< endl;
      exit(1);
    }
      //arquivo p/ gravacao
    ofstream output((consultasDirectory+query+".num").c_str());
    if( !output.is_open()){
      cerr << "ERRO  - Nao foi possivel abrir o arquivo:" <<(consultasDirectory+query+".num")<< endl;
      exit(1);
    }
    string url;     
  
    getline(input, url);
    while(!input.eof()){
      map<string, int>::iterator it = urls_map.find(url);
      if(it != urls_map.end()){
        output << it->second << endl; 
      }
      else{
        cerr << "\tUrl não encontrada: " << it->second << endl;
        output << "-1" << endl; 
      }
      getline(input, url);
    }
   
    getline( input_consultas, query );
  }//while
}

vector<std::string> &split(const string &s, char delim, vector<std::string> &elems) {
    stringstream ss(s);
    string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


vector<std::string> split(const std::string &s, char delim) {
    vector<std::string> elems;
    return split(s, delim, elems);
}


bool ProcessArgs(const int argc, const int number_of_args, bool print_erro_message, std::string erro_message)
{
  if(argc == number_of_args){
    return true;
  }
  else{
    if(print_erro_message)
      std::cerr << erro_message << endl;
    return false;
  }
}
