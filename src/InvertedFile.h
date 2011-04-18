/*
 * InvertedFile.h
 *
 *  Created on: 15/04/2011
 *      Author: aecio
 */

#ifndef INVERTEDFILE_H_
#define INVERTEDFILE_H_

#include <fstream>
#include <cstdio>	
#include <string>
#include "Pair.h"

using namespace std;

class InvertedFile {

	int size;
	string name;
	fstream file;
	
public:

	class FileClosedException {};
	
	InvertedFile(string fileName, bool trunc = true){
		name = fileName;
		if(trunc){
			file.open(name.c_str(), ios::in | ios::out | ios::binary | ios::trunc);
			size = 0;
		} else{
			file.open(name.c_str(), ios::in | ios::out | ios::binary | ios::ate);
			int fileSize = file.tellg();
			size = fileSize / sizeof(Pair);
			rewind();
		}
	}
	
	int write(Pair& oc){
		ensureFileIsOpen();
		file.write((char *) &oc, sizeof(Pair)); // grava no arquivo
		size++;
		printPair(oc, "write");
		return size;
	}
	
	int writeBlock(Pair *oc, int blockSize){
		ensureFileIsOpen();
		file.write((char *) oc, sizeof(Pair)*blockSize); // grava no arquivo
		size += blockSize;
		return size;
	}
	
	Pair read(){
		ensureFileIsOpen();
		Pair oc;
		file.read((char*) &oc, sizeof(Pair)); // lê do arquivo
		printPair(oc, "read");
		return oc;
	}
	
	int readBlock(Pair *oc, int blockSize){
		ensureFileIsOpen();
		file.read((char*) oc, sizeof(Pair) * blockSize );
		if( file.eof() )
			return file.gcount()/sizeof(Pair);
		else
			return blockSize;
	}
	
	void printPair(Pair oc, string operation){
		cout << name;
		cout << ' ';
		cout << operation;
		cout << ' ';
		cout << '(';
		cout << oc.docId;
		cout << ' ';
		cout << oc.frequency_dt;
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
			return getPointer/sizeof(Pair);
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
		size = fileSize / sizeof(Pair);
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

#endif /* INVERTEDFILE_H_ */
