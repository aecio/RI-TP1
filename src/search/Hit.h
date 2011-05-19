/*
 * Hit.h
 *
 *  Created on: 10/05/2011
 *      Author: aecio
 */

#ifndef HIT_H_
#define HIT_H_

#include "index/Doc.h"

class Hit {
public:
	double score;
	Doc doc;

	Hit() {}
	
	Hit(double score_, Doc document){
		score = score_;
		doc = document;
	}
	
	friend bool operator<(const Hit &x, const Hit &y){
		if(x.score < y.score)
			return true;
		else
			return false;
	}
	
	friend bool operator>(const Hit &x, const Hit &y){
		if(x.score > y.score)
			return true;
		else
			return false;
	}
	
};

#endif /* HIT_H_ */
