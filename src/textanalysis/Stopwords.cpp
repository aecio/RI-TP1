#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tr1/unordered_set>

#include "Stopwords.h"

/**
 * Lê stopwords de um arquivo de entrada.
 */
using namespace std;
using std::tr1::unordered_set;

Stopwords::Stopwords(string file){
    stopwords = unordered_set<string>();
	ifstream fin(file.c_str());
	while (fin) {
		string line;
		getline(fin, line);
		stopwords.insert(line);
	}
	fin.close();
}

unordered_set<string> Stopwords::list(){
	return stopwords;
}

bool Stopwords::isStopword(string word){
	if( stopwords.find(word) == stopwords.end() )
		return false;
	else
		return true;
}
