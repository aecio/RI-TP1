/**
 * Tests the SequenceFile class
 * @author: aeciosantos@dcc.ufmg.br
 */

#include <cstdlib>
#include <iostream>
#include <vector>
#include <assert.h>
#include "index/Occurrence.h"
#include "util/SequenceFile.h"
#include "util/BufferedFile.h"

#define SEQUENCE_TEST_FILE "indice_test/sequencefile.test"

using namespace std;

void should_return_false_if_is_at_last_entry_in_buffer(){
	cout << ">>> should_return_false_if_is_at_last_entry" << endl;
	
	//given
	SequenceFile<Occurrence> sequenceFile(SEQUENCE_TEST_FILE);
	Occurrence oc1(1, 1, 1, 1);
	Occurrence oc2(2, 2, 2, 1);
	Occurrence oc3(3, 3, 3, 1);
	sequenceFile.write(oc1);
	sequenceFile.write(oc2);
	sequenceFile.write(oc3);
	sequenceFile.reopen();
	
	BufferedFile<Occurrence> bf(&sequenceFile, 2);

	//then
	assert(bf.hasNext() == true);
	
	bf.read();
	assert(bf.hasNext() == true);

	bf.read();
	assert(bf.hasNext() == true);
	
	bf.read();
	assert(bf.hasNext() == false);
}


void should_read_the_values_in_the_same_order_of_file(){
	cout << ">>> should_read_the_values_in_the_same_order_of_file" << endl;
	//given
	SequenceFile<Occurrence> sequenceFile(SEQUENCE_TEST_FILE);
	
	for(int i=1; i <= 5; i++) {
		Occurrence oc(i, i, i, i);
		sequenceFile.write(oc);
	}
	sequenceFile.reopen();
	
	BufferedFile<Occurrence> bf(&sequenceFile, 2);

	//then
	int i=0;
	while( bf.hasNext() ) {
		i++;
		Occurrence o = bf.read();
		assert( o.term_id == i );
		assert( o.doc_id == i );
		assert( o.term_frequency == i );
		assert( o.field == i );
	}
	assert( i == 5);
}

void should_read_the_values_in_the_same_order_of_file_passing_buffer_in_constructor(){
	cout << ">>> should_read_the_values_in_the_same_order_of_file_passing_buffer_in_constructor" << endl;
	//given
	SequenceFile<Occurrence> sequenceFile(SEQUENCE_TEST_FILE);

	for(int i=1; i <= 5; i++) {
		Occurrence oc(i, i, i, i);
		sequenceFile.write(oc);
	}
	sequenceFile.reopen();

	int bufferSize = 2;
	Occurrence buffer[bufferSize];
	BufferedFile<Occurrence> bf(&sequenceFile, buffer, bufferSize);

	//then
	int i=0;
	while( bf.hasNext() ) {
		i++;
		Occurrence o = bf.read();
		assert( o.term_id == i );
		assert( o.doc_id == i );
		assert( o.term_frequency == i );
		assert( o.field == i );
	}
	assert( i == 5);
}

void buffered_file_test_cases() {
	cout << "--------------------" << endl;
	cout << "BufferedFile Tests" << endl;
	cout << "--------------------" << endl;
	
	should_return_false_if_is_at_last_entry_in_buffer();
	
	should_read_the_values_in_the_same_order_of_file();

	should_read_the_values_in_the_same_order_of_file_passing_buffer_in_constructor();
}
