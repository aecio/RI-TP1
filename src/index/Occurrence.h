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
	int term_id;
	int doc_id;
	int term_frequency;
	int field;

	Occurrence() {}
	
	Occurrence(int termId_, int docId_, int termFrequencyInDoc_, int field_){
		term_id = termId_;
		doc_id = docId_;
		term_frequency = termFrequencyInDoc_;
		field = field_;
	}

	friend bool operator<(const Occurrence &x, const Occurrence &y){
		if(x.term_id < y.term_id){
			return true;
		} else if( x.term_id == y.term_id) {
			if(x.field < y.field) {
				return true;
			} else if(x.field == y.field){
				if(x.doc_id < y.doc_id){
					return true;
				}
			}
		}
		return false;
	}

	friend bool operator>(const Occurrence &x, const Occurrence &y){
		if(x.term_id > y.term_id){
			return true;
		} else if( x.term_id == y.term_id) {
			if(x.field > y.field) {
				return true;
			} else if(x.field == y.field){
				if(x.doc_id > y.doc_id){
					return true;
				}
			}
		}
		return false;
	}
};

#endif /* OCCURRENCE_H_ */
