/**
 * Tests the IndexWriter class
 * @author: aeciosantos@dcc.ufmg.br
 */

#include <assert.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "index/IndexWriter.h"
#include "index/Occurrence.h"
#include "index/Pair.h"
#include "index/Field.h"
#include "util/SequenceFile.h"

using namespace std;

void should_merge_2_sorted_runs_into_one_sorted_file(){
	cout << ">>> should_merge_2_sorted_runs_in_another_sorted_file" << endl;

	//given
	IndexWriter* iw = new IndexWriter("./indice_test/");
	
	SequenceFile<Occurrence> runA("./indice_test/run1");
	SequenceFile<Occurrence> runB("./indice_test/run2");
	SequenceFile<Occurrence> runC("./indice_test/merged");
	
	Occurrence oc1(1, 1, 1, 1);
	Occurrence oc2(2, 2, 2, 1);
	Occurrence oc3(3, 3, 3, 1);
	Occurrence oc4(4, 4, 4, 1);
	Occurrence oc5(5, 5, 5, 1);
	Occurrence oc6(6, 6, 6, 1);
	
	runA.write(oc1);
	runA.write(oc3);
	runA.write(oc5);
	
	runB.write(oc2);
	runB.write(oc4);
	runB.write(oc6);
	
	//when
	iw->merge2runs(&runA, &runB, &runC);
	runC.close();
	runC.reopen();
	
	//then
	int i=0;
	while( runC.hasNext() ) {
		i++;
		Occurrence o = runC.read();
		assert(o.term_id == i);
		assert(o.doc_id == i);
		assert(o.term_frequency == i);
	}
	assert(i == 6);
	assert(runC.getSize() == 6);
}

void should_merge_a_vector_of_sorted_runs_into_one_sorted_file(){
	cout << ">>> should_merge_a_vector_of_sorted_runs_into_one_sorted_file" << endl;
	
	//given
	IndexWriter* iw = new IndexWriter("./indice_test");
	
	SequenceFile<Occurrence> runA("./indice_test/run1");
	SequenceFile<Occurrence> runB("./indice_test/run2");
	SequenceFile<Occurrence> runC("./indice_test/run3");
	
	Occurrence oc1(1, 1, 1, 1);
	Occurrence oc2(2, 2, 2, 1);
	Occurrence oc3(3, 3, 3, 1);
	Occurrence oc4(4, 4, 4, 1);
	Occurrence oc5(5, 5, 5, 1);
	Occurrence oc6(6, 6, 6, 1);
	Occurrence oc7(7, 7, 7, 1);
	Occurrence oc8(8, 8, 8, 1);
	Occurrence oc9(9, 9, 9, 1);
	Occurrence oc10(10, 10, 10, 1);
	
	runA.write(oc1);
	runA.write(oc4);
	runA.write(oc5);
	runA.write(oc8);
	
	runB.write(oc2);
	runB.write(oc6);
	runB.write(oc7);
	runB.write(oc10);
	
	runC.write(oc3);
	runC.write(oc9);
	
	vector<SequenceFile<Occurrence>*> runs;
	runs.push_back(&runA);
	runs.push_back(&runB);
	runs.push_back(&runC);
	
	//when
	SequenceFile<Occurrence>* merged = iw->merge(runs);
	merged->rewind();
	
	//then
	int i=0;
	while( merged->hasNext() ) {
		i++;
		Occurrence o = merged->read();
		assert(o.term_id == i);
		assert(o.doc_id == i);
		assert(o.term_frequency == i);
	}
	assert(i == 10);
	assert(merged->getSize() == 10);
}


void should_merge_a_vector_of_sorted_runs_into_one_sorted_file_using_kwaymerge(){
	cout << ">>> should_merge_a_vector_of_sorted_runs_into_one_sorted_file_using_kwaymerge" << endl;

	//given
	IndexWriter* iw = new IndexWriter("./indice_test");

	SequenceFile<Occurrence> runA("./indice_test/run1", true);
	SequenceFile<Occurrence> runB("./indice_test/run2", true);
	SequenceFile<Occurrence> runC("./indice_test/run3", true);

	Occurrence oc1(1, 1, 1, 1);
	Occurrence oc2(2, 2, 2, 1);
	Occurrence oc3(3, 3, 3, 1);
	Occurrence oc4(4, 4, 4, 1);
	Occurrence oc5(5, 5, 5, 1);
	Occurrence oc6(6, 6, 6, 1);
	Occurrence oc7(7, 7, 7, 1);
	Occurrence oc8(8, 8, 8, 1);
	Occurrence oc9(9, 9, 9, 1);
	Occurrence oc10(10, 10, 10, 1);

	runA.write(oc1);
	runA.write(oc4);
	runA.write(oc5);
	runA.write(oc8);

	runB.write(oc2);
	runB.write(oc6);
	runB.write(oc7);
	runB.write(oc10);

	runC.write(oc3);
	runC.write(oc9);

	vector<SequenceFile<Occurrence>*> runs;
	runs.push_back(&runA);
	runs.push_back(&runB);
	runs.push_back(&runC);

	//when
	SequenceFile<Occurrence>* merged = iw->kwaymerge(runs);
	merged->rewind();

	//then
	int i=0;
	while( merged->hasNext() ) {
		i++;
		Occurrence o = merged->read();
		assert(o.term_id == i);
		assert(o.doc_id == i);
		assert(o.term_frequency == i);
	}
	assert(i == 10);
	assert(merged->getSize() == 10);
}

