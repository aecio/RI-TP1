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
#include "search/IndexSearcher.h"
#include "search/BooleanIndexSearcher.h"
#include "search/VSMIndexSearcher.h"
#include "search/BM25IndexSearcher.h"
#include "search/CombinedIndexSearcher.h"
#include "search/Hit.h"
#include "index/Pair.h"
#include "index/Doc.h"

using namespace std;

void processarConsultas(string, string);
bool ProcessArgs(const int argc, const int number_of_args, bool print_erro_message, std::string erro_message);

IndexSearcher* searcher;
int MAX_HITS = 100;

int main(int argc, char** argv) { 
	//args
	string usage_error_message = "Usage: <complete_path_to_consultas_file> <results-directory> <vsm|bm25|comb>";
	if( ! ProcessArgs(argc, 4, true, usage_error_message) ){
		return 1; // erro while passing args
	}
	string consultas_filename = argv[1];
	string respostas_directory = argv[2];

	IndexReader* reader = new IndexReader("indice");
	
	if(string("vsm").compare(argv[3]) == 0){
		cout << "Usando o Modelo Vetorial" << endl;
		searcher = new VSMIndexSearcher(reader);
	} else if(string("bm25").compare(argv[3]) == 0){
		cout << "Usando o BM25" << endl;
		searcher = new BM25IndexSearcher(reader);
	}else if(string("comb").compare(argv[3]) == 0){
		cout << "Usando o Combinação VSM e BM25" << endl;
		searcher = new CombinedIndexSearcher(reader);
	}
	
	processarConsultas(consultas_filename, respostas_directory);

	return 0;
}

void processarConsultas(string consultas_filename, string respostas_directory) {

	//Abrir arquivo de consultas
	ifstream input_consultas((consultas_filename).c_str());
	if( !input_consultas.is_open()){
		cerr << "ERRO  - Nao foi possivel abrir o arquivo:" <<(consultas_filename) << endl;
		exit(1);
	}

	//Para cada consulta no arquivo
	string query;
	getline(input_consultas, query);
	while(!input_consultas.eof()){
		cout << endl << "# Processando: " << query << endl;    

		//arquivo p/ gravacao
		ofstream output((respostas_directory+query).c_str());
		if( !output.is_open()){
		  cerr << "ERRO  - Nao foi possivel abrir o arquivo:" <<(respostas_directory+query+".num")<< endl;
		  exit(1);
		}

		vector<Hit> hits = searcher->search(query, MAX_HITS);
		vector<Hit>::iterator it = hits.begin();
		for(;it != hits.end(); it++){
			output << it->doc.id << endl;
		}
		output.close();
		
		getline(input_consultas, query);
		
	}//while
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
