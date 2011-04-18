#include <iostream>
#include <string>
#include <algorithm>
#include <zlib.h>
#include "CollectionReader.h"
#include "Document.h"
#include "TextTokenizer.h"
#include "IndexWriter.h"

#define MAX_DOCS 1000

using namespace std;
using namespace RICPNS;	

int main(int argc, char* argv[]){
	
	string inputDirectory("/home/aecio/riCollection");
	string indexFileName("index.txt");

//	string inputDirectory("/home/aecio/toyExample");
//	string indexFileName("indexToCompressedColection.txt");

	CollectionReader * reader = new CollectionReader(inputDirectory,
													 indexFileName);
	Document doc;
	doc.clear();

	IndexWriter* iw = new IndexWriter("./indice/");
	int docsIndexed = 0;
	cout << "Indexing documents..." << endl;
	while(reader->getNextDocument(doc) && docsIndexed < MAX_DOCS){
		
		docsIndexed++;
//		cout <<	docsIndexed;
//		cout <<" "<< doc.getURL() << endl;
		
		string html =  doc.getText();
//		html =  "<html><head><title>Título da página</title></head><body><h1>Cabeçalho</h1>\n<p>Texto do parágrafo</p></body></html>";
		
		iw->addDocument(html);

		doc.clear();
	}
	iw->commit();
	return 0;
}

