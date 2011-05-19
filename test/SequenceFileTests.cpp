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

#define SEQUENCE_TEST_FILE "indice_test/sequencefile.test"

using namespace std;

void should_write_a_type_and_return_right_position(){
	cout << ">>> should_write_a_type_and_return_right_position" << endl;
	
	SequenceFile<Occurrence> sf(SEQUENCE_TEST_FILE);
	Occurrence oc1(1, 1, 1);
	Occurrence oc2(2, 2, 2);
	Occurrence oc3(3, 3, 3);
	Occurrence oc4(4, 4, 4);
	
	assert(1 == sf.write(oc1));
	assert(2 == sf.write(oc2));
	assert(3 == sf.write(oc3));	
	assert(4 == sf.write(oc4));	
}

void should_return_position_to_zero_when_rewind_is_called(){
	cout << ">>> should_return_position_to_zero_when_rewind_is_called" << endl;
	
	//given
	SequenceFile<Occurrence> sequenceFile(SEQUENCE_TEST_FILE);
		
	Occurrence oc1(1, 1, 1);
	sequenceFile.write(oc1);
	assert(sequenceFile.getPosition() == 1);
	
	Occurrence oc2(2, 2, 2);
	sequenceFile.write(oc2);
	assert(sequenceFile.getPosition() == 2);
	
	
	//when
	sequenceFile.rewind();

	//then	
	assert( sequenceFile.getPosition() == 0 );
}

void should_save_types_fields_values_correctly(){
	cout << ">>> should_save_types_fields_values_correctly" << endl;
	
	//given
	SequenceFile<Occurrence> sequenceFile(SEQUENCE_TEST_FILE);
	Occurrence occurrenceToBeSaved(15, 15, 15);

	//when
	sequenceFile.write(occurrenceToBeSaved);
	
	//then
	sequenceFile.rewind();
	Occurrence occurrenceRetrieved = sequenceFile.read();
	
	assert(occurrenceRetrieved.termId == 15);
	assert(occurrenceRetrieved.termFrequencyInDoc == 15);
	assert(occurrenceRetrieved.docId == 15);
}

void should_return_false_if_is_at_last_entry(){
	cout << ">>> should_return_false_if_is_at_last_entry" << endl;
	
	//given
	SequenceFile<Occurrence> sequenceFile(SEQUENCE_TEST_FILE);
	Occurrence oc1(1, 1, 1);
	Occurrence oc2(2, 2, 2);
	sequenceFile.write(oc1);
	sequenceFile.write(oc2);
	sequenceFile.rewind();
	
	//then
	assert(sequenceFile.hasNext() == true);
	
	sequenceFile.read();
	assert(sequenceFile.hasNext() == true);
	
	sequenceFile.read();
	assert(sequenceFile.hasNext() == false);
	
	sequenceFile.rewind();
	assert(sequenceFile.hasNext() == true);
}


void should_write_and_read_the_same_values(){
	cout << ">>> should_write_and_read_the_same_values" << endl;
	
	SequenceFile<Occurrence> sequenceFile(SEQUENCE_TEST_FILE);
	
	for(int i=1; i <= 5; i++) {
		Occurrence oc(i, i, i);
		sequenceFile.write(oc);
	}
	
	int i=1;
	while( sequenceFile.hasNext() ) {
		Occurrence o = sequenceFile.read();
		assert( o.termId == i );
		assert( o.docId == i );
		assert( o.termFrequencyInDoc == i );
		i++;
	}
}

void should_read_a_block_of_objects(){
	cout << ">>> should_read_a_block_of_objects" << endl;
	
	//given
	SequenceFile<Occurrence> sequenceFile(SEQUENCE_TEST_FILE);
	Occurrence oc1(1, 1, 1);
	Occurrence oc2(2, 2, 2);
	Occurrence oc3(3, 3, 3);
	sequenceFile.write(oc1);
	sequenceFile.write(oc2);
	sequenceFile.write(oc3);
	sequenceFile.rewind();
	Occurrence occurs[3];
	
	//when
	int size = sequenceFile.readBlock(occurs, 3);
	assert(size == 3);
	
	for(int i=0; i<size; i++){
		assert(occurs[i].termId == i+1);
	}
}

void should_write_a_block_of_objects(){
	cout << ">>> should_write_a_block_of_objects" << endl;
	
	//given
	SequenceFile<Occurrence> sequenceFile(SEQUENCE_TEST_FILE);
	Occurrence oc1(1, 1, 1);
	Occurrence oc2(2, 2, 2);
	Occurrence oc3(3, 3, 3);

	Occurrence occurs[] = {oc1, oc2, oc3};
	sequenceFile.writeBlock(occurs, 3);
	sequenceFile.rewind();
	
	//when
	int i=0;
	while( sequenceFile.hasNext() ){
		i++;
		Occurrence o = sequenceFile.read();
		assert( o.termId == i );
		assert( o.docId == i );
		assert( o.termFrequencyInDoc == i );
	}
	assert(i == 3);
	assert(sequenceFile.getSize() == 3);
}

void should_return_correct_size_after_reopen(){
	cout << ">>> should_return_correct_size_after_reopen" << endl;
	//given
	SequenceFile<Occurrence> sequenceFile(SEQUENCE_TEST_FILE);
	Occurrence oc1(1, 1, 1);
	Occurrence oc2(2, 2, 2);
	sequenceFile.write(oc1);
	sequenceFile.write(oc2);
	
	//when
	sequenceFile.reopen();
	
	//then
	assert(sequenceFile.getSize() == 2);
	assert(sequenceFile.getPosition() == 0);
}

void should_set_the_position_in_the_file(){
	cout << ">>> should_set_the_position_in_the_file" << endl;
	//given
	SequenceFile<Occurrence> sequenceFile(SEQUENCE_TEST_FILE);
	Occurrence oc1(0, 0, 0);
	Occurrence oc2(1, 1, 1);
	Occurrence oc3(2, 2, 2);
	sequenceFile.write(oc1);
	sequenceFile.write(oc2);
	sequenceFile.write(oc3);
	Occurrence o;
	
	//when
	sequenceFile.setPosition(1);
	//then
	o = sequenceFile.read();
	assert(o.termId == 1);
	assert(o.termFrequencyInDoc == 1);
	assert(o.docId == 1);
	
	//when
	sequenceFile.setPosition(0);
	//then
	o = sequenceFile.read();
	assert(o.termId == 0);
	assert(o.termFrequencyInDoc == 0);
	assert(o.docId == 0);

	//when
	sequenceFile.setPosition(2);
	//then
	o = sequenceFile.read();
	assert(o.termId == 2);
	assert(o.termFrequencyInDoc == 2);
	assert(o.docId == 2);
}

void sequence_file_test_cases() {
	cout << "--------------------" << endl;
	cout << "SequenceFile Tests" << endl;
	cout << "--------------------" << endl;
	
	should_write_a_type_and_return_right_position();
	
	should_return_position_to_zero_when_rewind_is_called();
	
	should_save_types_fields_values_correctly();
	
	should_return_false_if_is_at_last_entry();
	
	should_write_and_read_the_same_values();
	
	should_read_a_block_of_objects();
	
	should_write_a_block_of_objects();

	should_return_correct_size_after_reopen();
	
	should_set_the_position_in_the_file();
}

