/*
 * TextTokenizer.h
 *
 *  Created on: 13/04/2011
 *      Author: aecio
 */

#ifndef TEXTTOKENIZER_H_
#define TEXTTOKENIZER_H_

#include <string>

using namespace std;


class TextTokenizer {
	string text;
	unsigned int tokenStart;
	unsigned int currentChar;
	
	bool isSpecialChar(char& ch);
	bool isAlphaNum(char& ch);
	
public:
	TextTokenizer(string&);
	
	bool hasNext();
	string nextToken();
};

#endif /* TEXTTOKENIZER_H_ */
