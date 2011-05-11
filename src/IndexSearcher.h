/*
 * IndexSearcher.h
 *
 *  Created on: 18/04/2011
 *      Author: aecio
 */

#ifndef INDEXSEARCHER_H_
#define INDEXSEARCHER_H_

#include <string>
#include <vector>
#include <algorithm>
#include "SequenceFile.h"
#include "Pair.h"
#include "Vocabulary.h"
#include "TextTokenizer.h"
#include "Doc.h"

class IndexSearcher {
	Vocabulary* vocabulary;
	SequenceFile<Pair>* invertedLists;
	SequenceFile<Doc>* pagesFile;
public:
	
	enum{AND, OR};

	IndexSearcher(string directory) {
		vocabulary = new Vocabulary(directory + "/vocabulary");
		invertedLists = new SequenceFile<Pair>(directory + "/index", false);
		pagesFile = new SequenceFile<Doc>(directory + "/urls", false);
	}
	
	vector<Doc> readDocs(Pair* pairs, int n){
		vector<Doc> docs;
		for(int i=0; i < n; i++){
			pagesFile->setPosition(pairs[i].docId-1);
			Doc d = pagesFile->read();
			docs.push_back(d);
		}
		return docs;
	}
	
	vector<Doc> readDocs(vector<Pair> pairs){
		vector<Doc> docs;
		for(unsigned int i=0; i < pairs.size(); i++){
			pagesFile->setPosition(pairs[i].docId-1);
			Doc d = pagesFile->read();
			docs.push_back(d);
		}
		return docs;
	}
	
	
	vector<Doc> singleQuery(Term* term){
		vector<Doc> hits;
		if(term != NULL) {
			Pair* pairs = readInvertedList(term);
			hits = readDocs(pairs, term->docFrequency);
		}
		return hits;
	}
	
	vector<Doc> doubleQuery(Term* term1, Term* term2, int conector){
		vector<Doc> hits;
		if(conector == AND && (term1 == NULL || term2 == NULL ) )
			return hits;
			
		if(conector == OR && term1 == NULL)
			return singleQuery(term2);
			
		if(conector == OR && term2 == NULL)
			return singleQuery(term1);
			
		
		Pair* pairs1 = readInvertedList(term1);
		Pair* pairs2 = readInvertedList(term2);
	
		if(conector == AND) {
			vector<Pair> result = intersectionSet( pairs1, term1->docFrequency,
													pairs2, term2->docFrequency);
			hits = readDocs(result);
		} else {
			vector<Pair> result = unionSet( pairs1, term1->docFrequency,
					 						pairs2, term2->docFrequency);
			hits = readDocs(result);
		}
		return hits;
	}
	
	vector<Doc> search(string query) {
		TextTokenizer tokenizer(query);
		int conector;
		vector<string> tokens;
		while(tokenizer.hasNext()) {
			string token = tokenizer.nextToken();
			if(token == "and"){
				conector = AND;
			}
			else if(token == "or"){
				conector = OR;
			}
			else {
				tokens.push_back(token);
			}
		}
		
		Term* term1 = vocabulary->findTerm(tokens[0]);
		if(tokens.size() == 1) {
			return singleQuery(term1);
		} else {
			Term* term2 = vocabulary->findTerm(tokens[1]);
			return doubleQuery(term1, term2, conector);
		}
	}
	
	Pair* readInvertedList(Term* term){
		Pair* pairs = new Pair[term->docFrequency];
		invertedLists->setPosition(term->listPosition);
		invertedLists->readBlock(pairs, term->docFrequency);
		return pairs;
	}
	
	vector<Pair> intersectionSet(Pair* set1start, int set1size, 
								 Pair* set2start, int set2size ){
		vector<Pair> inter(set1size+set2size);
		vector<Pair>::iterator end = set_intersection(	set1start, set1start+set1size,
														set2start, set2start+set2size,
														inter.begin() );
		vector<Pair> resultSet;
		vector<Pair>::iterator it = inter.begin();
		for(;it != end; it++) {
			resultSet.push_back(*it);
		}
		return resultSet;
	}
	
	vector<Pair> unionSet(Pair* set1start, int set1size, 
						  Pair* set2start, int set2size ){
		vector<Pair> unionset(set1size+set2size);
		vector<Pair>::iterator end = set_union(	set1start, set1start+set1size,
												set2start, set2start+set2size,
												unionset.begin() );
		vector<Pair> resultSet;
		vector<Pair>::iterator it = unionset.begin();
		for(;it != end; it++) {
			resultSet.push_back(*it);
		}
		return resultSet;
	}
	
};

#endif /* PAIR_H_ */
