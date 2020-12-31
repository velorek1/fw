/*
========================================================================
- HEADER - 
Module to handle basic file operations
@author : Velorek
@version : 1.0  
Last modified: 14/04/2019 Rename headers                                                              
========================================================================
*/

#ifndef _FILEB_H_
#define _FILEB_H_

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <stdio.h>
#include "scbuf.h"

/*====================================================================*/
/* FUNCTION PROTOTYPES                                                */
/*====================================================================*/

int     openFile(FILE ** filePtr, char fileName[], char *mode);
int     closeFile(FILE * filePtr);
long    getfileSize(FILE * filePtr);
long    countLinesFile(FILE * filePtr);
long    checkFile(FILE * filePtr);
int     file_exists(char *fileName);
long    gotoLine(FILE * filePtr, long line);
#endif
