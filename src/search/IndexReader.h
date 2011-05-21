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

class IndexReader {

	Vocabulary* vocabulary;
	SequenceFile<Pair>* invertedLists;
	SequenceFile<Doc>* docsFile;
	double avgDocLen;
	
public:

	IndexReader(string directory) {
		vocabulary = new Vocabulary(directory + "/vocabulary");
		invertedLists = new SequenceFile<Pair>(directory + "/index", false);
		docsFile = new SequenceFile<Doc>(directory + "/urls", false);
		
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
		return vocabulary->findTerm(word);
	}

	Pair* getInvertedList(Term* term){
		//###
//		clock_t inicio_list = clock();
		//###
		
		Pair* pairs = new Pair[term->docFrequency];
		invertedLists->setPosition(term->listPosition);
		invertedLists->readBlock(pairs, term->docFrequency);
		
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
	
	double getAverageDocLength(){
		return avgDocLen;
	}
};

#endif /* INDEXREADER_H_ */
