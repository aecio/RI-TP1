# CmakeLists.txt in src dir

INCLUDE_DIRECTORIES(${TP1_SOURCE_DIR}/riCode)

ADD_EXECUTABLE(	indexer Main.cpp 
				TextTokenizer.h TextTokenizer.cpp
				IndexWriter.h IndexWriter.cpp
				Term.h
				Pair.h
				Vocabulary.h
				SequenceFile.h
				VSMIndexSearcher.h)
				
ADD_EXECUTABLE(	searcher Searcher.cpp
				Pair.h
				Vocabulary.h
				SequenceFile.h
				TextTokenizer.h TextTokenizer.cpp
				VSMIndexSearcher.h)
				
ADD_LIBRARY(TP1 TextTokenizer.h TextTokenizer.cpp
				IndexWriter.h IndexWriter.cpp
				Term.h
				Pair.h
				Vocabulary.h
				SequenceFile.h
				VSMIndexSearcher.h)
				
				

TARGET_LINK_LIBRARIES(indexer z htmlcxx RICPlib)
