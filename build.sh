#!/bin/sh
cd src
g++ -o ../out toaster.cpp -lstdc++ -Wl,--no-as-needed -ldl -std=c++17 -g