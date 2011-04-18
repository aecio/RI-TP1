/*
 * SequenceFile.h
 *
 *  Created on: 15/04/2011
 *      Author: aecio
 */

#ifndef SEQUENCEFILE_H_
#define SEQUENCEFILE_H_

#include <fstream>
#include <cstdio>	
#include <string>

using namespace std;

template <class T>
class SequenceFile {

	int size;
	string name;
	fstream file;
	
public:

	class FileClosedException {};
	
	SequenceFile(string fileName, bool trunc = true){
		name = fileName;
		if(trunc){
			file.open(name.c_str(), ios::in | ios::out | ios::binary | ios::trunc);
			size = 0;
		} else{
			file.open(name.c_str(), ios::in | ios::out | ios::binary | ios::ate);
			int fileSize = file.tellg();
			size = fileSize / sizeof(T);
			rewind();
		}
	}
	
	int write(T& oc){
		ensureFileIsOpen();
		file.write((char *) &oc, sizeof(T)); // grava no arquivo
		size++;
		return size;
	}
	
	int writeBlock(T *oc, int blockSize){
		ensureFileIsOpen();
		file.write((char *) oc, sizeof(T)*blockSize); // grava no arquivo
		size += blockSize;
		return size;
	}
	
	T read(){
		ensureFileIsOpen();
		T oc;
		file.read((char*) &oc, sizeof(T)); // lê do arquivo
		return oc;
	}
	
	int readBlock(T *oc, int blockSize){
		ensureFileIsOpen();
		file.read((char*) oc, sizeof(T) * blockSize );
		if( file.eof() )
			return file.gcount()/sizeof(T);
		else
			return blockSize;
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
			return getPointer/sizeof(T);
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
		size = fileSize / sizeof(T);
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
