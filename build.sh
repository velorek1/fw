#!/bin/bash
#file: build.sh
echo -e "· Compiling \e[94mfw\e[0m - \e[94mF\e[0milevie\e[94mW\e[0mer Coded By \e[90mv3l0r3k\e[0m"

gcc -c -Wall -fsigned-char rterm.c
gcc -c -Wall -fsigned-char  scbuf.c
gcc -c -Wall -fsigned-char tm.c
gcc -c -Wall -fsigned-char keyb.c
gcc -c -Wall -fsigned-char fileb.c
gcc -c -Wall -fsigned-char opfile.c
gcc -c -Wall -fsigned-char uintf.c
gcc -c -Wall -fsigned-char listc.c
gcc -c -Wall -fsigned-char about.h
gcc -c -Wall -fsigned-char main.c
gcc rterm.o scbuf.o tm.o keyb.o fileb.o main.o opfile.o uintf.o listc.o -o fw
echo -e "· Run as \e[94m./fw\e[0m" 
