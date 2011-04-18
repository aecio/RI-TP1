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
#include "OccurrenceFile.h"
#include "InvertedFile.h"
#include "Vocabulary.h"

using namespace std;
	
class IndexWriter {

	static const unsigned int RUN_SIZE = 200000;
	
	string directory;
	Vocabulary vocabulary;
	
	int docIdCounter;
	int termIdCounter;
	
	OccurrenceFile* occurrencesFile;
	
	string extractTextFrom(string& html);
	void printOccurence(Occurrence& it);
	
public:
	IndexWriter(string directory);

	int addDocument(string&);
	void commit();
	
	list<OccurrenceFile*> createRuns();
	OccurrenceFile* merge(list<OccurrenceFile*>&);
	void merge2runs(OccurrenceFile*, OccurrenceFile*, OccurrenceFile*);
	InvertedFile* createInvertedFile(OccurrenceFile* of);
	
};

#endif /* INDEXWRITER_H_ */
