# Trabalho Prático 2 - Recuperação de Informação
# author: Aécio Solano Rodrigues Santos
# email: aeciosantos@dcc.ufmg.br

PROJECT(TP1)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

include(CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER(gcc-4.5 GNU)
CMAKE_FORCE_CXX_COMPILER(g++-4.5 GNU)


SET (CMAKE_C_FLAGS "-Wall -Werror")
SET (CMAKE_CXX_FLAGS "-Wall -Werror")


#SET (WT_CONNECTOR "wthttp" CACHE STRING "Connector used (wthttp or wtfcgi)")

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(riCode)
ADD_SUBDIRECTORY(test)
