INCLUDE_DIRECTORIES(${TP_SOURCE_DIR}/riCode)
INCLUDE_DIRECTORIES(${TP_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(/usr/local/include/Wt)

AUX_SOURCE_DIRECTORY(index INDEX)
AUX_SOURCE_DIRECTORY(textanalysis TEXTANALYSIS)
AUX_SOURCE_DIRECTORY(search SEARCH)
AUX_SOURCE_DIRECTORY(util UTIL)

ADD_LIBRARY(TP ${INDEX} ${TEXTANALYSIS} ${SEARCH} ${UTIL})

ADD_EXECUTABLE(indexer Indexer.cpp)
TARGET_LINK_LIBRARIES(indexer TP z htmlcxx RICPlib)
				
ADD_EXECUTABLE(searcher Searcher.cpp)
TARGET_LINK_LIBRARIES(searcher TP)
				
ADD_EXECUTABLE(url_identifier UrlIdentifier.cpp)
				
ADD_EXECUTABLE(gerar_consultas GerarConsultas.cpp)
TARGET_LINK_LIBRARIES(gerar_consultas TP)
				
ADD_EXECUTABLE(websearcher.wt WebSearcher.cpp)
TARGET_LINK_LIBRARIES(websearcher.wt TP ${WT_CONNECTOR} wt boost_signals)
