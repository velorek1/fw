#!/bin/bash
#file: build.sh
echo "Attempting to build fileviewer..."
gcc -c -Wall rterm.c
gcc -c -Wall scbuf.c
gcc -c -Wall tm.c
gcc -c -Wall keyb.c
gcc -c -Wall fileb.c
gcc -c -Wall opfile.c
gcc -c -Wall uintf.c
gcc -c -Wall listc.c
gcc -c -Wall about.h
gcc -c -Wall main.c
gcc rterm.o scbuf.o tm.o keyb.o fileb.o main.o opfile.o uintf.o listc.o -o fw
echo "Run as ./fw"

