/*
 * IndexWriter.cpp
 *
 *  Created on: 13/04/2011
 *      Author: aecio
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include "IndexWriter.h"
#include "TextTokenizer.h"
#include "SequenceFile.h"
#include "Pair.h"
#include "Page.h"

using namespace std;
	
IndexWriter::IndexWriter(string directory_, int runSize_){
	runSize = runSize_;	
	directory = directory_;
	docIdCounter = 0;
	termIdCounter = 0;
	occurrencesFile = new SequenceFile<Occurrence>(directory + "/occurrences");
	pagesFile = new SequenceFile<Doc>(directory + "/urls");
}
	
int IndexWriter::addDocument(Page& page){
	string text = page.getPlainText();
	TextTokenizer tokenizer(text);

	map<string, int> termFrequencies;
	docIdCounter++;
	
	while(tokenizer.hasNext()){ //para cada termo "indexável"
		string t = tokenizer.nextToken();
		termFrequencies[t]++; //contabilizar frequencia do termo neste doc
	}
	
	map<string, int>::iterator it = termFrequencies.begin();
	for(; it != termFrequencies.end(); it++){
		
		int termId = vocabulary.addTerm(it->first);
		Occurrence oc(termId, docIdCounter, it->second);
		occurrencesFile->write(oc);
		
	}
	Doc d(page.url);
	pagesFile->write(d);
	
	cout << docIdCounter << " - " << d.url << endl;
	
	return docIdCounter;
}

void IndexWriter::commit() {
	
	list<SequenceFile<Occurrence>*> runs = createRuns();
	occurrencesFile->deleteFile();
	
	SequenceFile<Occurrence>* occurrencesSorted = merge(runs);

	SequenceFile<Pair>* invertedLists = createInvertedFile(occurrencesSorted);
	occurrencesSorted->deleteFile();
	
	vocabulary.saveTo(directory + "/vocabulary");
	
	invertedLists->close();
	pagesFile->close();
	
	pagesFile->reopen();
	while(pagesFile->hasNext()){
		cout << pagesFile->getPosition() << " = " << pagesFile->read().url << endl;
	}
	pagesFile->close();

	cout << "Done." << endl;
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

list<SequenceFile<Occurrence>*> IndexWriter::createRuns(){
	occurrencesFile->rewind();
	list<SequenceFile<Occurrence>*> runs;
	Occurrence* occurs = new Occurrence[runSize];
	while( occurrencesFile->hasNext() ) {
		int blockNumber = runs.size()+1;
		
		cout << endl << "Reading block " << blockNumber << endl;
		int occursRead = occurrencesFile->readBlock(occurs, runSize);

		cout << "Sorting block " << blockNumber << "." << endl;
		sort(occurs, occurs+occursRead);
		
		stringstream fileName;
		fileName << directory << "/run" << blockNumber;
		SequenceFile<Occurrence>* tempFile = new SequenceFile<Occurrence>(fileName.str());
		
		cout << "Rewriting ordered run in file " << fileName.str() << endl;
		tempFile->writeBlock(occurs, occursRead);
		tempFile->close();
		runs.push_back(tempFile);
	}
	delete occurs;
	occurs = NULL;
	return runs;
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

void IndexWriter::printOccurence(Occurrence& it){
		cout << it.termId;
		cout << ' ';
		cout << it.docId;
		cout << ' ';
		cout << it.termFrequencyInDoc;
		cout << '\n';
}
