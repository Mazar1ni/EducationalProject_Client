cmake_minimum_required (VERSION 3.11)

set (PROJECT Client) 
 
project (${PROJECT}) 
 
set (HEADERS 
	Client/Log.h)
 
set (SOURCES 
	Client/Log.cpp
	Client/main.cpp) 
 
add_executable (${PROJECT} ${HEADERS} ${SOURCES})