#!/bin/bash
g++ -c -std=c++11 main.cpp
g++ main.o  -o run -lsfml-graphics -lsfml-window -lsfml-system 
$SHELL