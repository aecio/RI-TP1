# CmakeLists.txt in src dir

INCLUDE_DIRECTORIES(${TP1_SOURCE_DIR}/riCode)
INCLUDE_DIRECTORIES(${TP1_SOURCE_DIR}/entities_html)

ADD_EXECUTABLE(	indexer Main.cpp 
				TextTokenizer.h TextTokenizer.cpp
				IndexWriter.h IndexWriter.cpp
				Term.h Term.cpp
				Occurrence.h)

TARGET_LINK_LIBRARIES(indexer z htmlcxx RICPlib)
