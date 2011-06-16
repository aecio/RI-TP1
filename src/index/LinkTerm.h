/*
 * LinkTerm.h
 *
 *  Created on: 13/06/2011
 *      Author: aecio
 */

#ifndef LINKTERM_H_
#define LINKTERM_H_

class LinkTerm {
	char link[URL_SIZE];
	int terms_number;
public:

	LinkTerm(): terms_number(0){}

	LinkTerm(string linkStr, int termsNumber){
		terms_number = termsNumber;
		truncString(linkStr, link, URL_SIZE);
	}

	void truncString(string str, char* c_str, int maxSize){
		int length = str.copy(c_str, maxSize);
		c_str[length] = '\0';
	}

	string getLink(){
		return string(link);
	}

	int getTermsNumber(){
		return terms_number;
	}
};

#endif /* LINKTERM_H_ */
