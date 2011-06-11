/*
 * Doc.h
 *
 *  Created on: 09/05/2011
 *      Author: aecio
 */

#ifndef DOC_H_
#define DOC_H_

#include <string>
#include <stdlib.h>

#define URL_SIZE 1000
#define TITLE_SIZE 300
#define DESCRIPTION_SIZE 4000

using namespace std;

class Doc {
public:
	int id;
	char url[URL_SIZE];
	char title[TITLE_SIZE];
	char description[DESCRIPTION_SIZE];

	Doc() {}
	
	Doc(int id_, string urlStr, string titleStr, string descriptionStr){
		id = id_;
		truncString(urlStr, url, URL_SIZE);
		truncString(titleStr, title, TITLE_SIZE);
		truncString(descriptionStr, description, DESCRIPTION_SIZE);
	}

	void truncString(string str, char* c_str, int maxSize){
		int length = str.copy(c_str, maxSize);
		c_str[length] = '\0';
	}

	int getId(){
		return id;
	}

	string getUrl(){
		return string(url);
	}

	string getTitle(){
		return string(title);
	}

	string getDescription(){
		return string(description);
	}

};

#endif /* DOC_H_ */
