/*
 * Term.cpp
 *
 *  Created on: 22/03/2011
 *      Author: aecio
 */

#include "Term.h"

Term::Term() {
	termFrequency = 0;
}

Term::Term(int term_id, int term_frequency) {
	id = term_id;
	termFrequency = term_frequency;
}

int Term::getId(){
	return id;
}
void Term::setId(int id_){
	id = id_;
}

int Term::getTermFrequency(){
	return termFrequency;
}
void Term::setTermFrequency(int termFrequency_){
	termFrequency = termFrequency_;
}
void Term::incrementTermFrequency(){
	termFrequency++;
}

streampos Term::getInvertedListPointer(){
	return listPointer;
}
void Term::setInvertedListPointer(streampos position){
	listPointer = position;
}
