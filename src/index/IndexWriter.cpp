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

#define INDEX_FILE "index"
#define VOCABULARY_FILE "vocabulary"
#define DOCUMENTS_FILE "docs"
#define DOC_LENGTH_FILE "doclength"
#define AVG_DOC_LENGTH_FILE "avgdoclen"
#define PAGERANK_FILE "pagerank"
#define PAGERANK_ITERATIONS 70

using namespace std;
	
IndexWriter::IndexWriter(string directory_, int runSize_){
	runSize = runSize_;	
	directory = directory_;
	docIdCounter = 0;
	averageDocLength = 0;

	documentsFile = new SequenceFile<Doc>(directory + "/" + DOCUMENTS_FILE);
	docLenghtFile =  new SequenceFile<int>(directory + "/" + DOC_LENGTH_FILE);

	outDegreeFile =  new SequenceFile<int>(directory + "/outdegrees");
	linkIdsFile = new SequenceFile<int>(directory + "/linksIds");

	linksStream.open((directory + "/links").c_str(), ios::in | ios::out | ios::trunc);
	termsStream.open((directory + "/termsNumber").c_str(), ios::in | ios::out | ios::trunc);

	buffer = new Occurrence[runSize];
	bufferSize = 0;
}

IndexWriter::~IndexWriter(){

	linksStream.close();
//	remove((directory + "/links").c_str());

	termsStream.close();
//	remove((directory + "/termsNumber").c_str());

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

	if(outDegreeFile != NULL){
		outDegreeFile->close();
		outDegreeFile->deleteFile();
		delete outDegreeFile;
		outDegreeFile = NULL;
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

	analyzer.analyze(page.getUrl());
	proccessTerms(analyzer.getTermFreqs(), URL);

	analyzer.analyze(page.getText());
	proccessTerms(analyzer.getTermFreqs(), CONTENT);
    int docLength = analyzer.getLength();

    docLenghtFile->write(docLength);
	averageDocLength += docLength;

	Doc doc(docIdCounter, page.getUrl(), page.getTitle(), page.getDescription());
	documentsFile->write(doc);

	string url(page.getUrl(), 0 , URL_SIZE);
	urls[url] = docIdCounter;

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

	map<string, string>::iterator it = links.begin();
	for(;it != links.end(); it++){
		analyzer.analyze(it->second);
		map<string, int> terms = analyzer.getTermFreqs();

		//Guardar link e termos
		linksStream << it->first << endl;
		termsStream << terms.size() << endl;
		map<string, int>::iterator anchorTerm = terms.begin();
		for(; anchorTerm != terms.end(); anchorTerm++) {
			termsStream << anchorTerm->first << " " << anchorTerm->second << endl;
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
	cout << ">> Flushing buffer of "<< bufferSize <<" occurrences to disk..." << endl;

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

	computeAnchorText();

	flush();

	/* Merge */
	SequenceFile<Occurrence>* occurrencesSorted = kwaymerge(runs);

	createInvertedFile(occurrencesSorted);
	occurrencesSorted->deleteFile();

	vocabulary.saveTo(directory + "/" + VOCABULARY_FILE);

	delete [] buffer;

	computeAverageDocLength();

	computePageRank();

	cout << ">> Done." << endl;
}

void IndexWriter::computeAnchorText(){
	cout << ">> Computing anchor text..." << endl;

	linksStream.seekg(0, ios::beg);
	termsStream.seekg(0, ios::beg);

	string link;
	int termsNumber;

	while(linksStream.good()){
		getline(linksStream, link);
		if(link == "" && linksStream.eof()) break;

		boost::unordered_map<string, int>::iterator url = urls.find(link);
		int docId;
		if(url == urls.end()){
			docId = -1; //documento que não existe na coleção
		}else{
			docId = url->second;
		}

		linkIdsFile->write(docId); //Guarda Id na mesma ordem para computação do PageRank

		termsStream >> termsNumber;

		string term;
		int frequency;

		for(int j=0; j < termsNumber; j++){
			termsStream >> term;
			termsStream >> frequency;
			if(docId != -1){
				int termId = vocabulary.addTerm(term);
				addOccurrence(termId, docId, frequency, ANCHOR_TEXT);
			}
		}
	}
}

void IndexWriter::computeAverageDocLength(){
	cout << ">> Computing average document length... " << endl;
	SequenceFile<double> avgdoclen(directory + "/" + AVG_DOC_LENGTH_FILE);
	double average = averageDocLength / docIdCounter;
	avgdoclen.write(average);
	avgdoclen.close();
	cout << "Average document length: " << average << endl;
}

void IndexWriter::computePageRank(){
	cout << ">> Computing PageRank... " << endl;

	//@@@
//	ofstream pagerank_temp;
//	pagerank_temp.open( (directory + "/" + "page_rank.txt").c_str(), ios::out | ios::trunc);
	//@@@

	const double q = 0.15; //constant
	double* pagerank = new double[docIdCounter];
	double* aux = new double[docIdCounter];
	int* outdegrees = new int[docIdCounter];

	outDegreeFile->rewind();
	int qtd = outDegreeFile->readBlock(outdegrees, docIdCounter);
	assert( qtd == docIdCounter);


	cout << "Inicializando... " << endl;
	for(int i=0; i<docIdCounter; i++) {
		pagerank[i] = 1;
	}

	cout << "Executatando iterações... " << endl;
	for(int iteration = 0; iteration < PAGERANK_ITERATIONS; iteration++){
//		cout << "Iteração " << iteration << endl;

		linkIdsFile->rewind();

		for(int i=0; i<docIdCounter; i++) {
			aux[i] = 0;
		}

		for(int docId=0; docId<docIdCounter; docId++) {

			int outDegreeDoc = outdegrees[docId];

			double rank;
			if(outDegreeDoc == 0){
				rank = 0;
			} else {
				rank = pagerank[docId] / outDegreeDoc;
			}

			int destLinkId[outDegreeDoc];
			int size = linkIdsFile->readBlock(destLinkId, outDegreeDoc);
			assert(size == outDegreeDoc);

			for(int i=0; i<size; i++){
				int linkId = destLinkId[i];
//				if(linkId != -1){
				if(linkId != -1 && linkId != docId){
					aux[linkId-1] += rank;
				}
			}
		}

//		if(iteration == PAGERANK_ITERATIONS-1) {
//			documentsFile->reopen();
//		}

		for(int i=0; i<docIdCounter; i++) {
			aux[i] = (q / docIdCounter) + (1-q) * aux[i];

			//@@@
//			if(iteration == PAGERANK_ITERATIONS-1){
//				documentsFile->setPosition(i);
//				Doc doc = documentsFile->read();
//				cout << setw(4) << i+1 << "  " << fixed << setprecision(10) << aux[i] << "  " << doc.getUrl() << endl;
//				pagerank_temp << setw(4) << i+1 << "  " << fixed << setprecision(10) << aux[i] << "  " << doc.getUrl() << endl;
//			}
			//@@@
		}

		double* temp = pagerank;
		pagerank = aux;
		aux = temp;
	}

	//Save PageRank
	SequenceFile<double> pagerankFile(directory + "/" + PAGERANK_FILE);
	pagerankFile.writeBlock(pagerank, docIdCounter);
	pagerankFile.close();

	delete [] pagerank;
	delete [] aux;
	delete [] outdegrees;
}

SequenceFile<Occurrence>* IndexWriter::kwaymerge(vector<SequenceFile<Occurrence>*>& runs){
	cout << ">> Finishing index merge..." << endl;
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
		run_index.erase(top);

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
	cout << ">> Building inverted index..." << endl;

	SequenceFile<Pair>* index = new SequenceFile<Pair>(directory + "/" + INDEX_FILE);

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
