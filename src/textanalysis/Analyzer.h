/*
 * Analyzer.h
 *
 *  Created on: 08/06/2011
 *      Author: aecio
 */

#ifndef ANALYZER_H_
#define ANALYZER_H_

#include "textanalysis/TextTokenizer.h"

using namespace std;

class Analyzer {
	int length;
	map<string, int> terms;

public:
	Analyzer(): length(0) {}

	Analyzer(string text): length(0) {
		analyze(text);
	}

	void analyze(string text){
		length = 0;
		terms.clear();
		TextTokenizer tokenizer(text);
		while(tokenizer.hasNext()){ 			//para cada termo "indexável"
			string t = tokenizer.nextToken();
			if(t == "") continue;
			terms[t]++; 						//contabilizar frequencia do termo neste doc
			length++;
		}
	}

	map<string, int> getTermFreqs(){
		return terms;
	}


	int getLength(){
		return length;
	}
};

#endif /* ANALYZER_H_ */
