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
#include "index/Occurrence.h"
#include "index/Vocabulary.h"
#include "index/Pair.h"
#include "index/Doc.h"
#include "util/SequenceFile.h"
#include "util/Page.h"

using namespace std;
	
class IndexWriter {

	int runSize;
	
	string directory;
	Vocabulary vocabulary;
	
	int docIdCounter;
	int termIdCounter;
	
	SequenceFile<Occurrence>* occurrencesFile;
	SequenceFile<Doc>* pagesFile;
	
	string extractTextFrom(string& html);
	void printOccurence(Occurrence& it);
	
public:
	IndexWriter(string directory, int runSize = 500000);

	int addDocument(Page& page);
	void commit();
	
	list<SequenceFile<Occurrence>*> createRuns();
	SequenceFile<Occurrence>* merge(list<SequenceFile<Occurrence>*>&);
	void merge2runs(SequenceFile<Occurrence>*, SequenceFile<Occurrence>*, SequenceFile<Occurrence>*);
	 SequenceFile<Pair>* createInvertedFile(SequenceFile<Occurrence>* of);
	
};

#endif /* INDEXWRITER_H_ */
