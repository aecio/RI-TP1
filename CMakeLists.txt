# Trabalho Prático 1 - Recuperação de Informação
# author: Aécio Solano Rodrigues Santos
# email: aeciosantos@dcc.ufmg.br

PROJECT(TP1)
cmake_minimum_required(VERSION 2.6)

set (CMAKE_C_FLAGS "-Wall -Werror")
set (CMAKE_CXX_FLAGS "-Wall -Werror")

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(riCode)
ADD_SUBDIRECTORY(test)

