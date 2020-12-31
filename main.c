/*
======================================================================
PROGRAM FW File Viewer - A file viewing tool.
@author : Velorek
@version : 0.1
Last modified : 28/12/2020 - FIRST COMMIT 
======================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "rterm.h"
#include "scbuf.h"
#include "tm.h"
#include "keyb.h"
#include "fileb.h"
#include "opfile.h"
#include "listc.h"
#include "about.h"
#define MAX_TEXT1 150
#define MAX_TEXT2 255
#define TITLE_LENGTH 26

//FUNCTION PROTOTYPES
void main_screen();
void cleanArea(int raw);
void tick2();
void tick1(int *num, int *i);
int refresh_screen();
int handleopenFile(FILE ** filePtr, char *fileName);
void credits();
void filetoDisplay(FILE *filePtr);
void scroll(FILE *filePtr);
void check_arguments(int argc, char *argv[]);
long checkScrollValues();

//GLOBAL VARIABLES
int scW, scH, old_scW, old_scH; //screen dimensions
char currentFile[MAX_TEXT1];
char fileName[MAX_TEXT2];
char msg[TITLE_LENGTH] = "=-[fw]:FILE VIEWER v0.1-=\0";
SCROLLDATA openFileData; //openFile dialog
LISTCHOICE *mylist, data; //menus handler
FILE   *filePtr;
int update = 0; //Bool global variable to control when screen is written to buffer
int time_since_keypressed = 0;
//FILE SCROLL POINTERS
long linesinFile =0;
int  displayLength = 0;
long scrollLimit = 0;
long currentLine = 0;
int scrollActive = 0;

//MAIN
int main(int argc, char *argv[]){

NTIMER mytimer1,mytimer2,mytimer3;
char ch=0;
int i=0;
int num=0;
int num2=0;
int keypressed=0, oldkeypressed=0;
  /*--------------------------INIT VALUES------------------------------*/
  /* INIT TIMER */
  //mytimer1.ticks must be set to 0 at start.
  mytimer1.ms = 30;  // Timer 1 - Title animation
  mytimer1.ticks = 0;
  mytimer2.ms = 100;
  mytimer2.ticks = 0; // Timer 2 - Time animation
  mytimer3.ms = 100;
  mytimer3.ticks = 0; // Timer 3 - Screen display control
  pushTerm();
  create_screen();
  get_terminal_dimensions(&scH, &scW);
  displayLength = scH - 5;
  old_scH = scH;
  old_scW = scW;
  main_screen();
  save_buffer();
  hidecursor();
  /*-------------------------------------------------------------------*/
  /*------------------------CHECK ARGUMENTS----------------------------*/
  check_arguments(argc, argv);
  /*------------------------MAIN PROGRAM LOOP---------------------------*/
  do{
	//TIMER 1
      if (timerC(&mytimer1) == 1) { 
 	 //Title animation
	 tick1(&num, &i);
       }
	//TIMER 2
      if (timerC(&mytimer2) == 1) { 
	 //Time animation
         tick2();
       } 
	//TIMER 3
       if (timerC(&mytimer3) == 1) {
        //Update screen control and screen refresh
         //if(time_since_keypressed > 5) time_since_keypressed = 0;
	 if (keypressed == 0) time_since_keypressed++;
	else
	  time_since_keypressed = 0;
         //check screen dimensions & update if changed
         refresh_screen(0);
       } 
   //CHECK KEYS
   oldkeypressed = ch;
   keypressed = kbhit();
   if (keypressed == 1){
     keypressed = 0;
     time_since_keypressed = 0;
     ch = readch();
     if (ch == 'a') 
	{
	if (scrollActive == 1){
	   update =1;
	   if (currentLine<scrollLimit) currentLine++; 
	   scroll(filePtr);
	}
	};
     if (ch == 'q') {if(currentLine >0) {update=1;currentLine--; scroll(filePtr);}};
     if (ch == 'x') {update=1;}
   } else{
     if (filePtr != NULL && update == 1 && time_since_keypressed>1) {
	//Screen buffer is updated here!
	filetoDisplay(filePtr); 
	update = 0;
	time_since_keypressed = 0;
     }
     ch = 0;
   }
  } while (ch != K_ESCAPE);
  /*--------------------------------------------------------------------*/
  credits();
  return 0;
}

