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
#include <tr1/unordered_set>

using namespace std;
using std::tr1::unordered_set;

class Stopwords {
	string file;
    unordered_set<string> stopwords;
public:
	Stopwords(string);

    unordered_set<string> list();
	bool isStopword(string);
};

#endif /* STOPWORDS_H_ */
