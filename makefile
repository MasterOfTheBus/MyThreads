# Makefile for Programming Assignment 3
# Compiler: G++

threadtest : mythread.cpp mythread.hpp
	g++ -g -o threadtest threadtest.cpp mythread.cpp

semtest : mythread.cpp mythread.hpp
	g++ -g -o semtest semtest.cpp mythread.cpp
