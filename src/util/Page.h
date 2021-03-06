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
#include <htmlcxx/html/Uri.h>
#include <htmlcxx/html/CharsetConverter.h>

using namespace std;
using namespace htmlcxx;

class Page {
  public:

    Page(string url, string httpResponse);

    string getText();
    string getUrl();
    string getTitle();
    string getKeywords();
    string getDescription();
    string getContentType();
    map<string, string> getLinks();

  private:

    void parseHTML(string html);

    string url_;
    string text_;
    string title_;
    string keywords_;
    string description_;
    string contentType_;
    map<string, string> links_;
	
};

inline string Page::getText(){
    return text_;
}

inline string Page::getUrl(){
    return url_;
}

inline string Page::getTitle(){
    return title_;
}

inline string Page::getKeywords(){
    return keywords_;
}

inline string Page::getDescription(){
    return description_;
}

inline string Page::getContentType(){
    return contentType_;
}

inline map<string, string> Page::getLinks(){
    return links_;
}

#endif /* PAGE_H_ */
