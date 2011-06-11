/*
 * Term.h
 *
 *  Created on: 22/03/2011
 *      Author: aecio
 */

#ifndef TERM_H_
#define TERM_H_

#include <string>
#include "index/Field.h"

using namespace std;

class Term {
	char term[30];

	int titlePosition;
	int titleFrequency;

	int contentFrequency;
	int contentPosition;

	int anchorTextPosition;
	int anchorTextFrequency;

	int keywordsPosition;
	int keywordsFrequency;

	int descriptionPosition;
	int descriptionFrequency;
	
public:

	Term() {
		contentFrequency = 0;
	}
	
	Term(string termStr):	titlePosition(0),
							titleFrequency(0),
							contentFrequency(0),
							contentPosition(0),
							anchorTextPosition(0),
							anchorTextFrequency(0),
							keywordsPosition(0),
							keywordsFrequency(0),
							descriptionPosition(0),
							descriptionFrequency(0) {
		int maxSize;
		if(termStr.size() < 30){
			maxSize = termStr.size();
		} else {
			maxSize = 30;
		}
		termStr.copy(term, maxSize);
		term[maxSize] = '\0';
	}
	
	string getTerm(){
		return string(term);
	}

	void setFieldListPosition(int field, int position){
		switch(field){
			case CONTENT:
				contentPosition = position;
				break;
			case TITLE:
				titlePosition = position;
				break;
			case ANCHOR_TEXT:
				anchorTextPosition = position;
				break;
			case KEYWORDS:
				keywordsPosition = position;
				break;
			case DESCRIPTION:
				descriptionPosition = position;
				break;
		}
	}

	int getFieldListPosition(int field){
		switch(field){
			case CONTENT:
				return contentPosition;
				break;
			case TITLE:
				return titlePosition;
				break;
			case ANCHOR_TEXT:
				return anchorTextPosition;
				break;
			case KEYWORDS:
				return keywordsPosition;
				break;
			case DESCRIPTION:
				return descriptionPosition;
				break;
		}
		return -1;
	}

	int getFieldFrequency(int field){
		switch(field){
			case CONTENT:
				return contentFrequency;
				break;
			case TITLE:
				return titleFrequency;
				break;
			case ANCHOR_TEXT:
				return anchorTextFrequency;
				break;
			case KEYWORDS:
				return keywordsFrequency;
				break;
			case DESCRIPTION:
				return descriptionFrequency;
				break;
		}
		return -1;
	}

	void incrementFieldFrequency(int field){
		switch(field){
			case CONTENT:
				contentFrequency++;
				break;
			case TITLE:
				titleFrequency++;
				break;
			case ANCHOR_TEXT:
				anchorTextFrequency++;
				break;
			case KEYWORDS:
				keywordsFrequency++;
				break;
			case DESCRIPTION:
				descriptionFrequency++;
				break;
		}
	}

};

#endif /* TERM_H_ */