void main_screen(){
int i=0;
   screen_color(BH_BLUE);
  for (i=1;i<scW; i++){
       write_ch(i,1,' ',B_BLUE,F_BLUE);
       write_ch(i,2,' ',B_WHITE,F_WHITE);
       write_ch(i,3,' ',B_BLACK,F_BLACK);
       write_ch(i,scH-1,' ',B_BLACK,F_BLACK);
       write_ch(i,scH,' ',B_WHITE,F_WHITE);
   }
  write_ch(scW,1,' ',B_BLUE,F_BLUE);
  write_ch(scW,2,' ',B_WHITE,F_WHITE);
  write_ch(scW,3,' ',B_BLACK,F_BLACK);
  write_ch(scW,scH-1,' ',B_BLACK,F_BLACK);
  write_str(1,2,"File  Help",B_WHITE,F_BLACK);
  write_ch(12,2,NVER_LINE, B_WHITE,F_BLACK);
  write_ch(1,2,'F',B_WHITE,F_BLUE);
  write_ch(7,2,'H',B_WHITE,F_BLUE);
  write_str(1,scH,"F2: MENUS   ALT-X: EXIT" , B_WHITE,F_BLACK);
  write_ch(11,scH,NVER_LINE , B_WHITE,F_BLACK);
  write_str((scW/2) - 10, 1,msg,B_BLUE,F_WHITE);
   if (strcmp(currentFile,"\0") == 0) {
  	write_str(1,3,"No file open!",B_BLACK,F_WHITE);
    } else
    {
	write_str(14,2,fileName,B_WHITE,F_BLACK);
	write_str(1,3,currentFile,B_BLACK,F_WHITE);
   }
  for (i=0; i<ABOUT_LINES; i++)
	write_str((scW/2) - (80/2), ((scH/2) - (ABOUT_LINES/2)) + i, about_msg[i], BH_BLUE, F_WHITE);
  update_screen();
}

void tick1(int *num, int *i){
//Title animation
  if (*num==0) {
    outputcolor(B_BLUE,F_YELLOW);
    gotoxy(((scW/2)-TITLE_LENGTH/2)+*i+3,1);
    printf("%c\n",msg[*i]);
    *num = 1;
    *i = *i + 1;
  } else {
    outputcolor(B_BLUE,F_WHITE);
    gotoxy(((scW/2)-TITLE_LENGTH/2)+*i+2,1);
    printf("%c\n",msg[*i-1]); 
    *num = 0;
  }
  if (*i>TITLE_LENGTH) {
    *i = 0; 
    *num=0; 
    outputcolor(B_BLUE,F_BLUE);
    gotoxy(((scW/2)-10),1);
    printf("%s\n",msg); 
   }
}
void tick2(){
//Time & screen refresh
  time_t  mytime = time(NULL);
  char   *time_str = ctime(&mytime);
  time_str[strlen(time_str) - 1] = '\0';
  //display system time
  outputcolor(B_BLUE,F_WHITE);
  gotoxy(scW - strlen(time_str), 1);
  printf("%s\n",time_str); 
}

int refresh_screen() {
/* Query terminal dimensions again and check if resize 
   has been produced */
  int j, k;
   get_terminal_dimensions(&scH, &scW);
   displayLength = scH - 5;
   if (filePtr != NULL && scrollActive == 1) scrollLimit = checkScrollValues();      //Update scroll values
  if(scH != old_scH || scW != old_scW){
        free_buffer();		//delete structure from memory for resize
        create_screen();		//create new structure 
        main_screen();		//Refresh screen in case of resize
        if (linesinFile > displayLength) scrollActive = 1;
        else scrollActive = 0;
	//if (filePtr != NULL && scrollActive == 1) scrollLimit = checkScrollValues();      //Update scroll values
        update=1;
        old_scH = scH;
        old_scW = scW;
	return 1;
  } 
  return 0;
}

