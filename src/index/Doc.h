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
#define TITLE_SIZE 2000
#define DESCRIPTION_SIZE 4000

using namespace std;

class Doc {
public:
	int id;
	char url[1000];
	char title[2000];
	char description[3000];

	Doc() {}
	
	Doc(int id_, string urlStr, string titleStr, string descriptionStr){
		id = id_;
		truncString(urlStr, url, URL_SIZE);
		truncString(titleStr, title, TITLE_SIZE);
		truncString(descriptionStr, description, DESCRIPTION_SIZE);
	}

	void truncString(string str, char* c_str, int size){
		size_t strSize = str.copy(c_str, size);
		c_str[strSize]='\0';
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
