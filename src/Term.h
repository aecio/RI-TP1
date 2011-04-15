/*
 * Term.h
 *
 *  Created on: 22/03/2011
 *      Author: aecio
 */

#ifndef TERM_H_
#define TERM_H_

#include <fstream>
#include <list>
#include "Occurrence.h"

using namespace std;

class Term {
	int id;
	int termFrequency;	// f_t - total frequency of the term (count documents containing this term)
	streampos listPointer;
public:
	Term();
	Term(int id, int termFrequency);
	
	int getId();
	void setId(int id);
	
	int getTermFrequency();
	void setTermFrequency(int termFrequency);
	void incrementTermFrequency();
	
	streampos getInvertedListPointer();
	void setInvertedListPointer(streampos);
};

#endif /* TERM_H_ */
