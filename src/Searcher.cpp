/*
 * Searcher.cpp
 *
 *  Created on: 18/04/2011
 *      Author: aecio
 */
#include <iostream>
#include <string>
#include <vector>
#include "IndexSearcher.h"
#include "Pair.h"
#include "Doc.h"

using namespace std;

int main(int argc, char* argv[]){
	
	string directory = ".";
	
	//Parse comand line arguments
	for(int i=0; i<argc; i++){
		string param(argv[i]);
		if(param == "--directory" || param == "-d"){
			i++;
			directory = string(argv[i]);
		}
	}
	cout << "Loading index..." << endl;
	IndexSearcher searcher(directory);
	SequenceFile<Doc>* pagesFile = new SequenceFile<Doc>(directory + "/urls", false);
	while(pagesFile->hasNext()){
		cout << pagesFile->getPosition() << " = " << pagesFile->read().url << endl;
	}
	
	
	string query;
	cout << "Type you query (and press ENTER): ";
	getline(cin, query);
	while(query != "sair"){
		vector<Pair> hits = searcher.search(query);
		if(hits.size() == 0){
			cout << endl << "Documents not found for your query. Try again." << endl << endl;
		} else {
			cout << "Search results for: " << query << endl;
			vector<Pair>::iterator it = hits.begin();
			for(; it != hits.end(); it++){
				pagesFile->setPosition(it->docId-1);
				Doc d = pagesFile->read();
				cout << " -> " << it->docId << " - ";
				cout << d.url << endl;
			}
		}
		cout << "Type you query (and press ENTER): ";
		getline(cin, query);
	}
	return 0;
}

