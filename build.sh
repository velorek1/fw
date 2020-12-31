#!/bin/bash
#file: build.sh
echo "Attempting to build fileviewer..."
gcc -c rterm.c
gcc -c scbuf.c
gcc -c tm.c
gcc -c keyb.c
gcc -c fileb.c
gcc -c opfile.c
gcc -c uintf.c
gcc -c listc.c
gcc -c about.h
gcc -c main.c
gcc rterm.o scbuf.o tm.o keyb.o fileb.o main.o opfile.o uintf.o listc.o -o fw
echo "Run as ./fw"

