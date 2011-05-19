/*
 * Searcher.cpp
 *
 *  Created on: 18/04/2011
 *      Author: aecio
 */
#include <iostream>
#include <string>
#include <vector>
#include "search/BooleanSearcher.h"
#include "search/VSMIndexSearcher.h"
#include "index/Pair.h"
#include "index/Doc.h"

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
	VSMIndexSearcher searcher(directory);
	
//	SequenceFile<Doc>* pagesFile = new SequenceFile<Doc>(directory + "/urls", false);
//	while(pagesFile->hasNext()) {
//		cout << pagesFile->getPosition() << " = " << pagesFile->read().url << endl;
//	}
	
	string query;
	cout << "Type you query (and press ENTER): ";
	getline(cin, query);
	while(query != "sair"){
		vector<Doc> hits = searcher.search(query);
		
		if(hits.size() == 0){
			cout << endl << "Documents not found for your query. Try again." << endl;
		} else {
			cout << "Search results for: " << query << endl;
			vector<Doc>::iterator it = hits.begin();
			for(; it != hits.end(); it++){
				cout << endl;
				cout << "(" << it->id  << ") - " << it->url << endl;
				cout << "score: " << "0.0000" << endl;
				cout 		<< "length: " << it->length << endl;
			}
		}
		cout << endl << "Type you query (and press ENTER): ";
		getline(cin, query);
	}
	return 0;
}

