# Trabalho Prático 2 - Recuperação de Informação
# author: Aécio Solano Rodrigues Santos
# email: aeciosantos@dcc.ufmg.br

PROJECT(TP)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# Força utilizar o compilador GCC 4.5, devido a bug de versões anteriores com Wt
INCLUDE(CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER(gcc-4.5 GNU)
CMAKE_FORCE_CXX_COMPILER(g++-4.5 GNU)

SET (CMAKE_C_FLAGS "-Wall -Werror -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE")
SET (CMAKE_CXX_FLAGS "-Wall -Werror -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE")

SET (WT_CONNECTOR "wthttp" CACHE STRING "Connector used (wthttp or wtfcgi)")

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(riCode)
ADD_SUBDIRECTORY(test)
