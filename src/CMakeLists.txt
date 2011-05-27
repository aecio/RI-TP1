INCLUDE_DIRECTORIES(${TP1_SOURCE_DIR}/riCode)
INCLUDE_DIRECTORIES(${TP1_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(/usr/local/include/Wt)

ADD_SUBDIRECTORY(index)
ADD_SUBDIRECTORY(search)
ADD_SUBDIRECTORY(textanalysis)

ADD_EXECUTABLE(	indexer Indexer.cpp 
				textanalysis/TextTokenizer.h
				textanalysis/TextTokenizer.cpp
				textanalysis/Stopwords.h
				textanalysis/Stopwords.cpp
				index/IndexWriter.h
				index/IndexWriter.cpp
				index/Term.h
				index/Pair.h
				index/Vocabulary.h
				util/SequenceFile.h
				util/Page.h)
				
ADD_EXECUTABLE(	searcher Searcher.cpp
				index/Pair.h
				index/Vocabulary.h
				textanalysis/TextTokenizer.h
				textanalysis/TextTokenizer.cpp
				textanalysis/Stopwords.h
				textanalysis/Stopwords.cpp
				search/VSMIndexSearcher.h
				search/BooleanIndexSearcher.h
				util/SequenceFile.h)
				
ADD_EXECUTABLE(	urlidentifier urlidentifier.cpp
				index/Doc.h
				util/SequenceFile.h)
				
ADD_EXECUTABLE(	gerar_consultas gerar_consultas.cpp
				index/Doc.h
				search/BM25IndexSearcher.h
				index/Pair.h
				index/Vocabulary.h
				textanalysis/TextTokenizer.h
				textanalysis/TextTokenizer.cpp
				textanalysis/Stopwords.h
				textanalysis/Stopwords.cpp
				search/VSMIndexSearcher.h
				search/BooleanIndexSearcher.h
				util/SequenceFile.h)
				
ADD_LIBRARY(TP1 textanalysis/TextTokenizer.h
				textanalysis/TextTokenizer.cpp
				textanalysis/Stopwords.h
				textanalysis/Stopwords.cpp
				index/IndexWriter.h
				index/IndexWriter.cpp
				index/Term.h
				index/Pair.h
				index/Vocabulary.h
				search/VSMIndexSearcher.h
				search/BooleanIndexSearcher.h
				util/Page.h
				util/SequenceFile.h)

TARGET_LINK_LIBRARIES(indexer z htmlcxx RICPlib)


ADD_EXECUTABLE(	websearcher.wt WebSearcher.cpp)

TARGET_LINK_LIBRARIES(websearcher.wt wt wthttp boost_signals TP1)
