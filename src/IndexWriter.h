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
#include "Term.h"
#include "Occurrence.h"
#include "OccurrenceFile.h"

using namespace std;
	
class IndexWriter {

	string directory;
	map<string, Term> vocabulary;
	
	int docId;
	int termId;
	
	OccurrenceFile* ofile;
	unsigned int runSize;
	
	string extractTextFrom(string& html);
	Term& getVocabularyTerm(string t);
	
	void printOccurence(Occurrence& it);
public:
	list<OccurrenceFile*> createRuns();
	OccurrenceFile* merge(list<OccurrenceFile*>&);
	void merge2runs(OccurrenceFile*, OccurrenceFile*, OccurrenceFile*);
	
	IndexWriter(string directory);
	
	void addDocument(string&);
	void commit();
	
};

#endif /* INDEXWRITER_H_ */
