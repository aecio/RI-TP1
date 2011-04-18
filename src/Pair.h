/*
 * Pair.h
 *
 *  Created on: 17/04/2011
 *      Author: aecio
 */

#ifndef PAIR_H_
#define PAIR_H_

class Pair {
public:
	int docId;
	int frequency_dt;

	Pair() {}
	
	Pair(int docId_, int frequency_dt_){
		docId = docId_;
		frequency_dt = frequency_dt_;
	}
};

#endif /* PAIR_H_ */
