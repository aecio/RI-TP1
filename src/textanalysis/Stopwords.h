/*
 * Stopwords.h
 *
 *  Created on: 19/03/2011
 *      Author: aecio
 */

#ifndef STOPWORDS_H_
#define STOPWORDS_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>

using namespace std;

class Stopwords {
	string file;
	set<string> stopwords;
public:
	Stopwords(string);

	set<string> list();
	bool isStopword(string);
};

#endif /* STOPWORDS_H_ */
