/*
 * Ocurrence.h
 *
 *  Created on: 22/03/2011
 *      Author: aecio
 */

#ifndef OCCURRENCE_H_
#define OCCURRENCE_H_

class Occurrence {
public:
	int termId;
	int docId;
	int termFrequencyInDoc;

	Occurrence() {}
	
	Occurrence(int termId_, int docId_, int termFrequencyInDoc_){
		termId = termId_;
		docId = docId_;
		termFrequencyInDoc = termFrequencyInDoc_;
	}

	friend bool operator<(const Occurrence &x, const Occurrence &y){
		if(x.termId < y.termId)
			return true;
		else if( (x.termId == y.termId) && (x.docId < y.docId) )
			return true;
		else
			return false;
	}

	friend bool operator>(const Occurrence &x, const Occurrence &y){
		if(x.termId > y.termId)
			return true;
		else if( (x.termId == y.termId) && (x.docId > y.docId) )
			return true;
		else
			return false;
	}
};

#endif /* OCCURRENCE_H_ */
