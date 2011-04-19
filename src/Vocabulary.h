/*
 * Vocabulary.h
 *
 *  Created on: 17/04/2011
 *      Author: aecio
 */

#ifndef VOCABULARY_H_
#define VOCABULARY_H_

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "Term.h"
#include "SequenceFile.h"

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
			id = vocabulary.size();
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
			return 0;
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
			sf.write(*it);
		}
		sf.close();
	}
		
};

#endif /* VOCABULARY_H_ */
