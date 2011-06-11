/**
 * Tests the OccurrenceFile class
 * @author: aeciosantos@dcc.ufmg.br
 */

#include <cstdlib>
#include <iostream>
#include <vector>
#include <assert.h>
#include "index/Vocabulary.h"

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
	assert(v.getTermFieldFrequency(id, CONTENT) == 0);
	
	v.incrementTermFieldFrequency(id, CONTENT);
	assert(v.getTermFieldFrequency(id, CONTENT) == 1);
	
	v.incrementTermFieldFrequency(id, CONTENT);
	assert(v.getTermFieldFrequency(id, CONTENT) == 2);
}

void should_set_and_return_correct_list_positions(){
	cout << ">>> should_set_and_return_correct_list_positions" << endl;
	
	//given
	Vocabulary v;
	int id1 = v.addTerm("term1");
	int id2 = v.addTerm("term2");
	
	//when
	v.setTermFieldPosition(id1, CONTENT, 0);
	v.setTermFieldPosition(id2, CONTENT, 10);
	
	//then
	assert(v.getTermFieldPosition(id1, CONTENT) == 0);
	assert(v.getTermFieldPosition(id2, CONTENT) == 10);
}

void should_save_terms_to_a_file(){
	cout << ">>> should_save_terms_to_a_file" << endl;
	
	Vocabulary v;	
	string term1 = "term1";
	string term2 = "term2";
	string term3 = "term3";
	
	int id1 = v.addTerm(term1);
	v.setTermFieldPosition(id1, CONTENT, 10);
	v.incrementTermFieldFrequency(id1, CONTENT);
	assert(id1 == 1);
	assert(v.getTermFieldFrequency(id1, CONTENT) == 1);
	
	int id2 = v.addTerm(term2);
	v.setTermFieldPosition(id2, CONTENT, 20);
	v.incrementTermFieldFrequency(id2, CONTENT);
	v.incrementTermFieldFrequency(id2, CONTENT);
	assert(id2 == 2);
	assert(v.getTermFieldFrequency(id2, CONTENT) == 2);
	
	int id3 = v.addTerm(term3);
	v.setTermFieldPosition(id3, CONTENT, 30);
	v.incrementTermFieldFrequency(id3, CONTENT);
	v.incrementTermFieldFrequency(id3, CONTENT);
	v.incrementTermFieldFrequency(id3, CONTENT);
	assert(id3 == 3);
	assert(v.getTermFieldFrequency(id3, CONTENT) == 3);
	
	//when
	v.saveTo(VOCABULARY_TEST_FILE);
	
	//then
	SequenceFile<Term> sf(VOCABULARY_TEST_FILE, false);
	
	Term t = sf.read();
	assert(term1 == t.getTerm());
	assert(t.getFieldFrequency(CONTENT) == 1);
	assert(t.getFieldListPosition(CONTENT) == 10);
	
	t = sf.read();
	assert(term2 == t.getTerm());
	assert(t.getFieldFrequency(CONTENT) == 2);
	assert(t.getFieldListPosition(CONTENT) == 20);
	
	t = sf.read();
	assert(term3 == t.getTerm());
	assert(t.getFieldFrequency(CONTENT) == 3);
	assert(t.getFieldListPosition(CONTENT) == 30);
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
	assert(term1 == t->getTerm());
	
	t = vocabulary.findTerm(term2);
	assert(term2 == t->getTerm());
	
	t = vocabulary.findTerm(term3);
	assert(term3 == t->getTerm());
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

