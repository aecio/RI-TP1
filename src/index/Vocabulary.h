/*
 * Vocabulary.h
 *
 *  Created on: 17/04/2011
 *      Author: aecio
 */

#ifndef VOCABULARY_H_
#define VOCABULARY_H_

#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include "index/Term.h"
#include "util/SequenceFile.h"

using namespace std;

class Vocabulary {
	
	map<string, int> vocabulary;
	vector<Term> terms;
	
public:
	Vocabulary() {}
	
	Vocabulary(string fileToOpen){
		SequenceFile<Term> sf(fileToOpen, false);
		
		while(sf.hasNext()){
			Term term = sf.read();

			terms.push_back(term);
			
			int& id = vocabulary[string(term.term)];
			id = vocabulary.size()-1;
		}
	}
	
	int addTerm(string termStr){
		map<string, int>::iterator it = vocabulary.find(termStr);
		if(it == vocabulary.end() ){
			//se não encontrou, adicionar no vector e no map
			Term term(termStr);
			
			terms.push_back(term);
			
			int& id = vocabulary[termStr];
			id = vocabulary.size();
			
			return id;
		} else {
			return it->second;
		}
	}
	
	Term* findTerm(string termStr){
		map<string, int>::iterator it = vocabulary.find(termStr);
		if(it == vocabulary.end() ){
			return NULL;
		} else {
			Term* p = &terms[it->second];
			return p;
		}
	}
	
	void setListPosition(int termId, int position){
		terms[termId-1].listPosition = position;
	}
	
	int getListPosition(int termId){
		return terms[termId-1].listPosition;
	}
	
	void incrementDocFrequency(int termId){
		terms[termId-1].docFrequency++;
	}
	
	int getDocFrequency(int termId){
		return terms[termId-1].docFrequency;
	}
	
	void saveTo(string fileName){
		SequenceFile<Term> sf(fileName);
		vector<Term>::iterator it = terms.begin();
		for(; it != terms.end(); it++){
//			cout << it->term << endl;
			sf.write(*it);
		}
		sf.close();
	}
		
};

#endif /* VOCABULARY_H_ */