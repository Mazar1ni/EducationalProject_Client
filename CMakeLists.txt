cmake_minimum_required (VERSION 3.11)

set (PROJECT Client) 
 
project (${PROJECT}) 
 
set (HEADERS 
	Client/Log.h
	Client/Client.h)
 
set (SOURCES 
	Client/Log.cpp
	Client/Client.cpp
	Client/main.cpp) 
 
add_executable (${PROJECT} ${HEADERS} ${SOURCES})

add_definitions(-std=c++14)