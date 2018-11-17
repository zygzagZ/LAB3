#!/bin/bash
g++ -g -Wall -Wextra -O2 -std=c++17 -c wallet.cc -o wallet.o
g++ -g -Wall -Wextra -O2 -std=c++17 -c operation.cc -o operation.o
g++ -g -Wall -Wextra -O2 -std=c++17 -c main.cc -o main.o
g++ -g main.o operation.o wallet.o -o main
// g++ -g -Wall -Wextra -O2 -std=c++17 -c wallet_example.cc -o wallet_example.o
// g++ -g wallet_example.o operation.o wallet.o -o wallet_example
