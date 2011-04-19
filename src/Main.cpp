#include <iostream>
#include <string>
#include <algorithm>
#include <zlib.h>
#include "CollectionReader.h"
#include "Document.h"
#include "TextTokenizer.h"
#include "IndexWriter.h"

#define MAX_DOCS 99999999
#define RUN_SIZE 200000

using namespace std;
using namespace RICPNS;	

int main(int argc, char* argv[]){
	
	string inputDirectory("/home/aecio/riCollection");
	string indexFileName("index.txt");
	string indexDirectory(".");
	int runSize = RUN_SIZE;
	int numDocs = MAX_DOCS;
	
	//Parse comand line arguments
	for(int i=0; i<argc; i++){
		string param(argv[i]);
		if(param == "--directory" || param == "-d"){
			i++;
			inputDirectory = string(argv[i]);
		}
		else if(param == "--fileName" || param == "-f"){
			i++;
			indexFileName = string(argv[i]);
		}
		else if(param == "--runSize" || param == "-r"){
			i++;
			if(sscanf(argv[i], "%d", &runSize) == EOF){
				//Erro na conversão, então usar valor padrão
				runSize = RUN_SIZE;
			}
		} else if(param == "--numDocs" || param == "-n"){
			i++;
			if(sscanf(argv[i], "%d", &numDocs) == EOF){
				//Erro na conversão, então usar valor padrão
				numDocs = MAX_DOCS;
			}
		} else if(param == "-o" || param == "--output"){
			i++;
			indexDirectory = string(argv[i]);
		}
	}

	CollectionReader * reader = new CollectionReader(inputDirectory,
													 indexFileName);
	Document doc;
	doc.clear();

	IndexWriter* iw = new IndexWriter(indexDirectory, runSize);
	int docsIndexed = 0;
	cout << "Indexing documents..." << endl;
	while(reader->getNextDocument(doc) && docsIndexed < numDocs){
		docsIndexed++;
		
		if(docsIndexed % 10000 == 0) {
			cout <<	docsIndexed << " documents..." << endl;
		}
		
		string html = doc.getURL();
		html += " ";
		html += doc.getText();
		iw->addDocument(html);

		doc.clear();
	}
	iw->commit();
	return 0;
}

