#include <string>
#include <iostream>
#include <algorithm>
#include "TextTokenizer.h"
#include "Stopwords.h"

using namespace std;

TextTokenizer::TextTokenizer(string& text_){
	stopfile = new Stopwords("stopwords");
	text = text_;
	std::transform(text.begin(), text.end(), text.begin(), ::tolower);
	tokenStart = 0;
	currentChar = 0;
}

bool TextTokenizer::hasNext(){
	if(currentChar < text.size())
		return true;
	else
		return false;
}

bool TextTokenizer::isAlphaNum(char& ch){
	if( (ch >= 48 && ch <= 57) || (ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122) ){
		return true;
	}else{
		return false;
	}
}

bool TextTokenizer::isSpecialChar(char& ch){
	if( ch == -61 ){
		currentChar++;
		return true;
	}else{
		return false;	
	}
}

string TextTokenizer::nextToken(){
	string token;
	
	for(; currentChar < text.size(); currentChar++){
		
		char ch = text[currentChar];
		if( isAlphaNum(ch) || isSpecialChar(ch)){
			tokenStart = currentChar;
			do {
				currentChar++;
				ch = text[currentChar];
			} while( isAlphaNum(ch) || isSpecialChar(ch) );
			
			token = text.substr(tokenStart, currentChar-tokenStart);
			
			if(stopfile->isStopword(token))
				continue;
			else
				break;
		}
		
	}
//	cout << token << endl;
	return token;
}
