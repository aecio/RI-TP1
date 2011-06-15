/*
 * MultiFieldIndexSearcher.h
 *
 *  Created on: 09/06/2011
 *      Author: aecio
 */

#ifndef MULTIFIELDINDEXSEARCHER_H_
#define MULTIFIELDINDEXSEARCHER_H_

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

class MultiFieldIndexSearcher : public IndexSearcher{

	IndexReader* indexReader;

public:

	MultiFieldIndexSearcher(string directory) {
		indexReader = new IndexReader(directory);
	}

	MultiFieldIndexSearcher(IndexReader* ir) {
		indexReader = ir;
	}

//	virtual ~MultiFieldIndexSearcher();

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
		double b = 0; //b should be kept close to 1 to emphasise BM11 formula.
		double K1 = 0.5; //Empirical constant. k1 = 1 works well with real collections
		int N = indexReader->getCollectionSize(); // N = Number of documents in the collection

		Field fields[] = {TITLE, CONTENT, ANCHOR_TEXT, KEYWORDS, DESCRIPTION};
		int fieldWeights[] = {3, 1, 1, 2, 2};

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

			cout << "Searching documents containing term \"" << term->getTerm() << "\"." << endl;

			//1. agregar listas em invertidas de todos os campos em uma, multiplicando pelo peso
			//dos respectivos campos.

			vector<Pair> invertedList;
			for(int i=0; i<5; i++){
				vector<Pair> list = indexReader->getDocFreqs(term, fields[i]); //nao existe ainda
				for(unsigned int j=0; j<list.size(); j++){
					Pair doc_freq(list[j].doc_id, list[j].frequency_dt * fieldWeights[i]);
					invertedList.push_back(doc_freq);
				}
			}

			//2. calcular bm25 na "lista unificada" para cada termo

			//TODO: Calcular a quantidade de documentos real
			int ni = term->getFieldFrequency(CONTENT); // ni = numero de docs que contem termo k_i
			double probabilisticFactor = log( ( N - ni + 0.5 )/(ni + 0.5) );

//			Pair* invertedList = indexReader->getInvertedList(term);

			for(unsigned int i=0; i<invertedList.size(); i++){

				map<int, Hit>::iterator value = accumulators.find(invertedList[i].doc_id);
				Hit accumulator;
				if(value == accumulators.end() ){
					accumulator = accumulators[invertedList[i].doc_id];
					accumulator.doc.id = invertedList[i].doc_id;
					accumulator.length = indexReader->getDocLength(invertedList[i].doc_id);
				}else{
					accumulator = value->second;
				}

				int f_ij = invertedList[i].frequency_dt;
				double docNormalization = accumulator.length / indexReader->getAverageDocLength();
				double tfIdfFactor = ((K1+1) * f_ij) / ( K1 * ((1-b)+(b*docNormalization)) + f_ij );

				accumulator.score += tfIdfFactor * probabilisticFactor;

				accumulators[invertedList[i].doc_id] = accumulator;
			}

			//###
			clock_t fim_term = clock();
			double total = ((double)(fim_term-inicio_term)) / (double) (CLOCKS_PER_SEC/1000);
			printf("Time to search term \"%s\": %.0f ms\n", term->getTerm().c_str(), total);
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
//			result.push_back(topDocs.top());
//			topDocs.pop();
			Hit hit = topDocs.top();
			hit.doc = indexReader->getDoc(hit.doc.id);
			result.push_back(hit);
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

	map<int, Hit> bm25ranking(map<int, Hit> accumulators){
		return accumulators;
	}

};
#endif /* MULTIFIELDINDEXSEARCHER_H_ */
