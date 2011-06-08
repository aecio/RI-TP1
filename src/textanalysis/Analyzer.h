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
	Analyzer(string & text){
		length = 0;
		TextTokenizer tokenizer(text);
		while(tokenizer.hasNext()){ 			//para cada termo "indexável"
			string t = tokenizer.nextToken();
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
