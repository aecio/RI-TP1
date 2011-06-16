INCLUDE_DIRECTORIES(${TP_SOURCE_DIR}/riCode)
INCLUDE_DIRECTORIES(${TP_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(/usr/local/include/Wt)

SET (PROJECT_SOURCE index/IndexWriter.h
					index/IndexWriter.cpp
					index/Term.h
					index/Pair.h
					index/Vocabulary.h
					index/Doc.h
					textanalysis/TextTokenizer.h
					textanalysis/TextTokenizer.cpp
					textanalysis/Stopwords.h
					textanalysis/Stopwords.cpp
					search/BM25IndexSearcher.h
					search/VSMIndexSearcher.h
					search/BooleanIndexSearcher.h
					search/PageRankSearcher.h
					search/CombinedIndexSearcher.h
					util/Page.h
					util/SequenceFile.h)

ADD_LIBRARY (TP ${PROJECT_SOURCE})

ADD_EXECUTABLE (indexer Indexer.cpp)
TARGET_LINK_LIBRARIES(indexer TP z htmlcxx RICPlib)
				
ADD_EXECUTABLE (searcher Searcher.cpp)
TARGET_LINK_LIBRARIES(searcher TP)
				
ADD_EXECUTABLE (url_identifier UrlIdentifier.cpp)
				
ADD_EXECUTABLE (gerar_consultas GerarConsultas.cpp)
TARGET_LINK_LIBRARIES (gerar_consultas TP)
				
ADD_EXECUTABLE (websearcher.wt WebSearcher.cpp)
TARGET_LINK_LIBRARIES (websearcher.wt TP ${WT_CONNECTOR} wt boost_signals)
