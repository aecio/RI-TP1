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
	FILE* file;
	
public:

	class FileClosedException {};
	
	SequenceFile(string fileName, bool trunc = true){
		open(fileName, trunc);
	}
	
	void open(string fileName, bool trunc = true){
		name = fileName;
		if(trunc){
			file = fopen(name.c_str(), "w+b");
			size = 0;
		} else{
			file = fopen(name.c_str(), "a+b");
			fseek(file, 0, SEEK_END);
			int fileSize = ftell(file);
			size = fileSize / sizeof(T);
			rewind();
		}
	}
	
	int write(T& oc){
		ensureFileIsOpen();
		fwrite((char *) &oc, sizeof(T), 1, file);
		size++;
		return size;
	}
	
	int writeBlock(T *oc, int blockSize){
		ensureFileIsOpen();
		fwrite((char *) oc, sizeof(T), blockSize, file);
		size += blockSize;
		return size;
	}
	
	T read(){
		ensureFileIsOpen();
		T oc;
		fread((char*) &oc, sizeof(oc), 1, file); // lê do arquivo
		return oc;
	}
	
	int readBlock(T *oc, int blockSize){
		ensureFileIsOpen();
		size_t elements_read = fread((char*) oc, sizeof(T), blockSize, file); // lê do arquivo
		return elements_read;
	}
	
	void rewind(){
		ensureFileIsOpen();
		fseek(file, 0, SEEK_SET);
	}
	
	void ensureFileIsOpen(){
		if( file == NULL){
			cout << "File " << name << " is not open!" << endl;
			throw FileClosedException();
		}
	}
	
	bool hasNext(){
		ensureFileIsOpen();
		if(getPosition() >= size){
			return false;
		} else {
			return true;
		}
	}
	
	int getPosition(){
		ensureFileIsOpen();
		int pointer = ftell(file);
		int position = pointer/sizeof(T);
		if(feof(file)){
			return position+1;
		}
		return position;
	}
	
	void setPosition(int position){
		ensureFileIsOpen();
		fseek(file, position*sizeof(T), SEEK_SET);
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
		open(name, false);
	}
	
	void close(){
		if(file != NULL){
			fclose(file);
			file = NULL;
		}
	}
	
	void deleteFile(){
		close();
		remove(name.c_str());
	}
	
};

#endif /* INVERTEDFILE_H_ */
