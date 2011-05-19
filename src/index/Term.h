/*
 * Term.h
 *
 *  Created on: 22/03/2011
 *      Author: aecio
 */

#ifndef TERM_H_
#define TERM_H_

#include <string>

using namespace std;

class Term {
public:
	char term[30];
	int docFrequency;	// f_t - total frequency of the term (count documents containing this term)
	int listPosition;
	
	Term() {
		docFrequency = 0;
	}
	
	Term(string termStr){
		int size;
		if(termStr.size() < 30){
			size = termStr.size();
		} else {
			size = 30;
		}
		termStr.copy(term, size);
		term[size] = '\0';
		docFrequency = 0;
		listPosition = 0;
	}
	
};

#endif /* TERM_H_ */