int handleopenFile(FILE ** filePtr, char *fileName) {
  long    checkF = 0;
  int     ok = 0;

  openFile(filePtr, fileName, "r");
  //Check for binary characters to determine filetype
  checkF = checkFile(*filePtr);
  linesinFile = countLinesFile(*filePtr);
  
  if (linesinFile > displayLength) scrollActive = 1;
    else scrollActive = 0;
  if(checkF > 5) {
    //File doesn't seem to be a text file. Open anyway?  
    ok = yesnoWindow(mylist, "File is not a text file! \nOpen anyway?", "Info");
    if(ok == 1) {
 	update=1;
    } else
    {
     credits();
    } 
  } else {
    update=1;
    return ok;
   }
}

void filetoDisplay(FILE *filePtr){
  long     inlineChar = 0, lineCounter = 0, i=1, whereinfile=0;
  int j,k;
  char    ch;
  time_t  mytime = time(NULL);
  char   *time_str = ctime(&mytime);
//Update screen buffer 
    cleanArea(0);
    rewind(filePtr);		//Make sure we are at the beginning
    whereinfile=gotoLine(filePtr,currentLine);
    if (whereinfile>1) fseek(filePtr, whereinfile, 0);
    while (!feof(filePtr)) {
	  ch = getc(filePtr);
          if (ch != END_LINE_CHAR && ch != '\0') {
		if (ch==9 || ch==13){
			 write_ch(i,lineCounter+4,'>',BH_GREEN,F_WHITE);
			 i++;
		}
 		else{
		 write_ch(i,lineCounter+4,ch,BH_BLUE,F_WHITE);
		  i++;
		}
	  }
	  if(ch == END_LINE_CHAR) { 
	    for (k=i; k<scW; k++){
		write_ch(k,lineCounter+4,' ',BH_BLUE,F_BLUE);
		}
	    lineCounter++;
	    i=1;
	  }
	  if (lineCounter > scH-6) break;
    }
  //display metrics
  write_str(1,scH-1,"- Lines:   | - Progress:    %", B_BLACK, F_WHITE); 
  write_num(10,scH-1,linesinFile,10, B_BLACK, F_YELLOW); 
  write_num(26,scH-1,lineCounter,10, B_BLACK, F_YELLOW); 
  //display system time
  time_str[strlen(time_str) - 1] = '\0';
  write_str(scW - strlen(time_str),1,time_str,B_BLUE,F_WHITE);
  //clean viewing area
  //cleanArea();
  //write to screen buffer
  update_screen();
}

void scroll(FILE *filePtr){
  long     inlineChar = 0, lineCounter = 0, i=1, whereinfile=0;
  char    ch;
  int j,k;
    //RAW output for smoother scroll
    rewind(filePtr);		//Make sure we are at the beginning
    whereinfile=gotoLine(filePtr,currentLine);
    if (whereinfile >1) fseek(filePtr, whereinfile, 0);
      while (!feof(filePtr)) {
	  ch = getc(filePtr);
	  outputcolor(F_WHITE,BH_BLUE);
          gotoxy(i,lineCounter+4);
          if (ch != END_LINE_CHAR && ch != '\0') {
		if (ch==9 || ch==13){
	  		outputcolor(F_WHITE,BH_GREEN);
			printf(">");
			i++;
		  } else{
		    printf("%c",ch);
          	    i++;
		} 
	  }
	  if(ch == END_LINE_CHAR) { 
	    for (k=i; k<=scW; k++){	
    		outputcolor(F_BLUE,BH_BLUE);
		gotoxy(k,lineCounter+4);
		printf("%c",32);
		}
	    lineCounter++;
	    i=1;
	  }
	  if (lineCounter > scH-6) break;
    }
}

