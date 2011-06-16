/*
 * PageRankSearcher.h
 *
 *  Created on: 15/06/2011
 *      Author: aecio
 */

#ifndef PAGERANKSEARCHER_H_
#define PAGERANKSEARCHER_H_

#include <string>
#include <vector>
#include <queue>
#include <iomanip>
#include <ios>
#include "search/IndexReader.h"
#include "search/IndexSearcher.h"
#include "search/BM25IndexSearcher.h"

using namespace std;

class PageRankSearcher : public IndexSearcher {

	IndexReader* indexReader;
	IndexSearcher* bm25;

public:

	PageRankSearcher(IndexReader* ir) {
		bm25 = new  BM25IndexSearcher(ir);
		indexReader = ir;
	}

	~PageRankSearcher(){
		delete bm25;
	}

	vector<Hit> search(string query) {
		return search(query, 10);
	}

	vector<Hit> search(string query, int maxHits){
		vector<Hit> hits_bm25 = bm25->search(query, maxHits*5);
		double K = 0.5;

		vector<Hit>::iterator it = hits_bm25.begin();
		it = hits_bm25.begin();
		for(; it != hits_bm25.end(); it++){
			double pageRank = indexReader->getPageRank(it->doc.getId());
			cout << setw(4) << it->doc.getId() << " = " << setw(10) << fixed << pageRank << "\t" << it->doc.getUrl() << endl;
			it->score = K * (it->score) + (1-K) * pageRank;
		}



		//TopDocs selection using a heaHeap (priority_queue is a Heap implementation)
		priority_queue< Hit, vector<Hit>, greater<Hit> > topDocs;

		it = hits_bm25.begin();
		for(int i=0; i < maxHits && it != hits_bm25.end(); i++, it++){
			topDocs.push(*it);
		}

		for(; it != hits_bm25.end(); it++){
			if(topDocs.top() < *it){
				topDocs.pop();
				topDocs.push(*it);
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

#endif /* PAGERANKSEARCHER_H_ */
