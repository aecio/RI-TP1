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
#include "util/SequenceFile.h"

using namespace std;

void should_merge_2_sorted_runs_into_one_sorted_file(){
	cout << ">>> should_merge_2_sorted_runs_in_another_sorted_file" << endl;

	//given
	IndexWriter* iw = new IndexWriter("./indice_test/");
	
	SequenceFile<Occurrence> runA("./indice_test/run1");
	SequenceFile<Occurrence> runB("./indice_test/run2");
	SequenceFile<Occurrence> runC("./indice_test/merged");
	
	Occurrence oc1(1, 1, 1);
	Occurrence oc2(2, 2, 2);
	Occurrence oc3(3, 3, 3);
	Occurrence oc4(4, 4, 4);
	Occurrence oc5(5, 5, 5);
	Occurrence oc6(6, 6, 6);
	
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
		assert(o.termId == i);
		assert(o.docId == i);
		assert(o.termFrequencyInDoc == i);
	}
	assert(i == 6);
	assert(runC.getSize() == 6);
}

void should_merge_a_list_of_sorted_runs_into_one_sorted_file(){
	cout << ">>> should_merge_a_list_of_sorted_runs_into_one_sorted_file" << endl;
	
	//given
	IndexWriter* iw = new IndexWriter("./indice_test");
	
	SequenceFile<Occurrence> runA("./indice_test/run1");
	SequenceFile<Occurrence> runB("./indice_test/run2");
	SequenceFile<Occurrence> runC("./indice_test/run3");
	
	Occurrence oc1(1, 1, 1);
	Occurrence oc2(2, 2, 2);
	Occurrence oc3(3, 3, 3);
	Occurrence oc4(4, 4, 4);
	Occurrence oc5(5, 5, 5);
	Occurrence oc6(6, 6, 6);
	Occurrence oc7(7, 7, 7);
	Occurrence oc8(8, 8, 8);
	Occurrence oc9(9, 9, 9);
	Occurrence oc10(10, 10, 10);
	
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
	
	list<SequenceFile<Occurrence>*> list;
	list.push_back(&runA);
	list.push_back(&runB);
	list.push_back(&runC);
	
	//when
	SequenceFile<Occurrence>* merged = iw->merge(list);
	merged->rewind();
	
	//then
	int i=0;
	while( merged->hasNext() ) {
		i++;
		Occurrence o = merged->read();
		assert(o.termId == i);
		assert(o.docId == i);
		assert(o.termFrequencyInDoc == i);
	}
	assert(i == 10);
	assert(merged->getSize() == 10);
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
	
	//term1
	p = index->read();
	assert(p.docId == 1);
	cout << p.frequency_dt << endl; 
	assert(p.frequency_dt == 1);
	
	p = index->read();
	assert(p.docId == 2);
	cout << p.frequency_dt << endl; 
	assert(p.frequency_dt == 1);
	
	p = index->read();
	assert(p.docId == 3);
	cout << p.frequency_dt << endl; 
	assert(p.frequency_dt == 3);
	
	//term2
	p = index->read();
	assert(p.docId == 1);
	assert(p.frequency_dt == 1);
	
	p = index->read();
	assert(p.docId == 2);
	assert(p.frequency_dt == 1);
	
	p = index->read();
	assert(p.docId == 3);
	assert(p.frequency_dt == 1);
	
	//term3
	p = index->read();
	assert(p.docId == 1);
	assert(p.frequency_dt == 1);
	
	//term4
	p = index->read();
	assert(p.docId == 1);
	assert(p.frequency_dt == 1);
	
	p = index->read();
	assert(p.docId == 2);
	assert(p.frequency_dt == 2);
	
}

//when creating inverted file should set the ter pointers in the vocabulary

void indexwriter_test_cases() {
	cout << "-----------------" << endl;
	cout << "IndexWriter Tests" << endl;
	cout << "-----------------" << endl;
	
	should_merge_2_sorted_runs_into_one_sorted_file();
	
	should_merge_a_list_of_sorted_runs_into_one_sorted_file();
	
	should_create_inverted_file_correctly();
	
}

