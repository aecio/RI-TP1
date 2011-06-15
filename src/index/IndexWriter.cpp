/*
 * IndexWriter.cpp
 *
 *  Created on: 13/04/2011
 *      Author: aecio
 */

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ios>
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
	documentsFile = new SequenceFile<Doc>(directory + "/docs");
	docLenghtFile =  new SequenceFile<int>(directory + "/doclength");
	outDegreeFile =  new SequenceFile<int>(directory + "/outdegrees");
	anchorFile = new SequenceFile<TermFreq>(directory + "/anchors");
	linksFile = new SequenceFile<LinkTerm>(directory + "/links");
	linkIdsFile = new SequenceFile<int>(directory + "/linksIds");
	pageRankFile = new  SequenceFile<double>(directory + "/pagerank");

	buffer = new Occurrence[runSize];
	bufferSize = 0;
}

IndexWriter::~IndexWriter(){

	if(documentsFile != NULL){
		documentsFile->close();
		delete documentsFile;
		documentsFile = NULL;
	}

	if(docLenghtFile != NULL){
		docLenghtFile->close();
		delete docLenghtFile;
		docLenghtFile = NULL;
	}

	if(pageRankFile != NULL){
		pageRankFile->close();
		delete pageRankFile;
		pageRankFile = NULL;
	}

	if(outDegreeFile != NULL){
		outDegreeFile->close();
		outDegreeFile->deleteFile();
		delete outDegreeFile;
		outDegreeFile = NULL;
	}

	if(anchorFile != NULL){
		anchorFile->close();
		anchorFile->deleteFile();
		delete anchorFile;
		anchorFile = NULL;
	}

	if(linksFile != NULL){
		linksFile->close();
		linksFile->deleteFile();
		delete linksFile;
		linksFile = NULL;
	}

	if(linkIdsFile != NULL){
		linkIdsFile->close();
		linkIdsFile->deleteFile();
		delete linkIdsFile;
		linkIdsFile = NULL;
	}
}
	
int IndexWriter::addDocument(Page& page) {
	docIdCounter++;

	analyzer.analyze(page.getTitle());
	proccessTerms(analyzer.getTermFreqs(), TITLE);

	analyzer.analyze(page.getDescription());
	proccessTerms(analyzer.getTermFreqs(), DESCRIPTION);

	analyzer.analyze(page.getKeywords());
	proccessTerms(analyzer.getTermFreqs(), KEYWORDS);

	analyzer.analyze(page.getText());
	proccessTerms(analyzer.getTermFreqs(), CONTENT);
    int docLength = analyzer.getLength();

    docLenghtFile->write(docLength);
	averageDocLength += docLength;

	Doc doc(docIdCounter, page.getUrl(), page.getTitle(), page.getDescription());
	documentsFile->write(doc);
	
	urls[page.getUrl()] = docIdCounter;

	processAnchorText(page.getLinks());

	return docIdCounter;
}

void IndexWriter::proccessTerms(map<string, int> terms, Field field){
	map<string, int>::iterator it = terms.begin();
	for(; it != terms.end(); it++){
		int termId = vocabulary.addTerm(it->first);
		addOccurrence(termId, docIdCounter, it->second, field);
	}
}

void IndexWriter::processAnchorText(map<string, string> links){
	//Escrever out-degree da pagina $docIdCounter
	int outDegree = links.size();
	outDegreeFile->write(outDegree);

	//Guardar links da pagina $docIdCounter
	map<string, string>::iterator it = links.begin();
	for(;it != links.end(); it++){
		analyzer.analyze(it->second);
		map<string, int> terms = analyzer.getTermFreqs();

		//Guardar link e quantidade de termos
		LinkTerm lt(it->first, terms.size());
		linksFile->write(lt);

		//Guardar frequencias dos termos
		map<string, int>::iterator anchorTerm = terms.begin();
		for(; anchorTerm != terms.end(); anchorTerm++){
			int termId = vocabulary.addTerm(anchorTerm->first);
			TermFreq tf(termId, anchorTerm->second);
			anchorFile->write(tf);
		}
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
	/** ANCHOR TEXT */
	anchorFile->rewind();
	linksFile->rewind();

	cout << "Computing anchor text..." << endl;
	while(linksFile->hasNext()){
		LinkTerm lt = linksFile->read();
		map<string, int>::iterator url = urls.find(lt.getLink());
		int docId;
		if(url == urls.end()){
			docId = -1; //documento que não existe na coleção
		}else{
			docId = url->second;
		}
		linkIdsFile->write(docId); //Guarda Id na mesma ordem para computação do PageRank

		for(int j=0; j < lt.getTermsNumber(); j++){
			TermFreq tf = anchorFile->read();
			if(docId != -1){
				addOccurrence(tf.getTermId(), docId, tf.getFrequency(), ANCHOR_TEXT);
			}
		}
	}


	/** FLUSH & MERGE */
	cout << "Finishing index merge..." << endl;
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
	delete avgdoclen;
	cout << "Average document length: " << average << endl;

	delete buffer;

	/** PAGE RANK **/
	cout << "Computing PageRank..." << endl;

	double* source = new double[docIdCounter];
	double* dest = new double[docIdCounter];
	int* outdegrees = new int[docIdCounter];

	outDegreeFile->rewind();
	outDegreeFile->readBlock(outdegrees, docIdCounter);

	double q = 0.15; //constant

	for(int i=0;i<docIdCounter;i++) {
		source[i] = 1/docIdCounter;
	}

	for(int iteration = 0; iteration < 50; iteration++){
		linkIdsFile->rewind();

		for(int i=0; i<docIdCounter; i++) {
			dest[i] = 0;
		}

		for(int docId=0; docId<docIdCounter; docId++) {
			double rank;
			if(outdegrees[docId] == 0)
				rank = 0;
			else
				rank = source[docId] / outdegrees[docId];

			for(int i=0; i < outdegrees[docId]; i++){
				int destLinkId = linkIdsFile->read();
				dest[destLinkId-1] += rank;
			}
		}

		for(int i=0; i<docIdCounter; i++) {
			dest[i] = (q / docIdCounter) + (1-q) * dest[i];

			//@@@
//			if(iteration == 49){
//				linksFile->setPosition(i);
//				LinkTerm lt = linksFile->read();
//				cout << setw(4) << i+1 << " = " << setw(10) << fixed << dest[i] << "\t" << lt.getLink() << endl;
//			}
			//@@@
		}

		double* temp = source;
		source = dest;
		dest = temp;
	}

	//Save PageRank
	pageRankFile->writeBlock(source, docIdCounter);
	pageRankFile->close();

	delete [] source;
	delete [] dest;
	delete [] outdegrees;

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
			}

			if(field != occurrence.field){
				field = occurrence.field;
				vocabulary.setTermFieldPosition(occurrence.term_id, occurrence.field, index->getPosition());
			}

			Pair entry(occurrence.doc_id, occurrence.term_frequency);
			index->write(entry);

			vocabulary.incrementTermFieldFrequency(occurrence.term_id, occurrence.field);
	}
	index->close();
	return index;
}
