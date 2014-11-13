# Makefile for Programming Assignment 3
# Compiler: G++

all: threadtest semtest spooler

threadtest : mythread.cpp mythread.hpp
	g++ -g -w -o threadtest threadtest.cpp mythread.cpp

semtest : mythread.cpp mythread.hpp
	g++ -g -w -o semtest semtest.cpp mythread.cpp

spooler : spooler.cpp mythread.hpp
	g++ -g -w -o spooler spooler.cpp mythread.cpp