void cleanArea(int raw){
int i,j;
   if (raw == 1) {
   for(j=4; j<scH-1;j++)
        for (i=1; i<=scW; i++){
    	  outputcolor(F_BLUE,BH_BLUE);
	  gotoxy(i,j);
  	  printf("%c",32);
        }
   }
   else{
   for(j=4; j<scH-1;j++)
        for (i=1; i<=scW; i++){
    	  write_ch(i,j,' ',F_BLUE,BH_BLUE);
        }
}
}
void check_arguments(int argc, char *argv[]){
//check arguments or display open file dialog
   if(argc > 1) {
    //Does the file exist? Open or create?
    if(file_exists(argv[1]) == 1) {
      //open file in arguments
      clearString(currentFile, MAX_TEXT);
      strcpy(fileName, argv[1]);
      getcwd(currentFile, sizeof(currentFile));	//Get path
      strcat(currentFile, argv[1]);
      handleopenFile(&filePtr, fileName); 
    } else {	      
      //display open file dialog if file does not exist
      strcpy(currentFile, "\0");
      openFileDialog(&openFileData);
      if (strcmp(openFileData.path, "\0") != 0 && file_exists(openFileData.path)){
        strcpy(currentFile, openFileData.fullPath);
        strcpy(fileName, openFileData.path);
        handleopenFile(&filePtr, fileName); 
      } else
      {
	//no file selected or file does not exist
        strcpy(currentFile, "No file open!");
        strcpy(fileName, "No file open!");
      }
     }
    } else{
	//display open file dialog if no arguments are given
      strcpy(currentFile, "\0");
      openFileDialog(&openFileData);
      if (strcmp(openFileData.path, "\0") != 0 && file_exists(openFileData.path)){
        strcpy(currentFile, openFileData.fullPath);
        strcpy(fileName, openFileData.path);
        handleopenFile(&filePtr, fileName); 
      } else
      {
	//no file selected or file does not exist
        strcpy(currentFile, "No file open!");
        strcpy(fileName, "No file open!");
      }
    }
   if (strcmp(currentFile,"\0") == 0) {
  	write_str(1,3,"No file open!",B_BLACK,F_WHITE);
    } else
    {
	write_str(14,2,fileName,B_WHITE,F_BLACK);
	write_str(1,3,currentFile,B_BLACK,F_WHITE);
   }
  update_screen();
}

long checkScrollValues(){
	return (linesinFile - displayLength) - 1;
}

void credits(){
NTIMER mytimer1;
int i=0, j=0;
char cmsg[31] = "\nFile vieWer. Coded by v3l0r3k\n";
 if(filePtr != NULL) {
    closeFile(filePtr);
  }
  mytimer1.ms = 10;  // Timer 1 - Credits animation
  mytimer1.ticks = 0;
  cleanArea(1);
  for (i=0; i<ABOUT_LINES; i++){
	outputcolor(F_WHITE, BH_BLUE);
	gotoxy((scW/2) - (80/2), ((scH/2) - (ABOUT_LINES/2)) + i);
        printf("%s",about_msg[i]);
  }
 resetTerm();
 outputcolor(B_BLACK,F_BLACK);
 gotoxy(1,scH-2);
 for (i=0; i<scW;i++)
	printf(" ");
 gotoxy(1,scH-1);
 for (i=0; i<scW;i++)
	printf(" ");
 gotoxy(1,scH);
 for (i=0; i<scW;i++)
	printf(" ");

 i=0;
 j=0;
 //printf("\n");
 do{
  if (timerC(&mytimer1) == 1) { 
     if (mytimer1.ticks<23){
      outputcolor(F_WHITE,B_BLACK);
      gotoxy(i,scH-2); 
      if (i<30) printf("%c\n",cmsg[i]);      
      i++;
     } else {
      gotoxy(j,scH-2); 
      //outputcolor(B_BLACK,F_WHITE);
      outputcolor(FH_BLUE,B_BLACK);
      if (j<30) {
	if (j== 1 || j== 9)  printf("%c\n",cmsg[j]);   
        if (j> 22) {outputcolor(FH_BLACK,B_BLACK); printf("%c\n",cmsg[j]);}      
      }
      j++;
      }  
 }
 } while (mytimer1.ticks<53);
  printf("\n");
  showcursor();
  resetAnsi(0);
  showcursor();
}


