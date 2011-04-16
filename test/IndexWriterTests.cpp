/**
 * Tests the IndexWriter class
 * @author: aeciosantos@dcc.ufmg.br
 */

#include <cstdlib>
#include <iostream>
#include <vector>
#include <assert.h>
#include "Occurrence.h"
#include "OccurrenceFile.h"
#include "IndexWriter.h"

using namespace std;

void should_merge_2_sorted_runs_into_one_sorted_file(){
	cout << "> should_merge_2_sorted_runs_in_another_sorted_file" << endl;

	//given
	IndexWriter* iw = new IndexWriter("directory");
	
	OccurrenceFile runA("run1");
	OccurrenceFile runB("run2");
	OccurrenceFile runC("merged");
	
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
	cout << "> should_merge_a_list_of_sorted_runs_into_one_sorted_file" << endl;
	
	//given
	IndexWriter* iw = new IndexWriter("directory");
	
	OccurrenceFile runA("run1");
	OccurrenceFile runB("run2");
	OccurrenceFile runC("run3");
	
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
	
	list<OccurrenceFile*> list;
	list.push_back(&runA);
	list.push_back(&runB);
	list.push_back(&runC);
	
	//when
	OccurrenceFile* merged = iw->merge(list);
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

void indexwriter_test_cases() {
	cout << "-----------------" << endl;
	cout << "IndexWriter Tests" << endl;
	cout << "-----------------" << endl;
	
	should_merge_2_sorted_runs_into_one_sorted_file();
	
	should_merge_a_list_of_sorted_runs_into_one_sorted_file();
	
}

