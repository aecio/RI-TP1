/*
 * OcurrenceFile.h
 *
 *  Created on: 15/04/2011
 *      Author: aecio
 */

#ifndef OCCURRENCEFILE_H_
#define OCCURRENCEFILE_H_

#include <fstream>
#include <cstdio>	
#include <string>
#include "Occurrence.h"

using namespace std;

class OccurrenceFile {

	int size;
	string name;
	fstream file;
	
public:

	class FileClosedException {};
	
	OccurrenceFile(string fileName){
		name = fileName;
		file.open(name.c_str(), ios::in | ios::out | ios::trunc | ios::binary );
		size = 0;
	}
	
	int write(Occurrence& oc){
		ensureFileIsOpen();
		file.write((char *) &oc, sizeof(Occurrence)); // grava no arquivo
		size++;
//		printOccurrence(oc, "write");
		return size;
	}
	
	int writeBlock(Occurrence *oc, int blockSize){
		ensureFileIsOpen();
		file.write((char *) oc, sizeof(Occurrence)*blockSize); // grava no arquivo
		size += blockSize;
		return size;
	}
	
	int readBlock(Occurrence *oc, int blockSize){
		ensureFileIsOpen();
		file.read((char*) oc, sizeof(Occurrence) * blockSize );
		if( file.eof() )
			return file.gcount()/sizeof(Occurrence);
		else
			return blockSize;
	}
	
	Occurrence read(){
		ensureFileIsOpen();
		Occurrence oc;
		file.read((char*) &oc, sizeof(Occurrence)); // lê do arquivo
//		printOccurrence(oc, "read");
		return oc;
	}
	
	void printOccurrence(Occurrence oc, string operation){
		cout << name;
		cout << ' ';
		cout << operation;
		cout << ' ';
		cout << '(';
		cout << oc.termId;
		cout << ' ';
		cout << oc.docId;
		cout << ' ';
		cout << oc.termFrequencyInDoc;
		cout << ')';
		cout << endl;
	}
	
	void rewind(){
		ensureFileIsOpen();
		file.seekg(0, ios::beg);
		file.seekp(0, ios::beg);
	}
	
	void ensureFileIsOpen(){
		if( !file.is_open() ) throw FileClosedException();
	}
	
	bool hasNext(){
		ensureFileIsOpen();
		if(getPosition() >= size){
			return false;
		} else {
			return true;
		}
	}
	
	bool eof(){
		if(file.eof())
			return true;
		else
			return false;
	}
	
	int getPosition(){
		ensureFileIsOpen();
		int getPointer = file.tellg();
		if(getPointer == 0)
			return getPointer;
		else
			return getPointer/sizeof(Occurrence);
	}
	
	int getSize(){
		ensureFileIsOpen();
		return size;
	}
	
	string getName(){
		return name;
	}
	
	void reopen(){
		close();
		file.open(name.c_str(), ios::in | ios::out | ios::binary | ios::ate);
		int fileSize = file.tellg();
		size = fileSize / sizeof(Occurrence);
		rewind();
	}
	
	void close(){
		if(file.is_open()){
			file.close();
		}
	}
	
	void deleteFile(){
		close();
		remove(name.c_str());
	}
	
};

#endif /* OCCURRENCEFILE_H_ */
