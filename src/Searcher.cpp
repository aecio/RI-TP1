/*
 * Searcher.cpp
 *
 *  Created on: 18/04/2011
 *      Author: aecio
 */
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "search/IndexSearcher.h"
#include "search/BooleanIndexSearcher.h"
#include "search/VSMIndexSearcher.h"
#include "search/BM25IndexSearcher.h"
#include "search/MultiFieldIndexSearcher.h"
#include "search/Hit.h"
#include "index/Pair.h"
#include "index/Doc.h"

using namespace std;

int main(int argc, char* argv[]){
	enum IRModel {BM25, VECTOR, BOOLEAN, MULTIFIELD};
	
	int maxHits = 10;
	string directory = ".";
	IndexSearcher* searcher;
	IRModel model = MULTIFIELD;
	
	//Parse comand line arguments
	for(int i=0; i<argc; i++){
		string param(argv[i]);
		if(param == "--directory" || param == "-d"){
			i++;
			directory = string(argv[i]);
		}
		if(param == "--model" || param == "-m"){
			i++;
			string modelStr(argv[i]);
			if(modelStr == "vector" || modelStr == "Vector" )
				model = VECTOR;
			else if(modelStr == "bm25" || modelStr == "BM25")
				model = BM25;
		}
		if(param == "--maxHits" || param == "-h"){
			i++;
			maxHits = atoi(argv[i]);
		}
	}
	
	cout << "Loading index...";
	switch(model){
		case VECTOR:
			cout << "(using Vector Spacel Model)" << endl;
			searcher = new VSMIndexSearcher(directory);
			break;
		case BM25:
			cout << "(using BM25 Model)" << endl;
			searcher = new BM25IndexSearcher(directory);
			break;
		default:
			cout << "(using BM25 Model with MultiFieldSearcher)" << endl;
			searcher = new MultiFieldIndexSearcher(directory);
			break;
	}
	
	string query;
	cout << "Type you query (and press ENTER): ";
	getline(cin, query);
	while(query != "sair"){
		vector<Hit> hits = searcher->search(query, maxHits);

		if(hits.size() == 0){
			cout << endl << "Documents not found for your query. Try again." << endl;
		} else {
			cout << "Search results for: " << query << endl;
			vector<Hit>::iterator it = hits.begin();
			for(; it != hits.end(); it++){
				cout << endl;
				cout << it->doc.title << endl;
				cout << it->doc.url << endl;
				cout << "score: " << it->score << "\t";
				cout << "docId: " << it->doc.id << "\t"; 
				cout << "docLength: " << it->length << endl;
			}
		}
		cout << "___________________________________________" << endl;
		cout << endl << "Type you query (and press ENTER): ";
		getline(cin, query);
	}
	return 0;
}

