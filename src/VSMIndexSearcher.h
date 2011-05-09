/*
 * VSMIndexSearcher.h
 *
 *  Created on: 27/04/2011
 *      Author: aecio
 */

#ifndef VSMINDEXSEARCHER_H_
#define VSMINDEXSEARCHER_H_

#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include "SequenceFile.h"
#include "Pair.h"
#include "Vocabulary.h"
#include "TextTokenizer.h"

class VSMIndexSearcher {

	Vocabulary* vocabulary;
	SequenceFile<Pair>* invertedLists;
	
public:

	VSMIndexSearcher(string directory) {
		vocabulary = new Vocabulary(directory + "/vocabulary");
		invertedLists = new SequenceFile<Pair>(directory + "/index", false);
	}
	
	vector<Pair> search(string query) {
		TextTokenizer tokenizer(query);

		vector<string> tokens;
		while(tokenizer.hasNext()) {
			tokens.push_back(tokenizer.nextToken());
		}
		
		map<int, double> accumulators;
		int numDocs = 0; //TODO: Ler esse valor do indice
		
		vector<string>::iterator it = tokens.begin();
		for(;it != tokens.end(); it++){
			Term* term = vocabulary->findTerm(*it);
			//TODO: se não encontar o termo? (term == NULL)

			double wt = 1 + log( numDocs/term->docFrequency );
			
			Pair* invertedList = readInvertedList(term);
			
			for(int i=0; i<term->docFrequency; i++){
				map<int, double>::iterator value = accumulators.find(invertedList[i].docId);
				if(value == accumulators.end() ){
					accumulators[invertedList[i].docId] = 0;
				}
				double& ac = value->second;
				ac += log(1+ invertedList[i].frequency_dt) * wt;
			}
		}
		
		//pra cada acumulador, dividir pela normalização do documento
		map<int, double>::iterator ac = accumulators.begin();
		for(; ac != accumulators.end(); ac++){
			int docId = ac->first;
			double documentNorm; //read document normalization using docId
			ac->second = ac->second / documentNorm;
		}
		
		
		//selecionar os k topDocs
		
		//retornar hits
		vector<Pair> result;
		return result;
	}
	
	Pair* readInvertedList(Term* term){
		Pair* pairs = new Pair[term->docFrequency];
		invertedLists->setPosition(term->listPosition);
		invertedLists->readBlock(pairs, term->docFrequency);
		return pairs;
	}
	
};

#endif /* VSMINDEXSEARCHER_H_ */
