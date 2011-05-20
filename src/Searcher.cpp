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
#include "search/BM25IndexSearcher.h"
#include "search/Hit.h"
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
//	BM25IndexSearcher searcher(directory);
	
	string query;
	cout << "Type you query (and press ENTER): ";
	getline(cin, query);
	while(query != "sair"){
		vector<Hit> hits = searcher.search(query, 20);
		
		if(hits.size() == 0){
			cout << endl << "Documents not found for your query. Try again." << endl;
		} else {
			cout << "Search results for: " << query << endl;
			vector<Hit>::iterator it = hits.begin();
			for(; it != hits.end(); it++){
				cout << endl;
				cout << it->doc.url << endl;
				cout << "score: " << it->score << endl;
				cout << "docId: " << it->doc.id << endl;
				cout << "length: " << it->doc.length << endl;
			}
		}
		cout << endl << "Type you query (and press ENTER): ";
		getline(cin, query);
	}
	return 0;
}

