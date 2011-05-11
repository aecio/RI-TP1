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
#include <queue>
#include "SequenceFile.h"
#include "Pair.h"
#include "Vocabulary.h"
#include "TextTokenizer.h"
#include "Hit.h"

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
	
	vector<Doc> search(string query) {
		TextTokenizer tokenizer(query);

		vector<string> tokens;
		while(tokenizer.hasNext()) {
			tokens.push_back(tokenizer.nextToken());
		}
		
		map<int, double> accumulators;
		int numDocs = pagesFile->getSize();
		
		vector<string>::iterator it = tokens.begin();
		for(;it != tokens.end(); it++){
			Term* term = vocabulary->findTerm(*it);
			//TODO: se não encontar o termo? (term == NULL)

			double wt = 1 + log( numDocs/term->docFrequency );
			
			Pair* invertedList = readInvertedList(term);
			
			for(int i=0; i<term->docFrequency; i++){
				double valor;
				map<int, double>::iterator value = accumulators.find(invertedList[i].docId);
				if(value == accumulators.end() ){
					accumulators[invertedList[i].docId] = 0;
					valor = accumulators[invertedList[i].docId];
				}else{
					valor = value->second;
				}
				valor += log(1+ invertedList[i].frequency_dt) * wt;
				accumulators[invertedList[i].docId] = valor;
				cout << valor << endl;
			}
		}
		
		
		//pra cada acumulador, normalizar score pelo tamanho do documento
		
		priority_queue< Hit, vector<Hit>, greater<Hit> > topDocs;
		
		map<int, double>::iterator ac = accumulators.begin();
		for(int i=0; i<10 && ac != accumulators.end(); i++){
			int docId = ac->first;
			pagesFile->setPosition(docId-1);
			Doc doc = pagesFile->read();
			
			ac->second = ac->second / doc.length;
			
			Hit hit(ac->second, doc);
//			cout << "score: " << hit.score << " url: " << hit.doc.url << endl;
			topDocs.push(hit);
			
			ac++;
		}
		
//		cout << endl << "demais docs..." << endl;
		
		for(; ac != accumulators.end(); ac++){
			int docId = ac->first;
			pagesFile->setPosition(docId-1);
			Doc doc = pagesFile->read();
			
			ac->second = ac->second / doc.length;
			
			Hit hit(ac->second, doc);
			cout << "score: " << hit.score << " url: " << hit.doc.url << endl;
			if(topDocs.top() > hit){
				topDocs.pop();
				topDocs.push(hit);
			}
		}
		
		cout << "TopDocs" << endl;
		vector<Doc> result;
		while( !topDocs.empty() ){
			Hit hit = topDocs.top();
//			cout << "score: " << hit.score << " url: " << hit.doc.url << endl;
			result.push_back(hit.doc);
			topDocs.pop();
		}
		reverse(result.begin(), result.end());
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
