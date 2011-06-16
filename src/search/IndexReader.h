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
	SequenceFile<double>* pageRankFile;
	double avgDocLen;
	
public:

	IndexReader(string directory) {
		vocabulary = new Vocabulary(directory + "/vocabulary");
		invertedLists = new SequenceFile<Pair>(directory + "/index", false);
		docsFile = new SequenceFile<Doc>(directory + "/docs", false);
		docLengthFile = new SequenceFile<int>(directory + "/doclength", false);
		pageRankFile = new  SequenceFile<double>(directory + "/pagerank", false);

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
		delete [] pairs;

		return docfreqs;
	}

	Pair* getInvertedList(Term* term){
		Pair* pairs = new Pair[term->getFieldFrequency(CONTENT)];
		invertedLists->setPosition(term->getFieldListPosition(CONTENT));
		invertedLists->readBlock(pairs, term->getFieldFrequency(CONTENT));
		return pairs;
	}
	
	Doc getDoc(int docId){
		docsFile->setPosition(docId-1);
		Doc doc = docsFile->read();
		return doc;
	}
	
	double getPageRank(int docId){
		pageRankFile->setPosition(docId-1);
		return pageRankFile->read();
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
