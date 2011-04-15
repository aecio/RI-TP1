/*
 * IndexWriter.cpp
 *
 *  Created on: 13/04/2011
 *      Author: aecio
 */
 
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>
#include <htmlcxx/html/Node.h>
#include <htmlcxx/html/ParserDom.h>
#include "IndexWriter.h"
#include "TextTokenizer.h"

using namespace std;
using namespace htmlcxx;
	
IndexWriter::IndexWriter(string directory_){
	docId = 0;
	termId = 0;
	occurrencesCount = 0;
	directory = directory_;
	fout.open("triplas.txt");
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
	

Term& IndexWriter::getVocabularyTerm(string t) {
	map<string, Term>::iterator entry = vocabulary.find(t);
	if(entry == vocabulary.end() ){
		Term& term = vocabulary[t];
		term.setId(++termId);
		return term;
	} else {
		return entry->second;
	}
}

void IndexWriter::addDocument(string& html){
	
	string text = extractTextFrom(html);
	TextTokenizer tokenizer(text);
	map<string, int> docFrequencies;
	docId++;
	
	while(tokenizer.hasNext()){ //para cada termo "indexável"
		string t = tokenizer.nextToken();
		docFrequencies[t]++; //contabilizar frequencia do termo neste doc
	}
	
	map<string, int>::iterator it = docFrequencies.begin();
	for(; it != docFrequencies.end(); it++){
		string t = it->first;
		Term& term = getVocabularyTerm(t);
		
		Occurrence oc(term.getId(), docId, it->second);
		write(oc);
		occurrencesCount++;
		
//		occurrences.push_back(  );
	}
}

void IndexWriter::write(Occurrence& it){
	fout.write((char *) &it, sizeof(Occurrence)); // grava no arquivo
}

Occurrence IndexWriter::read(){
	Occurrence oc;
	fout.read((char*) &oc, sizeof(Occurrence)); // lê do arquivo
	return oc;
}

void IndexWriter::commit() {
	//finalizar criação do índice no disco
	cout << "Reading occurrences file..." << endl;
	occurrences.clear();
	fout.seekg(0, ios::beg);
	for(int i=0; i<occurrencesCount; i++){
		Occurrence it = read();
		occurrences.push_back(it);
	}
	
	cout << "Ordering..." << endl;
	occurrences.sort();
	
	cout << "Flushing occurrences to disk..." << endl;
	fout.seekp(0, ios::beg);
	list<Occurrence>::iterator it = occurrences.begin();
	for(; it != occurrences.end(); it++){
		write(*it);
	}
	cout << "Done." << endl;

}

//void IndexWriter::printOccurence(Occurrence& it){
//		cout << it.termId;
//		cout << ' ';
//		cout << it.docId;
//		cout << ' ';
//		cout << it.termFrequencyInDoc;
//		cout << '\n';
//}

