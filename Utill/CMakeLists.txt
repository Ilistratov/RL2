cmake_minimum_required (VERSION 3.8)

set(SRCS 
	Logger.h
	Logger.cpp
	UtillFunctions.h
	UtillFunctions.cpp
)

add_library(Utill ${SRCS})
set_property(TARGET Utill PROPERTY CXX_STANDARD 17)
