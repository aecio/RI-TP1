/**
 * Tests the OccurrenceFile class
 * @author: aeciosantos@dcc.ufmg.br
 */

#include <cstdlib>
#include <iostream>
#include <vector>
#include <assert.h>
#include "Occurrence.h"
#include "OccurrenceFile.h"

#define TEST_FILE "occurrences_test"

using namespace std;

void should_write_occurrence_and_return_right_position(){
	cout << ">>> should_write_occurrence_and_return_right_position" << endl;
	OccurrenceFile of(TEST_FILE);
	for(int i=1; i <= 5; i++) {
		Occurrence oc(i, i, i);
		int position = of.write(oc);
		assert( position == i);
	}
}

void should_return_position_to_zero_when_rewind_is_called(){
	cout << ">>> should_return_position_to_zero_when_rewind_is_called" << endl;
	
	//given
	OccurrenceFile of(TEST_FILE);
		
	Occurrence oc1(1, 1, 1);
	of.write(oc1);
	assert(of.getPosition() == 1);
	
	Occurrence oc2(2, 2, 2);
	of.write(oc2);
	assert(of.getPosition() == 2);
	
	
	//when
	of.rewind();

	//then	
	assert( of.getPosition() == 0 );
}

void should_save_occurrence_fields_values_correctly(){
	cout << ">>> should_save_occurrence_fields_values_correctly" << endl;
	
	//given
	OccurrenceFile of(TEST_FILE);
	Occurrence occurrenceToBeSaved(15, 15, 15);

	//when
	of.write(occurrenceToBeSaved);
	
	//then
	of.rewind();
	Occurrence occurrenceRetrieved = of.read();
	
	assert(occurrenceRetrieved.termId == 15);
	assert(occurrenceRetrieved.termFrequencyInDoc == 15);
	assert(occurrenceRetrieved.docId == 15);
}

void should_return_false_if_is_at_last_entry(){
	cout << ">>> should_return_false_if_is_at_last_entry" << endl;
	
	//given
	OccurrenceFile of(TEST_FILE);
	Occurrence oc1(1, 1, 1);
	Occurrence oc2(2, 2, 2);
	of.write(oc1);
	of.write(oc2);
	of.rewind();
	
	//then
	assert(of.hasNext() == true);
	
	of.read();
	assert(of.hasNext() == true);
	
	of.read();
	assert(of.hasNext() == false);
	
	of.rewind();
	assert(of.hasNext() == true);
}


void should_write_and_read_the_same_values(){
	cout << ">>> should_write_and_read_the_same_values" << endl;
	
	OccurrenceFile of(TEST_FILE);
	
	for(int i=1; i <= 5; i++) {
		Occurrence oc(i, i, i);
		of.write(oc);
	}
	
	int i=1;
	while( of.hasNext() ) {
		Occurrence o = of.read();
		assert( o.termId == i );
		assert( o.docId == i );
		assert( o.termFrequencyInDoc == i );
		i++;
	}
}

void should_read_a_block_of_occurrences(){
	cout << ">>> should_read_a_block_of_occurrences" << endl;
	
	//given
	OccurrenceFile of(TEST_FILE);
	Occurrence oc1(1, 1, 1);
	Occurrence oc2(2, 2, 2);
	Occurrence oc3(3, 3, 3);
	of.write(oc1);
	of.write(oc2);
	of.write(oc3);
	of.rewind();
	Occurrence occurs[3];
	
	//when
	int size = of.readBlock(occurs, 3);
	assert(size == 3);
	
	for(int i=0; i<size; i++){
		assert(occurs[i].termId == i+1);
	}
}

void should_write_a_block_of_occurrences(){
	cout << ">>> should_write_a_block_of_occurrences" << endl;
	
	//given
	OccurrenceFile of(TEST_FILE);
	Occurrence oc1(1, 1, 1);
	Occurrence oc2(2, 2, 2);
	Occurrence oc3(3, 3, 3);

	Occurrence occurs[] = {oc1, oc2, oc3};
	of.writeBlock(occurs, 3);
	of.rewind();
	
	//when
	int i=0;
	while( of.hasNext() ){
		i++;
		Occurrence o = of.read();
		assert( o.termId == i );
		assert( o.docId == i );
		assert( o.termFrequencyInDoc == i );
	}
	assert(i == 3);
	assert(of.getSize() == 3);
}

void should_return_correct_size_after_reopen(){
	cout << ">>> should_return_correct_size_after_reopen" << endl;
	//given
	OccurrenceFile of(TEST_FILE);
	Occurrence oc1(1, 1, 1);
	Occurrence oc2(2, 2, 2);
	of.write(oc1);
	of.write(oc2);
	
	//when
	of.reopen();
	
	//then
	assert(of.getSize() == 2);
	assert(of.getPosition() == 0);
}

void occurence_file_test_cases() {
	cout << "--------------------" << endl;
	cout << "OccurrenceFile Tests" << endl;
	cout << "--------------------" << endl;
	
	should_write_occurrence_and_return_right_position();
	
	should_return_position_to_zero_when_rewind_is_called();
	
	should_save_occurrence_fields_values_correctly();
	
	should_return_false_if_is_at_last_entry();
	
	should_write_and_read_the_same_values();
	
	should_read_a_block_of_occurrences();
	
	should_write_a_block_of_occurrences();

	should_return_correct_size_after_reopen();
}

