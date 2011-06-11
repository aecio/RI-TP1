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
#include "search/IndexReader.h"
#include "textanalysis/TextTokenizer.h"
#include "util/SequenceFile.h"

class VSMIndexSearcher : public IndexSearcher {

	IndexReader* indexReader;
	
public:

	VSMIndexSearcher(string directory) {
		indexReader = new IndexReader(directory);
	}
	
	VSMIndexSearcher(IndexReader* ir) {
		indexReader = ir;
	}
	
	~VSMIndexSearcher(){
		delete indexReader;
	}
	
	vector<Hit> search(string query) {
		return search(query, 10);
	}
	
	vector<Hit> search(string query, int maxHits) {
		TextTokenizer tokenizer(query);

		//Query parsing...
		vector<string> tokens;
		while(tokenizer.hasNext()) {
			tokens.push_back(tokenizer.nextToken());
		}
		
		//Score calculus
		map<int, Hit> accumulators;
		int numDocs = indexReader->getCollectionSize();
		
		vector<string>::iterator it = tokens.begin();
		for(;it != tokens.end(); it++){

			Term* term = indexReader->getTerm(*it);
			if(term == NULL)
				continue;
			
			double wt = 1 + log( numDocs/term->getFieldFrequency(CONTENT) ); // deixar pré-computado no índice?
			
			Pair* invertedList = indexReader->getInvertedList(term);
			
			for(int i=0; i<term->getFieldFrequency(CONTENT); i++){
				Hit accumulator;
				map<int, Hit>::iterator value = accumulators.find(invertedList[i].doc_id);
				if(value == accumulators.end() ){
					accumulator = accumulators[invertedList[i].doc_id];
					accumulator.doc.id = invertedList[i].doc_id;
					accumulator.length = indexReader->getDocLength(invertedList[i].doc_id);
				}else{
					accumulator = value->second;
				}
				accumulator.score += log(1+ invertedList[i].frequency_dt) * wt;
				accumulators[invertedList[i].doc_id] = accumulator;
			}
		}
		
		
		//Document length normatization and TopDocs selection
		priority_queue< Hit, vector<Hit>, greater<Hit> > topDocs;

		map<int, Hit>::iterator ac = accumulators.begin();
		
		for(int i=0; i < maxHits && ac != accumulators.end(); i++, ac++){
			ac->second.score = ac->second.score / ac->second.length; //document length normalization
			topDocs.push(ac->second);
		}
		
		for(; ac != accumulators.end(); ac++){
			ac->second.score = ac->second.score / ac->second.length; //document length normalization
			if(topDocs.top() < ac->second ){
				topDocs.pop();
				topDocs.push(ac->second);
			}
		}
		
		vector<Hit> result;
		while( !topDocs.empty() ){
//			result.push_back(topDocs.top());
//			topDocs.pop();
			Hit hit = topDocs.top();
			hit.doc = indexReader->getDoc(hit.doc.id);
			result.push_back(hit);
			topDocs.pop();
		}
		reverse(result.begin(), result.end());
		
		return result;
	}
	
};

#endif /* VSMINDEXSEARCHER_H_ */
