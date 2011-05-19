/*
 * Page.h
 *
 *  Created on: 29/04/2011
 *      Author: aecio
 */

#ifndef PAGE_H_
#define PAGE_H_

#include <string>
#include <htmlcxx/html/Node.h>
#include <htmlcxx/html/ParserDom.h>
#include <htmlcxx/html/utils.h>

using namespace std;
using namespace htmlcxx;

class Page {
public:
	string url;
	string text;

	Page() {}
	
	Page(string url_, string text_){
		url = url_;
		text = text_;
	}
	
	string getPlainText(){
		HTML::ParserDom parser;
		tree<HTML::Node> dom = parser.parseTree(text);
		tree<HTML::Node>::iterator it = dom.begin();
		string text;
		text += " ";
		text += url;
		for (; it != dom.end(); ++it) {
			if ((!it->isTag()) && (!it->isComment()) ) {
				text += " ";
				text += it->text();
			}
		}
		return HTML::decode_entities(text);
//		return text;
	}
	
	
};

#endif /* PAGE_H_ */
