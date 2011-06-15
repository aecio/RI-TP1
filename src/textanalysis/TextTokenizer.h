/*
 * TextTokenizer.h
 *
 *  Created on: 13/04/2011
 *      Author: aecio
 */

#ifndef TEXTTOKENIZER_H_
#define TEXTTOKENIZER_H_

#include <string>
#include "Stopwords.h"

using namespace std;


class TextTokenizer {

	static Stopwords* stopfile;

	string text;
	unsigned int tokenStart;
	unsigned int currentChar;
	
	bool isSpecialChar(char& ch);
	bool isAlphaNum(char& ch);
	
public:
	TextTokenizer(string&);
	~TextTokenizer();
	
	bool hasNext();
	string nextToken();
	void removeAccents(string &str);
};

#endif /* TEXTTOKENIZER_H_ */
