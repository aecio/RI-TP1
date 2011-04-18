/*
 * IndexWriter.h
 *
 *  Created on: 13/04/2011
 *      Author: aecio
 */

#ifndef INDEXWRITER_H_
#define INDEXWRITER_H_

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <list>
#include "Occurrence.h"
#include "SequenceFile.h"
#include "Vocabulary.h"
#include "Pair.h"

using namespace std;
	
class IndexWriter {

	static const unsigned int RUN_SIZE = 200000;
	
	string directory;
	Vocabulary vocabulary;
	
	int docIdCounter;
	int termIdCounter;
	
	SequenceFile<Occurrence>* occurrencesFile;
	
	string extractTextFrom(string& html);
	void printOccurence(Occurrence& it);
	
public:
	IndexWriter(string directory);

	int addDocument(string&);
	void commit();
	
	list<SequenceFile<Occurrence>*> createRuns();
	SequenceFile<Occurrence>* merge(list<SequenceFile<Occurrence>*>&);
	void merge2runs(SequenceFile<Occurrence>*, SequenceFile<Occurrence>*, SequenceFile<Occurrence>*);
	 SequenceFile<Pair>* createInvertedFile(SequenceFile<Occurrence>* of);
	
};

#endif /* INDEXWRITER_H_ */
