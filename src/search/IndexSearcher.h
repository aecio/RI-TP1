/*
 * IndexSearcher.h
 *
 *  Created on: 20/05/2011
 *      Author: aecio
 */

#ifndef INDEXSEARCHER_H_
#define INDEXSEARCHER_H_

#include <vector>
#include <string>
#include <search/Hit.h>

using namespace std;

class IndexSearcher {
public:
	virtual vector<Hit> search(string) = 0;
	virtual vector<Hit> search(string, int) = 0;
};

#endif /* INDEXSEARCHER_H_ */
