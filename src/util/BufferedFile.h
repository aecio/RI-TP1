/*
 * BufferedFile.h
 *
 *  Created on: 09/06/2011
 *      Author: aecio
 */

#ifndef BUFFEREDFILE_H_
#define BUFFEREDFILE_H_

#include "util/SequenceFile.h"

template<class T>
class BufferedFile {

	SequenceFile<T>* file;
	T* buffer;
	const int bufferSize;
	const bool freeMemoryAfter;
	int blockSize;
	int j;

	void maybeReadBlock() {
		if (j >= blockSize && file->hasNext()) {
			blockSize = file->readBlock(buffer, bufferSize);
			j = 0;
		}
	}

public:
	BufferedFile(SequenceFile<T>* file_, int bufferSize_):
			bufferSize(bufferSize_),
			freeMemoryAfter(true) {

		file = file_;
		j = 0;
		buffer = new T[bufferSize_];
		blockSize = file->readBlock(buffer, bufferSize);
	}

	BufferedFile(SequenceFile<T>* file_, T* buffer_, int bufferSize_):
				bufferSize(bufferSize_),
				freeMemoryAfter(false) {

		file = file_;
		j = 0;
		buffer = buffer_;
		blockSize = file->readBlock(buffer, bufferSize);
	}

	~BufferedFile() {
		if (freeMemoryAfter) {
			delete [] buffer;
			buffer = NULL;
		}
	}

	bool hasNext() {
		if (j < blockSize || file->hasNext())
			return true;
		else
			return false;
	}

	T read() {
		maybeReadBlock(); //se o buffer acabar, ler outro bloco
		return buffer[j++];
	}
};

#endif /* BUFFEREDFILE_H_ */
