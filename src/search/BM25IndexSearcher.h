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

class BM25IndexSearcher {

	Vocabulary* vocabulary;
	SequenceFile<Pair>* invertedLists;
	SequenceFile<Doc>* pagesFile;
	double avgDocLen;
	
public:

	BM25IndexSearcher(string directory) {
		vocabulary = new Vocabulary(directory + "/vocabulary");
		invertedLists = new SequenceFile<Pair>(directory + "/index", false);
		pagesFile = new SequenceFile<Doc>(directory + "/urls", false);
		
		SequenceFile<double>* avgdoclenFile = new SequenceFile<double>(directory + "/avgdoclen", false);
		avgDocLen = avgdoclenFile->read();
		avgdoclenFile->close();
	}
	
	vector<Hit> search(string query, int maxHits) {
		//###
		clock_t init_search = clock();
		//###
		
		TextTokenizer tokenizer(query);

		//Query parsing...
		vector<string> tokens;
		while(tokenizer.hasNext()) {
			tokens.push_back(tokenizer.nextToken());
		}
		
		//Defining constants
		
		double b = 0.75; //b should be kept close to 1 to emphasise BM11 formula.
		double K1 = 1; //Empirical constant. k1 = 1 works well with real collections
		int N = pagesFile->getSize(); // N = Number of documents in the collection

		//Ranking calculation
		map<int, Hit> accumulators;
		

		vector<string>::iterator it = tokens.begin();
		for(;it != tokens.end(); it++){
			//###
			clock_t inicio_term = clock();
			//###
			
			Term* term = vocabulary->findTerm(*it);

			if(term == NULL)
				continue;

			// ni = numero de docs que contem termo k_i
			int ni = term->docFrequency;
			
			double probabilisticFactor = log( ( N - ni + 0.5 )/(ni + 0.5) );
			
			//###
			clock_t inicio_list = clock();
			//###
			Pair* invertedList = readInvertedList(term);
			//###
			clock_t fim_list = clock();
			double total_list = ((double)(fim_list-inicio_list)) / (double) (CLOCKS_PER_SEC/1000);
			printf("Tempo para ler lista do termo %s: %.3f ms\n", term->term, total_list);
			//###
			
			for(int i=0; i<term->docFrequency; i++){
				
				map<int, Hit>::iterator value = accumulators.find(invertedList[i].docId);
				//###
				clock_t inicio_pair = clock();
				//###
				Hit accumulator;
				if(value == accumulators.end() ){
					accumulator = accumulators[invertedList[i].docId];
					pagesFile->setPosition(invertedList[i].docId-1);
					accumulator.doc = pagesFile->read();
				}else{
					accumulator = value->second;
				}
				clock_t fim_pair = clock();
				
				double docNormalization = accumulator.doc.length / avgDocLen;
				int f_ij = invertedList[i].frequency_dt;
				
				double tfIdfFactor = ((K1+1) * f_ij) / ( K1 * ((1-b)+(b*docNormalization)) + f_ij );
				
				accumulator.score += tfIdfFactor * probabilisticFactor;
				
				accumulators[invertedList[i].docId] = accumulator;
				
				//###
				double total = ((double)(fim_pair-inicio_pair)) / (double) (CLOCKS_PER_SEC/1000);
				if(total > 0)
					printf("Tempo para doc %i: %.3f ms\n", invertedList[i].docId, total);
				//###
			}
			//###
			clock_t fim_term = clock();
			double total = ((double)(fim_term-inicio_term)) / (double) (CLOCKS_PER_SEC/1000);
			printf("Tempo para o termo %s: %.0f ms\n\n", term->term, total);
			//###
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
		
		//###
		clock_t end_search = clock();
		double total_time = ((double)(end_search-init_search)) / (double) (CLOCKS_PER_SEC/1000);
		printf("Search finished in %.3f ms\n", total_time);
		//###
		return result;
	}
	
	Pair* readInvertedList(Term* term){
		Pair* pairs = new Pair[term->docFrequency];
		invertedLists->setPosition(term->listPosition);
		invertedLists->readBlock(pairs, term->docFrequency);
		return pairs;
	}
	
};

#endif /* BM25INDEXSEARCHER_H_ */
