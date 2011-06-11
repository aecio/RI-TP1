/*
 * IndexReader.h
 *
 *  Created on: 27/04/2011
 *      Author: aecio
 */

#ifndef INDEXREADER_H_
#define INDEXREADER_H_

#include <ctime>
#include <string>
#include <vector>
#include <vector>

class IndexReader {

	Vocabulary* vocabulary;
	SequenceFile<Pair>* invertedLists;
	SequenceFile<Doc>* docsFile;
	SequenceFile<int>* docLengthFile;
	double avgDocLen;
	
public:

	IndexReader(string directory) {
		vocabulary = new Vocabulary(directory + "/vocabulary");
		invertedLists = new SequenceFile<Pair>(directory + "/index", false);
		docsFile = new SequenceFile<Doc>(directory + "/urls", false);
		docLengthFile = new SequenceFile<int>(directory + "/doclength", false);
		SequenceFile<double>* avgdoclenFile = new SequenceFile<double>(directory + "/avgdoclen", false);
		avgDocLen = avgdoclenFile->read();
		avgdoclenFile->close();
	}
	
	~IndexReader(){
		invertedLists->close();
		docsFile->close();
	}
	
	int getCollectionSize(){
		return docsFile->getSize();
	}
	
	Term* getTerm(string word){
		cout << "Finding term " << word << endl;
		Term* t = vocabulary->findTerm(word);
		return t;
	}

	vector<Pair> getDocFreqs(Term* term, int field) {
		Pair* pairs = new Pair[term->getFieldFrequency(field)];
		invertedLists->setPosition(term->getFieldListPosition(field));
		invertedLists->readBlock(pairs, term->getFieldFrequency(field));
		vector<Pair> docfreqs;
		for(int i=0; i<term->getFieldFrequency(field); i++){
			docfreqs.push_back(pairs[i]);
		}
		delete pairs;

		return docfreqs;
	}

	Pair* getInvertedList(Term* term){
		//###
//		clock_t inicio_list = clock();
		//###
		
		Pair* pairs = new Pair[term->getFieldFrequency(CONTENT)];
		invertedLists->setPosition(term->getFieldListPosition(CONTENT));
		invertedLists->readBlock(pairs, term->getFieldFrequency(CONTENT));
		
		//###
//		clock_t fim_list = clock();
//		double total_list = ((double)(fim_list-inicio_list)) / (double) (CLOCKS_PER_SEC/1000);
//		printf("Tempo para ler lista do termo %s: %.3f ms\n", term->term, total_list);
		//###
		
		return pairs;
	}
	
	Doc getDoc(int docId){
		//###
//		clock_t inicio_pair = clock();
		//###
		
		docsFile->setPosition(docId-1);
		Doc doc = docsFile->read();
		
		//###
//		clock_t fim_pair = clock();
//		double total = ((double)(fim_pair-inicio_pair)) / (double) (CLOCKS_PER_SEC/1000);
//		printf("Tempo para ler doc %i: %.3f ms\n", docId, total);
		//###
		return doc;
	}
	
	int getDocLength(int docId){
		docLengthFile->setPosition(docId-1);
		int length = docLengthFile->read();
		return length;
	}

	double getAverageDocLength(){
		return avgDocLen;
	}
};

#endif /* INDEXREADER_H_ */
