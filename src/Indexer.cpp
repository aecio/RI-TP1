#include <algorithm>
#include <iostream>
#include <string>
#include <zlib.h>
#include "../riCode/CollectionReader.h"
#include "../riCode/Document.h"
#include "index/IndexWriter.h"
#include "util/Page.h"

#define MAX_DOCS 99999999
#define RUN_SIZE 5000000

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

	IndexWriter indexWriter(indexDirectory, runSize);

//	cout << "Skipping documents..." << endl;
//	for(int j=0; j<314411; j++){
//		reader->getNextDocument(doc);
//	}
//	cout << "done" << endl;

	int docsIndexed = 0;
	cout << "Indexing documents..." << endl;
	while(reader->getNextDocument(doc) && docsIndexed < numDocs){
		docsIndexed++;
		
		if(docsIndexed % 5000 == 0) {
			cout <<	docsIndexed << " documents indexed..." << endl;
		}
		
		Page p(doc.getURL(), doc.getText());
		indexWriter.addDocument(p);

		doc.clear();
	}
	indexWriter.commit();

	return 0;
}

