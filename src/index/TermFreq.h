/*
 * TermFreq.h
 *
 *  Created on: 13/06/2011
 *      Author: aecio
 */

#ifndef TERMFREQ_H_
#define TERMFREQ_H_

class TermFreq {
	int term_id;
	int frequency;
public:

	TermFreq(): term_id(0), frequency(0) {}
	TermFreq(int termId, int freq): term_id(termId), frequency(freq) {}

	int getTermId(){
		return term_id;
	}

	int getFrequency(){
		return frequency;
	}
};

#endif /* TERMFREQ_H_ */
