/**
 * Tests the IndexSearcher class
 * @author: aeciosantos@dcc.ufmg.br
 */

#include <vector>
#include <assert.h>
#include <iostream>
#include "IndexSearcher.h"

using namespace std;

void should_return_instersection_set(){
	cout << ">>> should_return_instersection_set" << endl;
	//given
	IndexSearcher* is = new IndexSearcher(".");
	Pair first[] = {Pair(1,1), Pair(2,2), Pair(3,3), Pair(4,4)};
	Pair second[] = {Pair(2,2), Pair(4,4)};

	//when
	vector<Pair> v = is->intersectionSet(first, 4, second, 2);

	//then
	assert(v.size() == 2);
	assert(v[0].docId == 2);
	assert(v[1].docId == 4);
}

void should_return_union_set(){
	cout << ">>> should_return_union_set" << endl;
	
	//given
	IndexSearcher* is = new IndexSearcher(".");
	Pair first[] = {Pair(1,1), Pair(3,3), Pair(4,4)};
	Pair second[] = {Pair(2,2), Pair(4,4)};

	//when
	vector<Pair> v = is->unionSet(first, 3, second, 2);

	//then
	assert(v.size() == 4);
	assert(v[0].docId == 1);
	assert(v[1].docId == 2);
	assert(v[2].docId == 3);
	assert(v[3].docId == 4);
}

void setUp(){
	ofstream vocabulary("vocabulary");
	ofstream index("index");
	vocabulary.close();
	index.close();
}

void tearDown(){
	remove("vocabulary");
	remove("index");
}

void indexsearcher_test_cases() {
	cout << "--------------------" << endl;
	cout << "IndexSearcher Tests" << endl;
	cout << "--------------------" << endl;
	
	setUp();
	
	should_return_instersection_set();
	
	should_return_union_set();
	
	tearDown();
}

