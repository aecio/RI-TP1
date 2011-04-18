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
#include <htmlcxx/html/Node.h>
#include <htmlcxx/html/ParserDom.h>
#include "IndexWriter.h"
#include "TextTokenizer.h"
#include "OccurrenceFile.h"
#include "Pair.h"
#include "InvertedFile.h"

using namespace std;
using namespace htmlcxx;
	
IndexWriter::IndexWriter(string directory_ = "indice"){
	docIdCounter = 0;
	termIdCounter = 0;
	directory = directory_;
	occurrencesFile = new OccurrenceFile("occurrences");
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
	

//Term& IndexWriter::getVocabularyTerm(string t) {
//	map<string, Term>::iterator entry = vocabulary.find(t);
//	if(entry == vocabulary.end() ){
//		Term& term = vocabulary[t];
//		term.id = ++termIdCounter;
//		return term;
//	} else {
//		return entry->second;
//	}
//}


//Term* IndexWriter::findTermById(int id){
//	map<string, Term>::iterator it = vocabulary.begin();
//	for(; it != vocabulary.end(); it++){
//		if(it->second.id == id){
//			return &(it->second);
//		}
//	}
//	return 0;
//}

int IndexWriter::addDocument(string& html){
	string text = extractTextFrom(html);
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
	
	return docIdCounter;
}

void IndexWriter::commit() {
	
	list<OccurrenceFile*> runs = createRuns();
	
	OccurrenceFile* occurrencesSorted = merge(runs);

	InvertedFile* invertedLists = createInvertedFile(occurrencesSorted);
	
	vocabulary.saveTo("vocabulary");
	
	occurrencesFile->close();
	occurrencesSorted->close();
	invertedLists->close();

	cout << "Creating inverted file... Done." << endl;
}

InvertedFile* IndexWriter::createInvertedFile(OccurrenceFile* of){
	
	InvertedFile* index = new InvertedFile("index");
	Occurrence block[RUN_SIZE];
	of->reopen();
	
	while( of->hasNext() ){
		int blockSize = of->readBlock(block, RUN_SIZE);
		cout << "Read block of size: " << blockSize << endl;
		int j=0;
		while(j < blockSize){
			int termId = block[j].termId;

			cout << "List position: " << index->getPosition() << endl;
			vocabulary.setListPosition(block[j].termId, index->getPosition() );
			
			while(termId == block[j].termId) {
								
				Pair entry(block[j].docId, block[j].termFrequencyInDoc);
				index->write(entry);
				
				vocabulary.incrementDocFrequency(termId);
				
				j++;
				
				//se o array acabar, ler outro bloco
				if(j >= blockSize && of->hasNext() ){ 
					blockSize = of->readBlock(block, RUN_SIZE);
					cout << "Reading a extra block of size: " << blockSize << endl;
					j=0;
				}
			}
			cout << "Term frequency: " << vocabulary.getDocFrequency(termId) << endl;
		}
	}
	index->close();
	return index;
}

list<OccurrenceFile*> IndexWriter::createRuns(){
	occurrencesFile->rewind();
	list<OccurrenceFile*> runs;
	while( occurrencesFile->hasNext() ) {
		int blockNumber = runs.size()+1;
		
		cout << endl << "Reading block " << blockNumber << endl;
		Occurrence occurs[RUN_SIZE];
		int occursRead = occurrencesFile->readBlock(occurs, RUN_SIZE);

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
		cout << "Only one Run. No need to merge." << endl;
		return runs.front();
	}
	
	OccurrenceFile* merged = 0;
	OccurrenceFile* run1 = 0;
	OccurrenceFile* run2 = 0;
	
	int id = 0;
	while(runs.size() > 1) {
		run1 = runs.front();
		runs.pop_front();
		
		run2 = runs.front();
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
