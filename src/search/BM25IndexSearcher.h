/*
 * BM25IndexSearcher.h
 *
 *  Created on: 27/04/2011
 *      Author: aecio
 */

#ifndef BM25INDEXSEARCHER_H_
#define BM25INDEXSEARCHER_H_

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

class BM25IndexSearcher : public IndexSearcher {

	IndexReader* indexReader;

public:

	BM25IndexSearcher(string directory) {
		indexReader = new IndexReader(directory);
	}
	
	BM25IndexSearcher(IndexReader* ir) {
		indexReader = ir;
	}
	
	vector<Hit> search(string query) {
		return search(query, 10);
	}
	
	vector<Hit> search(string query, int maxHits) {
		cout << "Searching term \"" << query << "\"..." << endl;
		
		//###
		clock_t init_search = clock();
		//###
		
		TextTokenizer tokenizer(query);

		//Query parsing...
		vector<string> tokens;
		while(tokenizer.hasNext()) {
			string t = tokenizer.nextToken();
			tokens.push_back(t);
			cout << t << endl;
		}
		
		//Defining constants
		double b = 0.75; //b should be kept close to 1 to emphasise BM11 formula.
		double K1 = 1; //Empirical constant. k1 = 1 works well with real collections
		int N = indexReader->getCollectionSize(); // N = Number of documents in the collection

		//Ranking calculation
		map<int, Hit> accumulators;
		vector<string>::iterator it = tokens.begin();
		for(;it != tokens.end(); it++){
			//###
			clock_t inicio_term = clock();
			//###
			
			Term* term = indexReader->getTerm(*it);
			if(term == NULL)
				continue;

			cout << "Searching documents containing term \"" << term->term << "\"..." << endl;			
			
			int ni = term->docFrequency; // ni = numero de docs que contem termo k_i
			double probabilisticFactor = log( ( N - ni + 0.5 )/(ni + 0.5) );
			
			Pair* invertedList = indexReader->getInvertedList(term);
			
			for(int i=0; i<term->docFrequency; i++){
				
				map<int, Hit>::iterator value = accumulators.find(invertedList[i].docId);
				Hit accumulator;
				if(value == accumulators.end() ){
					accumulator = accumulators[invertedList[i].docId];
					accumulator.doc = indexReader->getDoc(invertedList[i].docId);
				}else{
					accumulator = value->second;
				}
				
				int f_ij = invertedList[i].frequency_dt;
				double docNormalization = accumulator.doc.length / indexReader->getAverageDocLength();
				double tfIdfFactor = ((K1+1) * f_ij) / ( K1 * ((1-b)+(b*docNormalization)) + f_ij );
				
				accumulator.score += tfIdfFactor * probabilisticFactor;
				
				accumulators[invertedList[i].docId] = accumulator;
			}
				
			//###
			clock_t fim_term = clock();
			double total = ((double)(fim_term-inicio_term)) / (double) (CLOCKS_PER_SEC/1000);
			printf("Time to search term \"%s\": %.0f ms\n", term->term, total);
			//###
		}
		
		//###
		clock_t inicio_top = clock();
		//###
		
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
		
		//###
		clock_t fim_top = clock();
		double total_topdocs = ((double)(fim_top-inicio_top)) / (double) (CLOCKS_PER_SEC/1000);
		printf("Topdocs selection time: %.0f ms\n", total_topdocs);
		//###
		
		
		//###
		clock_t end_search = clock();
		double total_time = ((double)(end_search-init_search)) / (double) (CLOCKS_PER_SEC/1000);
		printf("Search executed in %.0f ms\n\n", total_time);
		//###
		return result;
	}
};

#endif /* BM25INDEXSEARCHER_H_ */
