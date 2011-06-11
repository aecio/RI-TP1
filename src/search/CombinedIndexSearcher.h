/*
 * CombinedIndexSearcher.h
 *
 *  Created on: 27/04/2011
 *      Author: aecio
 */

#ifndef COMBINEDINDEXSEARCHER_H_
#define COMBINEDINDEXSEARCHER_H_

#include <algorithm>
#include <cmath>
#include <string>
#include <time.h>
#include <vector>
#include <queue>
#include "index/Pair.h"
#include "index/Vocabulary.h"
#include "search/Hit.h"
#include "textanalysis/TextTokenizer.h"
#include "util/SequenceFile.h"

class CombinedIndexSearcher : public IndexSearcher {

	IndexReader* indexReader;
	IndexSearcher* bm25;
	IndexSearcher* vsm;

public:

	CombinedIndexSearcher(string directory) {
		indexReader = new IndexReader(directory);
	}
	
	CombinedIndexSearcher(IndexReader* ir) {
		indexReader = ir;
		vsm = new VSMIndexSearcher(indexReader);
		bm25 = new BM25IndexSearcher(indexReader);
	}
	
	vector<Hit> search(string query) {
		return search(query, 10);
	}
	
	vector<Hit> search(string query, int maxHits) {
		vector<Hit> hits_bm25 = bm25->search(query, maxHits*5);
		vector<Hit> hits_vsm = vsm->search(query, maxHits*5);
		map<int, Hit> accumulators;
		double K = 0.5;
		
		//normalize bm25 scores
		vector<Hit>::iterator it = hits_bm25.begin();
		double max_bm25 = it->score;
		for(; it != hits_bm25.end(); it++){
			if(it->score > max_bm25)
				max_bm25 = it->score;
		}
		it = hits_bm25.begin();
		for(; it != hits_bm25.end(); it++){
			double normalized_bm25 = K * (it->score / max_bm25);
			accumulators[it->doc.id] = Hit(normalized_bm25, it->length, it->doc);
		}
		
		//normalize vsm scores
		it = hits_vsm.begin();
		double max_vsm = it->score;
		for(; it != hits_vsm.end(); it++){
			if(it->score > max_vsm)
				max_vsm = it->score;
		}
		
		it = hits_vsm.begin();
		for(; it != hits_vsm.end(); it++){
			double normalized_vsm = (1-K) * (it->score / max_vsm);
			map<int, Hit>::iterator hit = accumulators.find(it->doc.id);
			if(hit == accumulators.end() ){
				accumulators[it->doc.id] = Hit(normalized_vsm, it->length, it->doc);
			}else{
				accumulators[it->doc.id].score += normalized_vsm;
			}
		}

		
		//TopDocs selection using a heaHeap (priority_queue is a Heap implementation)
		priority_queue< Hit, vector<Hit>, greater<Hit> > topDocs;

		map<int, Hit>::iterator ac = accumulators.begin();
		
		for(int i=0; i < maxHits && ac != accumulators.end(); i++, ac++){
			topDocs.push(ac->second);
		}
		
		for(; ac != accumulators.end(); ac++){
			if(topDocs.top() < ac->second){
				topDocs.pop();
				topDocs.push(ac->second);
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
};

#endif /* COMBINEDINDEXSEARCHER_H_ */