void should_merge_runs_with_runsize_smaller_than_occurrences(){
	cout << ">>> should_merge_runs_with_runsize_smaller_than_occurrences" << endl;

	//given
	IndexWriter* iw = new IndexWriter("./indice_test", 5); //runSize = 5

	SequenceFile<Occurrence> runA("./indice_test/run1", true);
	SequenceFile<Occurrence> runB("./indice_test/run2", true);
	SequenceFile<Occurrence> runC("./indice_test/run3", true);

	int n = 0;
	while(n<20){
		Occurrence o1(n, n, n, 1);
		runA.write(o1);
		n++;

		Occurrence o2(n, n, n, 1);
		runB.write(o2);
		n++;

		Occurrence o3(n, n, n, 1);
		runC.write(o3);
		n++;
	}

	vector<SequenceFile<Occurrence>*> runs;
	runs.push_back(&runA);
	runs.push_back(&runB);
	runs.push_back(&runC);

	//when
	SequenceFile<Occurrence>* merged = iw->kwaymerge(runs);
	merged->rewind();

	//then
	int i = 0;
	while( merged->hasNext() ) {
		Occurrence o = merged->read();
		assert(o.term_id == i);
		assert(o.doc_id == i);
		assert(o.term_frequency == i);
		i++;
	}
	assert(i == n);
	assert(merged->getSize() == n);
}

void should_create_inverted_file_correctly(){
	cout << ">>> should_create_inverted_file_correctly" << endl;
	
	//given
	IndexWriter* iw = new IndexWriter("./indice_test/");
	
	Page doc1("", "termo1 termo2 termo3 termo4");
	Page doc2("", "termo1 termo2 termo4 termo4");
	Page doc3("", "termo1 termo2 termo1 termo1");
	
	iw->addDocument(doc1);
	iw->addDocument(doc2);
	iw->addDocument(doc3);

	Pair p;
	
	//when
	iw->commit();
	
	//then
	SequenceFile<Pair>* index = new SequenceFile<Pair>("./indice_test/index", false);
	Vocabulary vocabulary("./indice_test/vocabulary");
	
	//term1
	assert( vocabulary.getTermFieldPosition(1, CONTENT) == 0);
	assert( vocabulary.getTermFieldFrequency(1, CONTENT) == 3);

	p = index->read();
	assert(p.doc_id == 1);
	assert(p.frequency_dt == 1);
	
	p = index->read();
	assert(p.doc_id == 2);
	assert(p.frequency_dt == 1);
	
	p = index->read();
	assert(p.doc_id == 3);
	assert(p.frequency_dt == 3);
	
	//term2
	assert( vocabulary.getTermFieldPosition(2, CONTENT) == 3);
	assert( vocabulary.getTermFieldFrequency(2, CONTENT) == 3);

	p = index->read();
	assert(p.doc_id == 1);
	assert(p.frequency_dt == 1);
	
	p = index->read();
	assert(p.doc_id == 2);
	assert(p.frequency_dt == 1);
	
	p = index->read();
	assert(p.doc_id == 3);
	assert(p.frequency_dt == 1);
	
	//term3
	assert( vocabulary.getTermFieldPosition(3, CONTENT) == 6);
	assert( vocabulary.getTermFieldFrequency(3, CONTENT) == 1);

	p = index->read();
	assert(p.doc_id == 1);
	assert(p.frequency_dt == 1);
	
	//term4
	assert( vocabulary.getTermFieldPosition(4, CONTENT) == 7);
	assert( vocabulary.getTermFieldFrequency(4, CONTENT) == 2);

	p = index->read();
	assert(p.doc_id == 1);
	assert(p.frequency_dt == 1);
	
	p = index->read();
	assert(p.doc_id == 2);
	assert(p.frequency_dt == 2);
	
}

//when creating inverted file should set the ter pointers in the vocabulary

void indexwriter_test_cases() {
	cout << "-----------------" << endl;
	cout << "IndexWriter Tests" << endl;
	cout << "-----------------" << endl;
	
	should_merge_2_sorted_runs_into_one_sorted_file();
	
	should_merge_a_vector_of_sorted_runs_into_one_sorted_file();
	
	should_merge_a_vector_of_sorted_runs_into_one_sorted_file_using_kwaymerge();

	should_merge_runs_with_runsize_smaller_than_occurrences();

	should_create_inverted_file_correctly();
	
}

