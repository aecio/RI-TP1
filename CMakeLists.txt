# Trabalho Prático 1 - Recuperação de Informação
# author: Aécio Solano Rodrigues Santos
# email: aeciosantos@dcc.ufmg.br
PROJECT(TP1)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET (CMAKE_C_FLAGS "-Wall -Werror")
SET (CMAKE_CXX_FLAGS "-Wall -Werror")


#SET (WT_CONNECTOR "wthttp" CACHE STRING "Connector used (wthttp or wtfcgi)")

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(riCode)
ADD_SUBDIRECTORY(test)

#Resolver bug de compilação do Witty
#ADD_DEFINITIONS(-DNDEBUG)
