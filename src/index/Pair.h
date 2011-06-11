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
	int doc_id;
	int frequency_dt;

	Pair() {}
	
	Pair(int docId_, int frequency_dt_){
		doc_id = docId_;
		frequency_dt = frequency_dt_;
	}
	
	friend bool operator<(const Pair &x, const Pair &y){
		if(x.doc_id < y.doc_id)
			return true;
		else
			return false;
	}
	
};

#endif /* PAIR_H_ */
