/*
 * Doc.h
 *
 *  Created on: 09/05/2011
 *      Author: aecio
 */

#ifndef DOC_H_
#define DOC_H_

#include <string>

using namespace std;

class Doc {
public:
	char url[1000];

	Doc() {}
	
	Doc(string strUrl){
		size_t length;
		length = strUrl.copy(url, strUrl.size(), 0);
		url[length]='\0';
	}
};

#endif /* DOC_H_ */
