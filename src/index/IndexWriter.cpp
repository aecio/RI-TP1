/*
 * IndexWriter.cpp
 *
 *  Created on: 13/04/2011
 *      Author: aecio
 */

#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "textanalysis/TextTokenizer.h"
#include "index/IndexWriter.h"
#include "index/Pair.h"
#include "util/Page.h"
#include "util/SequenceFile.h"

using namespace std;
	
IndexWriter::IndexWriter(string directory_, int runSize_){
	runSize = runSize_;	
	directory = directory_;
	docIdCounter = 0;
	termIdCounter = 0;
	averageDocLength = 0;
	pagesFile = new SequenceFile<Doc>(directory + "/urls");

	buffer = new Occurrence[runSize];
	bufferSize = 0;
}
	
int IndexWriter::addDocument(Page& page){
	docIdCounter++;
	
	string text = page.getPlainText();
	TextTokenizer tokenizer(text);

	map<string, int> terms;
	int documentLength = 0;
	while(tokenizer.hasNext()){ //para cada termo "indexável"
		string t = tokenizer.nextToken();
		terms[t]++; //contabilizar frequencia do termo neste doc
		documentLength++;
	}
	
	map<string, int>::iterator it = terms.begin();
	for(; it != terms.end(); it++){
		int termId = vocabulary.addTerm(it->first);
		addOccurrence(termId, docIdCounter, it->second);
	}
	
	Doc doc(docIdCounter, page.url, documentLength);
	pagesFile->write(doc);
	averageDocLength += documentLength;
	
	return docIdCounter;
}

void IndexWriter::addOccurrence(int term_id, int doc_id, int frequency){
	buffer[bufferSize] = Occurrence(term_id, doc_id, frequency);
	bufferSize++;
	maybeFlush();
}

void IndexWriter::maybeFlush(){
	if(bufferSize >= runSize){
		flush();
	}
}

void IndexWriter::flush(){
	if(bufferSize <= 0) return;

	int blockNumber = runs.size();

	cout << "Sorting block " << blockNumber << "." << endl;
	sort(buffer, buffer+bufferSize);

	stringstream fileName;
	fileName << directory << "/run" << blockNumber;
	cout << "Writing ordered run in file " << fileName.str() << endl;

	SequenceFile<Occurrence>* runFile = new SequenceFile<Occurrence>(fileName.str());
	runFile->writeBlock(buffer, bufferSize);
	runFile->close();

	runs.push_back(runFile);

	bufferSize = 0;
}

void IndexWriter::commit() {

	flush();

	SequenceFile<Occurrence>* occurrencesSorted = merge(runs);

	SequenceFile<Pair>* invertedLists = createInvertedFile(occurrencesSorted);
	occurrencesSorted->deleteFile();
	
	vocabulary.saveTo(directory + "/vocabulary");
	
	invertedLists->close();
	pagesFile->close();
	
	SequenceFile<double>* avgdoclen = new SequenceFile<double>(directory + "/avgdoclen");
	double average = averageDocLength / docIdCounter;
	avgdoclen->write(average);
	avgdoclen->close();
	
	cout << "Average document length: " << average << endl;

	cout << "Done." << endl;
}

SequenceFile<Occurrence>* IndexWriter::merge(list<SequenceFile<Occurrence>*>& runs) {
	cout << endl << "Merging " << runs.size() <<" runs..." << endl;
	
	if(runs.size() == 1){
		cout << "Only one Run. No need to merge." << endl;
		return runs.front();
	}
	
	SequenceFile<Occurrence>* merged = 0;
	SequenceFile<Occurrence>* run1 = 0;
	SequenceFile<Occurrence>* run2 = 0;
	
	int id = 0;
	while(runs.size() > 1) {
		run1 = runs.front();
		runs.pop_front();
		
		run2 = runs.front();
		runs.pop_front();
	
		stringstream name;
		name << directory << "/temp" << id++;
		merged = new SequenceFile<Occurrence>( name.str() );
		
		merge2runs(run1, run2, merged);
		
		run1->deleteFile();
		run2->deleteFile();
		
		runs.push_back(merged);
	}
	
	cout << "Runs merged into one file with " << runs.front()->getSize() << " entries." << endl;
	return runs.front();
}

void IndexWriter::merge2runs(SequenceFile<Occurrence>* runA,
							 SequenceFile<Occurrence>* runB,
							 SequenceFile<Occurrence>* mergedFile ) {
							 	
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

SequenceFile<Pair>* IndexWriter::createInvertedFile(SequenceFile<Occurrence>* of){
	cout << endl << "Creating final index file..." << endl;
	SequenceFile<Pair>* index = new SequenceFile<Pair>(directory + "/index");
	Occurrence* block = new Occurrence[runSize];
	of->reopen();
	while( of->hasNext() ){
		int blockSize = of->readBlock(block, runSize);
		int j=0;
		while(j < blockSize){
			int termId = block[j].termId;

			vocabulary.setListPosition(block[j].termId, index->getPosition() );

			while(termId == block[j].termId) {

				Pair entry(block[j].docId, block[j].termFrequencyInDoc);
				index->write(entry);

				vocabulary.incrementDocFrequency(termId);

				j++;

				//se o array acabar, ler outro bloco
				if(j >= blockSize && of->hasNext() ){
					blockSize = of->readBlock(block, runSize);
					j=0;
				}
			}
		}
	}
	index->close();
	return index;
}
