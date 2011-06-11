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
#include "index/Field.h"
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
			
			int& id = vocabulary[term.getTerm()];
			id = terms.size();
		}
		sf.close();
	}
	
	int addTerm(string termStr){
		map<string, int>::iterator it = vocabulary.find(termStr);
		if(it == vocabulary.end() ){
			//se não encontrou, adicionar no vector e no map
			Term term(termStr);
			
			terms.push_back(term);
			
			int& id = vocabulary[termStr];
			id = terms.size();
			
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
			Term* p = &terms[it->second-1];
			return p;
		}
	}
	
	void setTermFieldPosition(int termId, int field, int position){
		terms[termId-1].setFieldListPosition(field, position);
	}

	int getTermFieldPosition(int termId, int field){
		return terms[termId-1].getFieldListPosition(field);
	}

	int getTermFieldFrequency(int termId, Field field){
		return terms[termId-1].getFieldFrequency(field);
	}

	void incrementTermFieldFrequency(int termId, int field){
		terms[termId-1].incrementFieldFrequency(field);
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
