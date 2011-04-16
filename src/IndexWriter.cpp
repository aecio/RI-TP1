/*
 * IndexWriter.cpp
 *
 *  Created on: 13/04/2011
 *      Author: aecio
 */

#include <assert.h> 
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <htmlcxx/html/Node.h>
#include <htmlcxx/html/ParserDom.h>
#include "IndexWriter.h"
#include "TextTokenizer.h"
#include "OccurrenceFile.h"

using namespace std;
using namespace htmlcxx;
	
IndexWriter::IndexWriter(string directory_){
	docId = 0;
	termId = 0;
	runSize = 200000;
	directory = directory_;
	ofile = new OccurrenceFile("occurrences");
}
	
string IndexWriter::extractTextFrom(string& html){
	HTML::ParserDom parser;
	tree<HTML::Node> dom = parser.parseTree(html);
	tree<HTML::Node>::iterator it = dom.begin();
	tree<HTML::Node>::iterator end = dom.end();
	string text = "";	
	for (; it != end; ++it) {
		if ((!it->isTag()) && (!it->isComment()) ) {
			text += " ";
			text += it->text();
		}
	}
	return text;
}
	

Term& IndexWriter::getVocabularyTerm(string t) {
	map<string, Term>::iterator entry = vocabulary.find(t);
	if(entry == vocabulary.end() ){
		Term& term = vocabulary[t];
		term.setId(++termId);
		return term;
	} else {
		return entry->second;
	}
}

void IndexWriter::addDocument(string& html){
	string text = extractTextFrom(html);
	TextTokenizer tokenizer(text);
	map<string, int> docFrequencies;
	docId++;
	
	while(tokenizer.hasNext()){ //para cada termo "indexável"
		string t = tokenizer.nextToken();
		docFrequencies[t]++; //contabilizar frequencia do termo neste doc
	}
	
	map<string, int>::iterator it = docFrequencies.begin();
	for(; it != docFrequencies.end(); it++){
		string t = it->first;
		Term& term = getVocabularyTerm(t);
		
		Occurrence oc(term.getId(), docId, it->second);
		ofile->write(oc);
	}
}

void IndexWriter::commit() {
	
	list<OccurrenceFile*> runs = createRuns();
	OccurrenceFile* occurrencesSorted = merge(runs);
	
	ofile->close();
	occurrencesSorted->close();

	cout << "Created sorted occurrences file... Done." << endl;
}

list<OccurrenceFile*> IndexWriter::createRuns(){
	ofile->rewind();
	list<OccurrenceFile*> runs;
	while( ofile->hasNext() ) {
		int blockNumber = runs.size()+1;
		
		cout << endl << "Reading block " << blockNumber << endl;
		Occurrence occurs[runSize];
		int occursRead = ofile->readBlock(occurs, runSize);

		cout << "Sorting block " << blockNumber << "." << endl;
		sort(occurs, occurs+occursRead);
		
		stringstream fileName;
		fileName << "run" << blockNumber;
		OccurrenceFile* tempFile = new OccurrenceFile(fileName.str());
		
		cout << "Rewriting ordered run in file " << fileName.str() << endl;
		tempFile->writeBlock(occurs, occursRead);
		tempFile->close();
		runs.push_back(tempFile);
	}
	return runs;
}

OccurrenceFile* IndexWriter::merge(list<OccurrenceFile*>& runs) {
	cout << endl << "Merging " << runs.size() <<" runs..." << endl;
	
	if(runs.size() == 1){
		cout << "Oly one Run. No need to merge." << endl;
		return runs.front();
	}
	
	OccurrenceFile* merged = 0;
	OccurrenceFile* run1 = 0;
	OccurrenceFile* run2 = 0;
	
	int id = 0;
	while(runs.size() > 1) {
		run1 = runs.front();
//		run1->reopen();
		runs.pop_front();
		
		run2 = runs.front();
//		run2->reopen();
		runs.pop_front();
	
		
		stringstream name;
		name << "temp" << id++;
		merged = new OccurrenceFile( name.str() );
		
		merge2runs(run1, run2, merged);
		
		run1->deleteFile();
		run2->deleteFile();
		
		runs.push_back(merged);
	}
	
	cout << "Runs merged into one file with " << runs.front()->getSize() << " entries." << endl;
	return runs.front();
}

void IndexWriter::merge2runs(OccurrenceFile* runA,
							 OccurrenceFile* runB,
							 OccurrenceFile* mergedFile ) {
							 	
	runA->reopen();
	runB->reopen();
	mergedFile->reopen();
	
	cout << "Merging "<< runA->getName() <<" and "<< runB->getName() << " into " << mergedFile->getName() << endl;
	cout << runA->getName() << " size: " << runA->getSize() << endl;
	cout << runB->getName() << " size: " << runB->getSize() << endl;
	
	Occurrence headA = runA->read();
	Occurrence headB = runB->read();
	
	while ( (runA->getPosition() <= runA->getSize()) && runB->getPosition() <= runB->getSize() ) {
		if(headA < headB){
			mergedFile->write(headA);
			headA = runA->read();
		} else {
			mergedFile->write(headB);
			headB = runB->read();
		}
	}
	
	while( runA->getPosition() <= runA->getSize() ){
		mergedFile->write(headA);
		headA = runA->read();
	}
	
	while( runB->getPosition() <= runB->getSize() ){
		mergedFile->write(headB);
		headB = runB->read();
	}
	
	cout << "Finished merging "<< mergedFile->getName() <<" with " 
		 << mergedFile->getSize() << " entries." << endl << endl;
}

void IndexWriter::printOccurence(Occurrence& it){
		cout << it.termId;
		cout << ' ';
		cout << it.docId;
		cout << ' ';
		cout << it.termFrequencyInDoc;
		cout << '\n';
}
