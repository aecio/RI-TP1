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

using namespace std;

class Doc {
public:
	int id;
	char url[1000];
	int length;

	Doc() {
		length = 0;
	}
	
	Doc(int id_, string strUrl, int docLength){
		id = id_;
		size_t urlSize;
		urlSize = strUrl.copy(url, strUrl.size(), 0);
		url[urlSize]='\0';
		
		length = docLength;
	}
};

#endif /* DOC_H_ */
