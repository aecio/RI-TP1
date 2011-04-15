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

using namespace std;
	
class IndexWriter {

	string directory;
	map<string, Term> vocabulary;
	
	int docId;
	int termId;
	list<Occurrence> occurrences;
	int occurrencesCount;
	
	fstream fout;
	
	string extractTextFrom(string& html);
	Term& getVocabularyTerm(string t);
	
	void write(Occurrence& it);
	Occurrence read();
	
public:
	IndexWriter(string directory);
	
	void addDocument(string&);
	void commit();
	
};

#endif /* INDEXWRITER_H_ */
