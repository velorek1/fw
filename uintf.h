/*
========================================================================
- HEADER - 
Module to show user interface windows, textbox, etc.
@author : Velorek
@version : 1.0  
Last modified: 30/12/2020 Adapted for FileViewer
                                                                
========================================================================
*/

#ifndef _UINTF_H_
#define _UINTF_H_

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <stdio.h>
#include "scbuf.h"
#include "listc.h"
#include "keyb.h"

//HELP MESSAGE
#define HELP1 "List of most common keys: \n"
#define HELP2 "- F1: DISPLAY HELP\n"
#define HELP3 "- CTRL-L/F2: OPEN MENUS \n"
#define HELP4 "- ALT-O: OPEN FILE DIALOG \n"
#define HELP5 "- ALT-X/CTRL-C: EXIT\n"
#define HELP6 "- ALT-F: FILE MENU \n"
#define HELP7 "- ALT-H: HELP MENU\n"
#define HELP8 "- ALT-S: SET FILE\n"
#define HELP9 "- ALT-A: ABOUT DIALOG\n"
#define HELP10 "- ALT-I: INFO\n"
#define HELP11 "\nFor more info visit  \n [http://oldstuff286.blogspot.com]"

//END

#define MAX_TEXT 150

//MENU CONSTANTS
#define HOR_MENU -1
#define FILE_MENU 0
#define HELP_MENU 1
#define YESNO_MENU 3
#define OK_MENU 4
#define MAX_FILENAME 100

//DROP-DOWN MENUS
#define OPTION_1 0
#define OPTION_2 1
#define OPTION_3 2
#define OPTION_4 3
#define OPTION_5 4
#define OPTION_NIL -1		//Reset option
#define CONFIRMATION 1
#define K_LEFTMENU -1		//Left arrow key pressed while in menu
#define K_RIGHTMENU -2		//Right arrow key pressed while in menu

//EDIT CONSTANTS
#define CHAR_NIL '\0'
#define END_LINE_CHAR 0x0A	// $0A
#define FILL_CHAR 32

//GLOBAL VARIABLES - COLOR SCHEME
extern int EDITAREACOL;
extern int EDIT_FORECOLOR;
extern int STATUSBAR;
extern int STATUSMSG;
extern int MENU_PANEL;
extern int MENU2_PANEL;
extern int MENU_SELECTOR;
extern int MENU_FOREGROUND0;
extern int MENU2_FOREGROUND0;
extern int MENU_FOREGROUND1;
extern int EDITWINDOW_BACK;
extern int EDITWINDOW_FORE;
extern int SCROLLBAR_BACK;
extern int SCROLLBAR_FORE;
extern int SCROLLBAR_SEL;
extern int SCROLLBAR_ARR;
extern int WINDOW_TITLEB; // to be accessible from opfile.c
extern int WINDOW_TITLEF;

/*====================================================================*/
/* FUNCTION PROTOTYPES                                                */
/*====================================================================*/

/*----------------------------*/
/* User Interface - Text Box. */
/*----------------------------*/

void    loadmenus(LISTCHOICE * mylist, int choice);

int     textbox(int wherex, int wherey, int displayLength,
		char label[MAX_TEXT], char text[MAX_TEXT], int backcolor,
		int labelcolor, int textcolor);
int     alertWindow(LISTCHOICE * mylist, char *message, char *windowTitle);
int     infoWindow(LISTCHOICE * mylist, char *message, char *windowTitle);
int     inputWindow(char *label, char *tempFile, char *windowTitle);
int     yesnoWindow(LISTCHOICE * mylist, char *message, char *windowTitle);
int     helpWindow(LISTCHOICE * mylist, char *message, char *windowTitle);

#endif
