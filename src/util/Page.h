/*
 * Page.h
 *
 *  Created on: 29/04/2011
 *      Author: aecio
 */

#ifndef PAGE_H_
#define PAGE_H_

#include <string>
#include <map>
#include <boost/algorithm/string.hpp>
#include <htmlcxx/html/Node.h>
#include <htmlcxx/html/ParserDom.h>
#include <htmlcxx/html/utils.h>
#include "htmlcxx/html/Uri.h"
#include "htmlcxx/html/CharsetConverter.h"

using namespace std;
using namespace htmlcxx;

class Page {

	string url;
	string text;
	string title;
	string keywords;
	string description;
	string contentType;
	map<string, string> links;

	void parseHTML(string html){
		HTML::ParserDom parser;
		tree<HTML::Node> dom = parser.parseTree(html);
		tree<HTML::Node>::iterator it = dom.begin();
		text += url;
		for (; it != dom.end(); ++it) {
			if(it.node != 0 && dom.parent(it) != NULL){
				string parent_tag = dom.parent(it)->tagName();
				//Pular código javascript
				boost::to_lower(parent_tag);
				if(parent_tag == "script" || parent_tag == "noscript" ){
					it.skip_children();
					continue;
				}
			}
			//Parse plain text of the page
			if ((!it->isTag()) && (!it->isComment()) ) {
				text += " ";
				text += it->text();
			}
			else { //Parse metadata
				string tagName = it->tagName();
				boost::to_lower(tagName);
				if(tagName == "title"){
					it++;
					if(it == dom.end()) return;
					title = it->text();
				}
				else if(tagName == "meta"){
					it->parseAttributes();
					std::pair<bool, std::string> attrib = it->attribute("name");
					if(attrib.first == true){
						boost::to_lower(attrib.second);
						if(attrib.second == "description")
							description = it->attribute("content").second;
						if(attrib.second == "keywords")
							keywords = it->attribute("content").second;
					}
					attrib = it->attribute("http-equiv");
					boost::to_lower(attrib.second);
					if(attrib.first == true && attrib.second == "content-type"){
						contentType = it->attribute("content").second;
					}
				}
				else if(tagName == "a"){
					it->parseAttributes();
					std::pair<bool, std::string> attrib = it->attribute("rel");
					boost::to_lower(attrib.second);
					if(attrib.first == true && attrib.second == "nofollow"){

					}else{
						attrib = it->attribute("href");
						string anchor_text;
						int children = it.number_of_children();
						for(int i=0; i<children; i++){
							it++;
							if(it == dom.end()) return;
							if(!it->isTag()) anchor_text += it->text();
						}
						links[HTML::convert_link(attrib.second, url)] = anchor_text;
					}
				}
			}
		}
	}
	
public:

	Page(string url_, string httpResponse){
		url = url_;
//		const char* text = httpResponse.c_str();
//		if( HTML::detect_utf8(text, httpResponse.size()) == false ){
//			CharsetConverter cc("8859_1", "UTF8");
//			httpResponse = string(cc.convert(text));
//		}
		parseHTML(HTML::single_blank(HTML::decode_entities(httpResponse)));
	}


	string getText(){
		return text;
	}

	string getUrl(){
		return url;
	}

	string getTitle(){
		return title;
	}

	string getKeywords(){
		return keywords;
	}

	string getDescription(){
		return description;
	}

	string getContentType(){
		return contentType;
	}

	map<string, string> getLinks(){
		return links;
	}
	
};

#endif /* PAGE_H_ */
