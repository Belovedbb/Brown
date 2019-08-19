cmake_minimum_required(VERSION 3.12)
project(Brown)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES src/main.cpp src/token.h src/tokenizer.cpp
	src/tokenizer.h helpers/array_list.h helpers/headers.h 
	helpers/linked_list.h helpers/hash_map.h src/parser.h 
	src/parser.cpp  src/eval.h src/environment.h src/callable.h)

add_executable(Brown ${SOURCE_FILES})