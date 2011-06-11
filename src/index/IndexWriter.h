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
#include <vector>
#include "index/Field.h"
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
	double averageDocLength;

	Occurrence* buffer;
	int bufferSize;

	vector<SequenceFile<Occurrence>* > runs;
	SequenceFile<Doc>* documentsFile;
	SequenceFile<int>* docLenghtFile;

public:

	IndexWriter(string directory, int runSize = 500000);

	int addDocument(Page& page);
	void proccessTerms(map<string, int> terms, Field field);
	void addOccurrence(int term_id, int doc_id, int frequency, int field);
	void maybeFlush();
	void flush();
	void commit();

	SequenceFile<Occurrence>* kwaymerge(vector<SequenceFile<Occurrence>*>&);

	SequenceFile<Occurrence>* merge(vector<SequenceFile<Occurrence>*>&);
	void merge2runs(SequenceFile<Occurrence>*, SequenceFile<Occurrence>*, SequenceFile<Occurrence>*);
	SequenceFile<Pair>* createInvertedFile(SequenceFile<Occurrence>* of);

};

#endif /* INDEXWRITER_H_ */
