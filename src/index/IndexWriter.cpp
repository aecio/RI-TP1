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
#include <queue>
#include "textanalysis/Analyzer.h"
#include "index/IndexWriter.h"
#include "index/Pair.h"
#include "util/Page.h"
#include "util/SequenceFile.h"
#include "util/BufferedFile.h"

using namespace std;
	
IndexWriter::IndexWriter(string directory_, int runSize_){
	runSize = runSize_;	
	directory = directory_;
	docIdCounter = 0;
	averageDocLength = 0;
	documentsFile = new SequenceFile<Doc>(directory + "/urls");
	docLenghtFile =  new SequenceFile<int>(directory + "/doclength");
	buffer = new Occurrence[runSize];
	bufferSize = 0;
}
	
int IndexWriter::addDocument(Page& page) {
	docIdCounter++;
	
	string text = page.getText();
	string title = page.getTitle();
//	string anchor = page.getAnchorText();
	string description = page.getDescription();
	string keywords = page.getKeywords();

	Analyzer contentAnalyzer(text);
	Analyzer titleAnalyzer(title);
//	Analyzer anchorAnalyzer(achor);
	Analyzer descriptionAnalyzer(description);
	Analyzer keywordsAnalyzer(keywords);

	map<string, int> terms = contentAnalyzer.getTermFreqs();
	
	proccessTerms(contentAnalyzer.getTermFreqs(), CONTENT);
	proccessTerms(titleAnalyzer.getTermFreqs(), TITLE);
//	proccessTerms(achorAnalyzer.getTermFreqs(), ANCHOR_TEXT);
	proccessTerms(descriptionAnalyzer.getTermFreqs(), DESCRIPTION);
	proccessTerms(keywordsAnalyzer.getTermFreqs(), KEYWORDS);


	Doc doc(docIdCounter, page.getUrl(), page.getTitle(), page.getDescription());
	documentsFile->write(doc);

    int docLength = contentAnalyzer.getLength();
    docLenghtFile->write(docLength);
	averageDocLength += docLength;
	
	return docIdCounter;
}

void IndexWriter::proccessTerms(map<string, int> terms, Field field){
	map<string, int>::iterator it = terms.begin();
	for(; it != terms.end(); it++){
		int termId = vocabulary.addTerm(it->first);
		addOccurrence(termId, docIdCounter, it->second, field);
	}
}

void IndexWriter::addOccurrence(int term_id, int doc_id, int frequency, int field){
	buffer[bufferSize] = Occurrence(term_id, doc_id, frequency, field);
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

	SequenceFile<Occurrence>* occurrencesSorted = kwaymerge(runs);

	createInvertedFile(occurrencesSorted);

	occurrencesSorted->deleteFile();
	
	vocabulary.saveTo(directory + "/vocabulary");
	
	documentsFile->close();
	
	SequenceFile<double>* avgdoclen = new SequenceFile<double>(directory + "/avgdoclen");
	double average = averageDocLength / docIdCounter;
	avgdoclen->write(average);
	avgdoclen->close();
	
	cout << "Average document length: " << average << endl;

	cout << "Done." << endl;
}

SequenceFile<Occurrence>* IndexWriter::kwaymerge(vector<SequenceFile<Occurrence>*>& runs){
	cout << "Merging "<< runs.size() <<" runs into " << directory + "/mergedfile" << endl;

	if(runs.size() == 1){
		cout << "Only one Run. No need to merge." << endl;
		return runs.front();
	}

	SequenceFile<Occurrence>* merged = new SequenceFile<Occurrence>(directory + "/mergedfile");
	bufferSize = 0;

	map<Occurrence, int> run_index;
	priority_queue< Occurrence, vector<Occurrence>, greater<Occurrence> > heap;

	vector<SequenceFile<Occurrence>*>::iterator it = runs.begin();
	for(; it != runs.end(); it++){
		SequenceFile<Occurrence>* run = *it;
		run->reopen();

		Occurrence o = run->read();
		heap.push(o);
		run_index[o] = it - runs.begin();

		cout << "Run "<< it - runs.begin() << " size: " << run->getSize() << endl;
	}

	while( !heap.empty() ){
		Occurrence top = heap.top();
		heap.pop();

		buffer[bufferSize] = top;
		bufferSize++;

		if(bufferSize >= runSize){
			merged->writeBlock(buffer, bufferSize);
			bufferSize = 0;
		}

		int index = run_index[top];
		if(runs[index]->hasNext()){
			Occurrence head = runs[index]->read();
			heap.push(head);
			run_index[head] = index;
		}
	}

	it = runs.begin();
	for(; it != runs.end(); it++){
		SequenceFile<Occurrence>* run = *it;
		run->deleteFile();
	}

	if(bufferSize > 0){
		merged->writeBlock(buffer, bufferSize);
		bufferSize = 0;
	}
	cout << "Finished merging " << directory << "/mergedfile" << " with " << merged->getSize() << " entries." << endl;

	return merged;
}

SequenceFile<Occurrence>* IndexWriter::merge(vector<SequenceFile<Occurrence>*>& runs) {
	cout << "Merging " << runs.size() <<" runs..." << endl;
	
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
		runs.erase(runs.begin());
		
		run2 = runs.front();
		runs.erase(runs.begin());
	
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
	cout << "Creating final index file..." << endl;
	SequenceFile<Pair>* index = new SequenceFile<Pair>(directory + "/index");

	of->reopen();
	BufferedFile<Occurrence> bufferedFile(of, buffer, runSize);

	int termId = -1;
	int field;
	while( bufferedFile.hasNext() ){
			Occurrence occurrence = bufferedFile.read();

			if(termId != occurrence.term_id){
				termId = occurrence.term_id;
				field = -1;
//				cout << "Escrenvendo lista para termo: " << termId << endl;
			}

			if(field != occurrence.field){
				field = occurrence.field;
				vocabulary.setTermFieldPosition(occurrence.term_id, occurrence.field, index->getPosition());
//				cout << "Campo: " << occurrence.field << endl;
			}

			Pair entry(occurrence.doc_id, occurrence.term_frequency);
//			cout << "(" << occurrence.doc_id << "," << occurrence.term_frequency <<")" << endl;
			index->write(entry);

			vocabulary.incrementTermFieldFrequency(occurrence.term_id, occurrence.field);
	}
	index->close();
	return index;
}
