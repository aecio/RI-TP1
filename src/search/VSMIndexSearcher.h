/*
 * VSMIndexSearcher.h
 *
 *  Created on: 27/04/2011
 *      Author: aecio
 */

#ifndef VSMINDEXSEARCHER_H_
#define VSMINDEXSEARCHER_H_

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#include <queue>
#include "index/Pair.h"
#include "index/Vocabulary.h"
#include "search/Hit.h"
#include "textanalysis/TextTokenizer.h"
#include "util/SequenceFile.h"

class VSMIndexSearcher {

	Vocabulary* vocabulary;
	SequenceFile<Pair>* invertedLists;
	SequenceFile<Doc>* pagesFile;
	
public:

	VSMIndexSearcher(string directory) {
		vocabulary = new Vocabulary(directory + "/vocabulary");
		invertedLists = new SequenceFile<Pair>(directory + "/index", false);
		pagesFile = new SequenceFile<Doc>(directory + "/urls", false);
	}
	
	vector<Hit> search(string query, int maxHits) {
		TextTokenizer tokenizer(query);

		//Query parsing...
		vector<string> tokens;
		while(tokenizer.hasNext()) {
			tokens.push_back(tokenizer.nextToken());
		}
		
		//Score calculus
		map<int, double> accumulators;
		int numDocs = pagesFile->getSize();
		
		vector<string>::iterator it = tokens.begin();
		for(;it != tokens.end(); it++){
			Term* term = vocabulary->findTerm(*it);

			if(term == NULL)
				continue;
			
			double wt = 1 + log( numDocs/term->docFrequency ); // deixar pré-computado no índice?
			
			Pair* invertedList = readInvertedList(term);
			
			for(int i=0; i<term->docFrequency; i++){
				double valor;//normalization
				map<int, double>::iterator value = accumulators.find(invertedList[i].docId);
				if(value == accumulators.end() ){
					accumulators[invertedList[i].docId] = 0;
					valor = accumulators[invertedList[i].docId];
				}else{
					valor = value->second;
				}
				valor += log(1+ invertedList[i].frequency_dt) * wt;
				accumulators[invertedList[i].docId] = valor;
			}
		}
		
		
		//Document length normatization and TopDocs selection
		priority_queue< Hit, vector<Hit>, greater<Hit> > topDocs;

		map<int, double>::iterator ac = accumulators.begin();
		
		for(int i=0; i < maxHits && ac != accumulators.end(); i++, ac++){
			Hit hit = createHit(ac->first, ac->second);
			topDocs.push(hit);
		}
		
		for(; ac != accumulators.end(); ac++){
			Hit hit = createHit(ac->first, ac->second);
			if(topDocs.top() < hit){
				topDocs.pop();
				topDocs.push(hit);
			}
		}
		
		vector<Hit> result;
		while( !topDocs.empty() ){
			result.push_back(topDocs.top());
			topDocs.pop();
		}
		reverse(result.begin(), result.end());
		
		return result;
	}
	
	Hit createHit(int docId, double score){
		pagesFile->setPosition(docId-1);
		Doc doc = pagesFile->read();
		score = score / doc.length;	//document length normalization
		return Hit(score, doc);
	}
	
	Pair* readInvertedList(Term* term){
		Pair* pairs = new Pair[term->docFrequency];
		invertedLists->setPosition(term->listPosition);
		invertedLists->readBlock(pairs, term->docFrequency);
		return pairs;
	}
	
};

#endif /* VSMINDEXSEARCHER_H_ */
