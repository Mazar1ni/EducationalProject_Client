cmake_minimum_required (VERSION 3.11)

set (PROJECT Client) 
 
project (${PROJECT}) 
 
set (HEADERS 
	Client/Log.h
	Client/INetwork.h
	Client/UnixClient.h
	Client/WinClient.h)
 
set (SOURCES 
	Client/Log.cpp
	Client/INetwork.cpp
	Client/UnixClient.cpp
	Client/WinClient.cpp
	Client/main.cpp) 
 
add_executable (${PROJECT} ${HEADERS} ${SOURCES})

add_definitions(-std=c++14)