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
#include <boost/unordered_map.hpp>
#include "index/Field.h"
#include "index/Occurrence.h"
#include "index/Vocabulary.h"
#include "index/Pair.h"
#include "index/Doc.h"
#include "util/SequenceFile.h"
#include "util/Page.h"
#include "textanalysis/Analyzer.h"

using namespace std;

class IndexWriter {

	int runSize;

	string directory;
	Vocabulary vocabulary;

	int docIdCounter;
	double averageDocLength;

	Occurrence* buffer;
	int bufferSize;

	Analyzer analyzer;

	vector<SequenceFile<Occurrence>* > runs;

	SequenceFile<Doc>* documentsFile;
	SequenceFile<int>* docLenghtFile;
	SequenceFile<int>* outDegreeFile;
	SequenceFile<int>* linkIdsFile;

	fstream linksStream;
	fstream termsStream;

	boost::unordered_map<string, int> urls;

public:

	IndexWriter(string directory, int runSize = 500000);
	~IndexWriter();

	int addDocument(Page& page);
	void proccessTerms(map<string, int> terms, Field field);
	void processAnchorText(map<string, string> links);
	void addOccurrence(int term_id, int doc_id, int frequency, int field);
	void maybeFlush();
	void flush();
	void commit();
	void computePageRank();
	void computeAverageDocLength();
	void computeAnchorText();

	SequenceFile<Occurrence>* kwaymerge(vector<SequenceFile<Occurrence>*>&);

	SequenceFile<Occurrence>* merge(vector<SequenceFile<Occurrence>*>&);
	void merge2runs(SequenceFile<Occurrence>*, SequenceFile<Occurrence>*, SequenceFile<Occurrence>*);
	SequenceFile<Pair>* createInvertedFile(SequenceFile<Occurrence>* of);

};

#endif /* INDEXWRITER_H_ */
