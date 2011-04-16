# CmakeLists.txt in src dir

INCLUDE_DIRECTORIES(${TP1_SOURCE_DIR}/riCode)

ADD_EXECUTABLE(	indexer Main.cpp 
				TextTokenizer.h TextTokenizer.cpp
				IndexWriter.h IndexWriter.cpp
				Term.h Term.cpp
				Occurrence.h OccurrenceFile.h)
				
ADD_LIBRARY(TP1 TextTokenizer.h TextTokenizer.cpp
				IndexWriter.h IndexWriter.cpp
				Term.h Term.cpp
				Occurrence.h OccurrenceFile.h)

TARGET_LINK_LIBRARIES(indexer z htmlcxx RICPlib)
