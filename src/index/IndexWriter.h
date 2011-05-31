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
	double averageDocLength;

	Occurrence* buffer;
	int bufferSize;

	list<SequenceFile<Occurrence>* > runs;
	SequenceFile<Doc>* pagesFile;

public:
	IndexWriter(string directory, int runSize = 500000);

	int addDocument(Page& page);

	void addOccurrence(int term_id, int doc_id, int frequency);
	void maybeFlush();
	void flush();
	void commit();

	SequenceFile<Occurrence>* merge(list<SequenceFile<Occurrence>*>&);
	void merge2runs(SequenceFile<Occurrence>*, SequenceFile<Occurrence>*, SequenceFile<Occurrence>*);
	SequenceFile<Pair>* createInvertedFile(SequenceFile<Occurrence>* of);

};

#endif /* INDEXWRITER_H_ */
