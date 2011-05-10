/**
 * Tests the OccurrenceFile class
 * @author: aeciosantos@dcc.ufmg.br
 */

#include <cstdlib>
#include <iostream>
#include <vector>
#include <assert.h>
#include "Vocabulary.h"

#define VOCABULARY_TEST_FILE "indice_test/vocabulary"

using namespace std;

void should_add_and_return_the_id_of_the_term(){
	cout << ">>> should_add_and_return_the_id_of_the_term" << endl;
	
	//given
	Vocabulary v;
	string term1 = "term1";
	string term2 = "term2";
	string term3 = "term3";
	
	//then
	assert( v.addTerm(term1) == 1);
	assert( v.addTerm(term2) == 2);
	assert( v.addTerm(term3) == 3);
}

void should_not_add_same_term_twice(){
	cout << ">>> should_not_add_same_term_twice" << endl;

	//given
	Vocabulary v;	
	string term1 = "term1";
	string term2 = "term2";
	string term1dup = "term1";
	
	//then
	assert( v.addTerm(term1) == 1);
	assert( v.addTerm(term2) == 2);
	assert( v.addTerm(term1) == 1);
	assert( v.addTerm(term2) == 2);
	assert( v.addTerm(term1dup) == 1);
	
}

void should_increment_and_return_frequency_of_the_terms(){
	cout << ">>> should_increment_and_return_frequency_of_the_terms" << endl;
	
	//given
	Vocabulary v;
	string term1 = "term1";
	
	int id = v.addTerm(term1);

	//then
	assert(v.getDocFrequency(id) == 0);
	
	v.incrementDocFrequency(id);
	assert(v.getDocFrequency(id) == 1);
	
	v.incrementDocFrequency(id);
	assert(v.getDocFrequency(id) == 2);
}

void should_set_and_return_correct_list_positions(){
	cout << ">>> should_set_and_return_correct_list_positions" << endl;
	
	//given
	Vocabulary v;
	int id1 = v.addTerm("term1");
	int id2 = v.addTerm("term2");
	
	//when
	v.setListPosition(id1, 0);
	v.setListPosition(id2, 10);
	
	//then
	assert(v.getListPosition(id1) == 0);
	assert(v.getListPosition(id2) == 10);
}

void should_save_terms_to_a_file(){
	cout << ">>> should_save_terms_to_a_file" << endl;
	
	Vocabulary v;	
	string term1 = "term1";
	string term2 = "term2";
	string term3 = "term3";
	
	int id1 = v.addTerm(term1);
	v.setListPosition(id1, 10);
	v.incrementDocFrequency(id1);
	assert(id1 == 1);
	assert(v.getDocFrequency(id1) == 1);
	
	int id2 = v.addTerm(term2);
	v.setListPosition(id2, 20);
	v.incrementDocFrequency(id2);
	v.incrementDocFrequency(id2);
	assert(id2 == 2);
	assert(v.getDocFrequency(id2) == 2);
	
	int id3 = v.addTerm(term3);
	v.setListPosition(id3, 30);
	v.incrementDocFrequency(id3);
	v.incrementDocFrequency(id3);
	v.incrementDocFrequency(id3);
	assert(id3 == 3);
	assert(v.getDocFrequency(id3) == 3);
	
	//when
	v.saveTo(VOCABULARY_TEST_FILE);
	
	//then
	SequenceFile<Term> sf(VOCABULARY_TEST_FILE, false);
	
	Term t = sf.read();
	assert(term1.compare(t.term) == 0);
	assert(t.docFrequency == 1);
	assert(t.listPosition == 10);
	
	t = sf.read();
	assert(term2.compare(t.term) == 0);
	assert(t.docFrequency == 2);
	assert(t.listPosition == 20);
	
	t = sf.read();
	assert(term3.compare(t.term) == 0);
	assert(t.docFrequency == 3);
	assert(t.listPosition == 30);
}

void should_rebuild_vocabulary_correctly(){
	cout << ">>> should_rebuild_vocabulary_correctly" << endl;
	
	Vocabulary v;	
	string term1 = "term1";
	string term2 = "term2";
	string term3 = "term3";
	
	int id1 = v.addTerm(term1);
	assert(id1 == 1);
	
	int id2 = v.addTerm(term2);
	assert(id2 == 2);
	
	int id3 = v.addTerm(term3);
	assert(id3 == 3);
	
	v.saveTo(VOCABULARY_TEST_FILE);
	
	//when
	Vocabulary vocabulary(VOCABULARY_TEST_FILE);
	
	//then
	Term* t;
	t = vocabulary.findTerm(term1);
	assert(term1.compare(t->term) == 0);
	
	t = vocabulary.findTerm(term2);
	assert(term2.compare(t->term) == 0);
	
	t = vocabulary.findTerm(term3);
	assert(term3.compare(t->term) == 0);
}

void vocabulary_test_cases() {
	cout << "--------------------" << endl;
	cout << "Vocabulary Tests" << endl;
	cout << "--------------------" << endl;
	
	should_add_and_return_the_id_of_the_term();
	should_not_add_same_term_twice();
	should_increment_and_return_frequency_of_the_terms();
	should_set_and_return_correct_list_positions();
	should_save_terms_to_a_file();
	should_rebuild_vocabulary_correctly();
}

