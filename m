#!/bin/bash
g++ -g -Wall -Wextra -O2 -std=c++17 -c wallet.cc -o wallet.o
g++ -g -Wall -Wextra -O2 -std=c++17 -c main.cc -o main.o
g++ -g main.o wallet.o -o main
