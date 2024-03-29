/*
======================================================================
PROGRAM FW File Viewer - A retro TUI file viewing tool 
@author : Velorek
@version : 0.1
Last modified : 08/09/2021 - Screen resize limits
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
#define LIMITHEIGHT 15
#define LIMITWIDTH 50
#define TITLE_LENGTH 26
#define HORIZONTAL_SHIFT 500 //maximum horizontal scroll

//FUNCTION PROTOTYPES
void main_screen();
void cleanArea(int raw);
void tick2();
void tick1(int *num, int *i);
int refresh_screen();
int special_keys(char ch);
int handleopenFile(FILE ** filePtr, char *fwfileName);
void credits();
void filetoDisplay(FILE *filePtr, int scupdate);
void scroll(FILE *filePtr);
char* check_arguments(int argc, char *argv[]);
long checkScrollValues();
void    drop_down(char *kglobal);
char horizontal_menu();
int fileInfoDialog();
void about_info();
char *setfile();
int help_info();
void update_indicators();

/* -----------------------------------*/
//GLOBAL VARIABLES
int scW, scH, old_scW, old_scH; //screen dimensions
char currentPath[MAX_TEXT1];
char fwfileName[MAX_TEXT2];
char msg[TITLE_LENGTH] = "=-[fw]:FILE VIEWER v0.1-=\0";
char    kglobal = 0;		// Global variable for menu animation
LISTCHOICE *mylist, data; //menus handler
SCROLLDATA openFileData; //openFile dialog
LISTCHOICE *mylist, data; //menus handler
FILE   *filePtr=NULL;

//BOOL-like variables
int update = 0; //Bool global variable to control when screen is written to buffer
int displayLogo = 0; //Display logo control
int time_since_keypressed = 0;
int status = 0;

//FILE SCROLL POINTERS
long linesinFile =0;
int  hdisplayLength=0; //horizontal scroll
int  currentColumn = 0; //horizontal scroll
int  displayLength = 0; //vertical scroll
long scrollLimit = 0; //vertical scroll
long currentLine = 0; //verticall scroll
int scrollActive = 0; //vertical scroll
/* -----------------------------------*/


//MAIN
int main(int argc, char *argv[]){

NTIMER mytimer1,mytimer2,mytimer3;
char ch=0;
int i=0;
int num=0;
int keypressed=0;

  /*--------------------------INIT VALUES------------------------------*/
  /* INIT TIMER */
  //mytimer1.ticks must be set to 0 at start.
  mytimer1.ms = 30;  // Timer 1 - Title animation
  mytimer1.ticks = 0;
  mytimer2.ms = 100;
  mytimer2.ticks = 0; // Timer 2 - Time animation
  mytimer3.ms = 100;
  mytimer3.ticks = 0; // Timer 3 - Screen display control
  pushTerm();  //record previous terminal settings
  create_screen(); 
  get_terminal_dimensions(&scH, &scW);
  if (scW >79 && scH > 23) displayLogo = 1;
  else displayLogo = 0; 
  displayLength = scH - 5;
  old_scH = scH;
  old_scW = scW;
  resetch();
  main_screen();
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
   keypressed = kbhit();
   if (keypressed == 1){
     keypressed = 0;
     time_since_keypressed = 0;
     ch = readch();
     //ESC-KEY related keys
     if (special_keys(ch) == -1) status = -1;
    //FAIL-SAFE ARROW KEYS
   if (filePtr != NULL){
      if (ch =='a') {
        //Left-arrow key
         if(currentColumn > 0) {currentColumn--; cleanArea(1); scroll(filePtr);}  
      } 
      if (ch =='d') {
        //Right-arrow key
         if(currentColumn < HORIZONTAL_SHIFT) {currentColumn++; cleanArea(1); scroll(filePtr);}  
      }
      if (ch =='w') {
        //Up-arrow key
         if(currentLine >0) {currentLine--; if (currentColumn > 1) cleanArea(1);scroll(filePtr);}
      } 
      if (ch == 's') {
        //Down-arrow key 
 	  if (scrollActive == 1){
	     if (currentLine<scrollLimit) currentLine++; 
	     if (currentColumn > 1) cleanArea(1);
	     scroll(filePtr);
   	  }
       }
    }
    if (ch == K_CTRL_C) status = -1;
    if (ch == K_CTRL_L) {
	filetoDisplay(filePtr,  1);
       if(horizontal_menu() == K_ESCAPE) {
 	//Exit horizontal menu with ESC 3x
	kglobal = K_ESCAPE;
	main_screen();
       }
       drop_down(&kglobal);
     }
     } else{
     
     if (filePtr != NULL && update == 1 && time_since_keypressed>1) {
	//Screen buffer is updated here! Screenshot of what is shown on screen
	filetoDisplay(filePtr, 1); 
	update = 0;
	time_since_keypressed = 0;
     } 
     ch = 0;
     keypressed = 0;
   }
  } while (status != -1);
  /*--------------------------------------------------------------------*/
  credits();
  return 0;
}


/* --------------------------------------*/
//DISPLAY
/* --------------------------------------*/

void main_screen(){
int i=0;
   screen_color(B_BLUE);
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
  write_str(1,scH,"F2: MENUS   ALT-O: OPEN   ALT-X/CTRL-C: EXIT" , B_WHITE,F_BLACK);
  write_ch(11,scH,NVER_LINE , B_WHITE,F_BLACK);
  write_ch(25,scH,NVER_LINE , B_WHITE,F_BLACK);
  write_str((scW/2) - 10, 1,msg,B_BLUE,F_WHITE);
  update_indicators();
  if (displayLogo == 1){
  for (i=0; i<ABOUT_LINES; i++)
	write_str((scW/2) - (80/2), ((scH/2) - (ABOUT_LINES/2)) + i, about_msg[i], B_BLUE, F_WHITE);
  }
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
//Time 
  time_t  mytime = time(NULL);
  char   *time_str = ctime(&mytime);
  time_str[strlen(time_str) - 1] = '\0';
  //display system time
  outputcolor(B_WHITE,F_BLACK);
  gotoxy(scW - strlen(time_str), 2);
  printf("%s\n",time_str); 
}

int refresh_screen() {
/* Query terminal dimensions again and check if resize 
   has been produced */
   get_terminal_dimensions(&scH, &scW);
   if (scW < LIMITWIDTH || scH < LIMITHEIGHT) { return 0;} //resize limit
    if (scW >79 && scH > 23) displayLogo = 1;
     else displayLogo = 0; 
     displayLength = scH - 5;
     hdisplayLength = scW;
     if (filePtr != NULL && scrollActive == 1) scrollLimit = checkScrollValues();      //Update scroll values
    if(scH != old_scH || scW != old_scW){
        free_buffer();		//delete structure from memory for resize
        create_screen();		//create new structure 
        main_screen();		//Refresh screen in case of resize
        if (linesinFile > displayLength) scrollActive = 1;
        else scrollActive = 0;
        update=1;
	update_indicators();
        old_scH = scH;
        old_scW = scW;
	return 1;
    } 
  return 0;
}

void update_indicators(){
int i; 
 for (i=1;i<scW; i++){
       write_ch(i,2,' ',B_WHITE,F_WHITE);
       write_ch(i,3,' ',B_BLACK,F_BLACK);
   }
  write_ch(scW,2,' ',B_WHITE,F_WHITE);
  write_ch(scW,3,' ',B_BLACK,F_BLACK);

  write_ch(12,2,NVER_LINE, B_WHITE,F_BLACK);
  if (strcmp(currentPath,"\0") == 0) {
  	write_str(1,3,"No file open!",B_BLACK,F_WHITE);
    } else
    {
	write_str(14,2,fwfileName,B_WHITE,F_BLACK);
	write_str(1,3,currentPath,B_BLACK,F_WHITE);
   }
  write_str(1, 2, "File  Help", B_WHITE, F_BLACK);
  write_str(1, 2, "F", B_WHITE, F_BLUE);
  write_str(7, 2, "H", B_WHITE, F_BLUE);
  update_screen();
}

/*-----------------------------------------*/
/* Manage keys that send a ESC sequence    */
/*-----------------------------------------*/

int special_keys(char ch) {
/* MANAGE SPECIAL KEYS */
/* 
   New implementation: Trail of chars found in keyb.c
   If K_ESCAPE is captured read a trail up to 5 characters from the console.
   This is to control the fact that some keys may change
   according to the terminal and expand the editor's possibilities.
   Eg: F2 can be either 27 79 81 or 27 91 91 82.  
   - Note : if (currentColumn > 1) cleanArea(1);	
	When horizontal scroll is active all the screen is cleaned when moving.
*/

  char    chartrail[5];
  if(ch == K_ESCAPE) {
    read_keytrail(chartrail);	//Read trail after ESC key

    //Check key trails for special keys.

    //FUNCTION KEYS : F1 - F4
    if(strcmp(chartrail, K_F2_TRAIL) == 0 ||
       strcmp(chartrail, K_F2_TRAIL2) == 0) {
      //update screen
      filetoDisplay(filePtr, 0);
      if(horizontal_menu() == K_ESCAPE) {
	//Exit horizontal menu with ESC 3x
	kglobal = K_ESCAPE;
	main_screen();
      }
     drop_down(&kglobal);
    } else if(strcmp(chartrail, K_F1_TRAIL) == 0 ||
	      strcmp(chartrail, K_F1_TRAIL2) == 0) {
      filetoDisplay(filePtr,  1);
      help_info(); 
      // ARROW KEYS
    } else if((strcmp(chartrail, K_LEFT_TRAIL) == 0) && filePtr != NULL ) {
      //Left-arrow key
       if(currentColumn > 0) {currentColumn--; cleanArea(1); scroll(filePtr);}  
    } else if((strcmp(chartrail, K_RIGHT_TRAIL) == 0)  && filePtr != NULL ) {
      //Right-arrow key
       if(currentColumn < HORIZONTAL_SHIFT) {currentColumn++; cleanArea(1); scroll(filePtr);}  
    } else if((strcmp(chartrail, K_UP_TRAIL) == 0)  && filePtr != NULL ) {
      //Up-arrow key
       if(currentLine >0) {currentLine--; if (currentColumn > 1) cleanArea(1);scroll(filePtr);}
    } else if((strcmp(chartrail, K_DOWN_TRAIL) == 0)  && filePtr != NULL ) {
      //Down-arrow key 
 	if (scrollActive == 1){
	   if (currentLine<scrollLimit) currentLine++; 
	   if (currentColumn > 1) cleanArea(1);
	   scroll(filePtr);
	}
     } else if((strcmp(chartrail, K_PAGEDOWN_TRAIL) == 0)  && filePtr != NULL )  {
      if (currentLine + displayLength < scrollLimit) currentLine = currentLine + displayLength;
      else currentLine = scrollLimit;
	if (currentColumn > 1) cleanArea(1);
	scroll(filePtr);
     } else if((strcmp(chartrail, K_PAGEUP_TRAIL) == 0)  && filePtr != NULL ) {
       if (currentLine - displayLength > 1) currentLine = currentLine - displayLength;
 	else currentLine = 0;
	if (currentColumn > 1) cleanArea(1);
	scroll(filePtr);
     } else if((strcmp(chartrail, K_HOME_TRAIL) == 0 ||
	      strcmp(chartrail, K_HOME_TRAIL2) == 0)  && filePtr != NULL ) {
	currentLine = 0;
	if (currentColumn > 1) cleanArea(1);
	scroll(filePtr);
     } else if((strcmp(chartrail, K_END_TRAIL) == 0 ||
	      strcmp(chartrail, K_END_TRAIL2) == 0)  && filePtr != NULL ) {
	currentLine = scrollLimit;
	if (currentColumn > 1) cleanArea(1);
	scroll(filePtr);
     } else if(strcmp(chartrail, K_ALT_F) == 0) {
      filetoDisplay(filePtr,  1);
      data.index=FILE_MENU;
      drop_down(&kglobal);	//animation   
    } else if(strcmp(chartrail, K_ALT_H) == 0) {
      filetoDisplay(filePtr,  1);
      data.index=HELP_MENU;
      drop_down(&kglobal);	//animation  
    } else if(strcmp(chartrail, K_ALT_I) == 0) {
      filetoDisplay(filePtr, 0);
      fileInfoDialog();
    } else if(strcmp(chartrail, K_ALT_A) == 0) {
      filetoDisplay(filePtr, 0);
      about_info();
    } else if(strcmp(chartrail, K_ALT_S) == 0) {
      filetoDisplay(filePtr, 0);
      setfile();
    } else if(strcmp(chartrail, K_ALT_X) == 0) {
      return -1;
    } else if(strcmp(chartrail, K_ALT_O) == 0) {
      filetoDisplay(filePtr, 1);
      openFileDialog(&openFileData);
      if (strcmp(openFileData.path, "\0") != 0 && file_exists(openFileData.path)){
        strcpy(currentPath, "\0");
        cleanString(currentPath, MAX_TEXT1);
        cleanString(fwfileName, MAX_TEXT2);
        strcpy(currentPath, openFileData.fullPath);
        strcpy(fwfileName, openFileData.path);
        handleopenFile(&filePtr, fwfileName);
	update_indicators(); 
      }
    }
  }
 return 0;
}

/* --------------------------------------*/
//FILE OPERATIONS
/* --------------------------------------*/
int handleopenFile(FILE ** filePtr, char *fwfileName) {
  //long    checkF = 0;
  //int     ok = 0;
  currentLine = 0;
  currentColumn=0;
  scrollLimit = 0;
  openFile(filePtr, fwfileName, "r");
  //Check for binary characters to determine filetype
  //checkF = checkFile(*filePtr);
  linesinFile = countLinesFile(*filePtr);
  
  if (linesinFile > displayLength) scrollActive = 1;
    else scrollActive = 0;

    update=1;

return 0;
}

//this routine copies file content to screen buffer so that windows and dialogs
//can be displayed and the content they cover can be later retrieved
void filetoDisplay(FILE *filePtr, int scupdate){
  long     lineCounter = 0, i=1, whereinfile=0;
  double progress=0;
  int k=0;
  int wherex = 0;
  char    ch;
  time_t  mytime = time(NULL);
  char   *time_str = ctime(&mytime);
//Update screen buffer 

if (filePtr != NULL) {
    cleanArea(0);
    rewind(filePtr);		//Make sure we are at the beginning
    whereinfile=gotoLine(filePtr,currentLine);
    if (whereinfile>1) fseek(filePtr, whereinfile, 0);
    while (!feof(filePtr)) {
	  ch = getc(filePtr);	
	  wherex = labs(i-currentColumn);
          if (ch != END_LINE_CHAR && ch != '\0') {
		if (ch==9){ //for convenience TAB char is shown in green
			 //with horizontal scroll
			 if (i> currentColumn) write_ch(wherex,lineCounter+4,'>',B_GREEN,F_WHITE);
			 i++;
		} else if (ch==13){
		//windows 0x0d is transform to 0x20 
		  ch=32;
		}
 		else{
		  if (i> currentColumn) write_ch(wherex,lineCounter+4,ch,B_BLUE,F_WHITE);
		  i++;
		}
	  }
	  if(ch == END_LINE_CHAR) { 
		//next line
	    for (k=i; k<scW; k++){
		write_ch(k,lineCounter+4,' ',B_BLUE,F_BLUE);
		}
	    lineCounter++;
	    i=1;
	  }
	//break when it reaches end of vertical displaying area
	  if (lineCounter > scH-6) break;
    }
    //to delete the last 0x0A character on screen
    write_ch(i-1,lineCounter+4,' ',B_BLUE,F_BLUE);
  //display metrics
  write_str(1,scH-1,"- Lines:         | - Progress:    %  | - H:    /500", B_BLACK, F_WHITE); 
  progress = ((double) currentLine / (double) scrollLimit) * 100;
  write_num(10,scH-1,linesinFile,10, B_BLACK, F_YELLOW); 
  if (scrollActive ==1) write_num(32,scH-1,(int)progress,3, B_BLACK, F_YELLOW); 
  else  write_num(32,scH-1,100,3, B_BLACK, F_YELLOW);  
  write_num(45,scH-1,currentColumn,3, B_BLACK, F_YELLOW); 
  //display system time
  time_str[strlen(time_str) - 1] = '\0';
  write_str(scW - strlen(time_str),2,time_str,B_WHITE,F_BLACK);
  //clean viewing area
  //write to screen buffer
  if (scupdate==1) {update_screen();}
}
}

//this routine does a scroll through file and output directly to screen
void scroll(FILE *filePtr){
  long    lineCounter = 0, i=1, whereinfile=0;
  double    progress;
  char    ch;
  int k;
  int wherex = 0;
if (filePtr != NULL) {
    //RAW output for smoother scroll
    rewind(filePtr);		//Make sure we are at the beginning
    whereinfile=gotoLine(filePtr,currentLine);
    if (whereinfile >1) fseek(filePtr, whereinfile, 0);

      while (!feof(filePtr)) {
	  ch = getc(filePtr);
	  outputcolor(F_WHITE,B_BLUE);
	  wherex = labs(i-currentColumn); 
          if (wherex < scW-1) gotoxy(labs(i-currentColumn),lineCounter+4);
          if (ch != END_LINE_CHAR && ch != '\0') {
		if (ch==9){
			//for convenience TAB char is shown in green
	  		outputcolor(F_WHITE,B_GREEN);
			if (i> currentColumn) printf(">");
			i++;
		} else if (ch==13){
		  //windows 0x0d0a transformed to 0x20 
		   ch = 32;
		} else{
		    //currenColumn is for horizontal scroll
			if (i> currentColumn) {
				printf("%c",ch);}
		i++;
		} 
	  }
	  if(ch == END_LINE_CHAR) { 
	   //next line
	   printf("%c",32);
	   for (k=i; k<=scW; k++){	
    		outputcolor(F_BLUE,B_BLUE);
		gotoxy(k,lineCounter+4);
		printf("%c",32);
		}
	    lineCounter++;
	    i=1;
	  }
	//break when it reaches the end of vertical displaying area
	  if (lineCounter > scH-6) break;
    }
  //delete last 0x0a
  gotoxy(i-1,lineCounter+4);
  outputcolor(F_BLUE,B_BLUE);
  printf(" ");
  //metrics
  gotoxy(1,scH-1);
  outputcolor(F_WHITE,B_BLACK);
  printf("- Lines:         | - Progress:    %c  | - H:    /500",37);
  progress = ((double) currentLine / (double) scrollLimit) * 100;
  gotoxy(10,scH-1);
  outputcolor(F_YELLOW,B_BLACK);
  printf("%ld", linesinFile); 
  gotoxy(32,scH-1);
  outputcolor(F_YELLOW,B_BLACK); 
  if (scrollActive == 1) printf("%d", (int)progress);
  else printf("100"); 
  gotoxy(45,scH-1);
  outputcolor(F_YELLOW,B_BLACK);
  printf("%d", currentColumn); 
 
} 
}

//clean viewing area
void cleanArea(int raw){
int i,j;
   if (raw == 1) {
   for(j=4; j<scH-1;j++)
        for (i=1; i<=scW; i++){
    	  outputcolor(F_BLUE,B_BLUE);
	  gotoxy(i,j);
  	  printf("%c",32);
        }
   }
   else{
   for(j=4; j<scH-1;j++)
        for (i=1; i<=scW; i++){
    	  write_ch(i,j,' ',F_BLUE,B_BLUE);
        }
}
}

char *check_arguments(int argc, char *argv[]){
char *ok=NULL;
//check arguments or display open file dialog
   if(argc > 1) {
    //Does the file exist? Open or create?
    if(file_exists(argv[1]) == 1) {
      //open file in arguments
      clearString(currentPath, MAX_TEXT);
      strcpy(fwfileName, argv[1]);
      ok=getcwd(currentPath, sizeof(currentPath));	//Get path
      strcat(currentPath, "/");
      strcat(currentPath, argv[1]);
      handleopenFile(&filePtr, fwfileName); 
    } else {	      
      //display open file dialog if file does not exist
      strcpy(currentPath, "\0");
      openFileDialog(&openFileData);
      if (openFileData.itemIndex == 0) {
        strcpy(currentPath, "No file to open!");
        strcpy(fwfileName, "No file to open!");
        return 0;
      }
      if (strcmp(openFileData.path, "\0") != 0 && file_exists(openFileData.path)){
        strcpy(currentPath, openFileData.fullPath);
        strcpy(fwfileName, openFileData.path);
        handleopenFile(&filePtr, fwfileName); 
      } else
      {
	//no file selected or file does not exist
        strcpy(currentPath, "No file open!");
        strcpy(fwfileName, "No file open!");
      }
     }
    } else{
	//display open file dialog if no arguments are given
      strcpy(currentPath, "\0");
      openFileDialog(&openFileData);
      if (openFileData.itemIndex == 0) {
        strcpy(currentPath, "No file to open!");
        strcpy(fwfileName, "No file to open!");
        return 0;
      }
 
      if (strcmp(openFileData.path, "\0") != 0 && file_exists(openFileData.path)){
        strcpy(currentPath, openFileData.fullPath);
        strcpy(fwfileName, openFileData.path);
        handleopenFile(&filePtr, fwfileName); 
      } else
      {
	//no file selected or file does not exist
        strcpy(currentPath, "No file open!");
        strcpy(fwfileName, "No file open!");
      }
    }
   if (strcmp(currentPath,"\0") == 0) {
  	write_str(1,3,"No file open!",B_BLACK,F_WHITE);
    } else
    {
	write_str(14,2,fwfileName,B_WHITE,F_BLACK);
	write_str(1,3,currentPath,B_BLACK,F_WHITE);
   }
  update_screen();
  return ok;
}

long checkScrollValues(){
	return (linesinFile - displayLength);
}

/* --------------------------------------*/
//DROP-DOWN MENUS
/* --------------------------------------*/


/*--------------------------*/
/* Display horizontal menu  */
/*--------------------------*/

char horizontal_menu() {
  char    temp_char;
  kglobal=-1;
  loadmenus(mylist, HOR_MENU);
  temp_char = start_hmenu(&data);
  free_list(mylist);
  write_str(1, 2, "File  Help", B_WHITE, F_BLACK);
  write_str(1, 2, "F", B_WHITE, F_BLUE);
  write_str(7, 2, "H", B_WHITE, F_BLUE);
  update_screen();
  return temp_char;
}

/*-------------------------*/
/* Display File menu       */
/*-------------------------*/

void filemenu() {  
  int i=0;
  data.index = OPTION_NIL;
  loadmenus(mylist, FILE_MENU);
  write_str(1, 2, "File", MENU_SELECTOR, MENU_FOREGROUND1);
  draw_window(1, 3, 13, 9, MENU_PANEL, MENU_FOREGROUND0,0, 1,0);
  for (i=2; i<13; i++)
	write_ch(i,7,NHOR_LINE,B_WHITE,F_BLACK);
  kglobal = start_vmenu(&data);
  close_window();
  update_indicators();
  update_screen();
  free_list(mylist);

  if(data.index == OPTION_1) {
    //File info
   fileInfoDialog();
  }
  if(data.index == OPTION_3) {
    //External Module - Open file dialog.
      openFileDialog(&openFileData);
      if (strcmp(openFileData.path, "\0") != 0 && file_exists(openFileData.path)){
        strcpy(currentPath, "\0");
        cleanString(currentPath, MAX_TEXT1);
        cleanString(fwfileName, MAX_TEXT2);
        strcpy(currentPath, openFileData.fullPath);
        strcpy(fwfileName, openFileData.path);
        handleopenFile(&filePtr, fwfileName);
	update_indicators(); 
      }
  }
  if(data.index == OPTION_2) {
	setfile();
  }
  if(data.index == OPTION_4) {
	status = -1;
  }
  data.index = OPTION_NIL;
}

char *setfile(){

char *ok=NULL;
int count=0;
 char tempFile[MAX_TEXT];
   count = inputWindow("New File:", tempFile, "Set file name");
    if(count > 0) {
     cleanString(currentPath, MAX_TEXT1);
     cleanString(fwfileName, MAX_TEXT2);
     strcpy(fwfileName, tempFile);
     if (file_exists(fwfileName)){
       handleopenFile(&filePtr, fwfileName);
       ok=getcwd(currentPath, sizeof(currentPath));	//Get path
       strcat(currentPath, "/");
       strcat(currentPath, fwfileName);
     } else
     {
       infoWindow(mylist, "File does not exist!", "File Information");
       cleanString(currentPath, MAX_TEXT1);
       cleanString(fwfileName, MAX_TEXT2);
       currentPath[0] ='\0';
       fwfileName[0] ='\0';
       filePtr = NULL;
       scrollActive = 0;
       cleanArea(0);
       displayLogo = 1; 
       main_screen();
     }
     update_indicators();
    }
   return ok;
}
/*--------------------------*/
/* Display Help menu        */
/*--------------------------*/

void helpmenu() { 

  data.index = OPTION_NIL;
  loadmenus(mylist, HELP_MENU);
  write_str(7, 2, "Help", MENU_SELECTOR, MENU_FOREGROUND1);
  draw_window(7, 3, 16, 6, MENU_PANEL, MENU_FOREGROUND0, 0,1,0);
  kglobal = start_vmenu(&data);
  close_window();
  write_str(1, 2, "File  Help", B_WHITE, F_BLACK);
  write_str(1, 2, "F", B_WHITE, F_BLUE);
  write_str(7, 2, "H", B_WHITE, F_BLUE);
  update_screen();
  free_list(mylist);
  if(data.index == OPTION_1) {
    //Help info
    help_info();
  }
  if(data.index == OPTION_2) {
    //About info
    about_info();
  }
  data.index = -1;
}

/*----------------------*/
/* Drop_Down Menu Loop  */
/*----------------------*/

void drop_down(char *kglobal) {
/* 
   Drop_down loop animation. 
   K_LEFTMENU/K_RIGHTMENU -1 is used when right/left arrow keys are used
   so as to break vertical menu and start the adjacent menu
   kglobal is changed by the menu functions.
*/
  do {
    if(*kglobal == K_ESCAPE) {
      //Exit drop-down menu with ESC           
      *kglobal = 0;
      break;
    }
    if(data.index == FILE_MENU) {
      filemenu();
      if(*kglobal == K_LEFTMENU) {
	data.index = HELP_MENU;
      }
      if(*kglobal == K_RIGHTMENU) {
	data.index = HELP_MENU;
      }
    }
    if(data.index == HELP_MENU) {
      helpmenu();
      if(*kglobal == K_LEFTMENU) {
	data.index = FILE_MENU;
      }
      if(*kglobal == K_RIGHTMENU) {
	data.index = FILE_MENU;
      }
    }
  } while(*kglobal != K_ENTER);
}

/*------------------*/
/* File Info Dialog */
/*------------------*/

int fileInfoDialog() {
  long    size = 0, lines = 0;
  int     i;
  char    sizeStr[20];
  char    linesStr[20];
  char    tempMsg[150];
  char    pathtxt[60];
  if(filePtr != NULL) {
    size = getfileSize(filePtr);
    lines = countLinesFile(filePtr);
    if(size <= 0)
      size = 0;
    if(lines <= 0)
      lines = 0;
    sprintf(sizeStr, "%ld", size);
    sprintf(linesStr, "%ld", lines);
    strcpy(tempMsg, "[+] File Data:\n- ");
    strcat(tempMsg, sizeStr);
    strcat(tempMsg, " bytes.\n- ");
    strcat(tempMsg, "");
    strcat(tempMsg, linesStr);
    strcat(tempMsg, " lines.\n[");
    for (i=0;i<60;i++){
        if (i!=30) pathtxt[i] = currentPath[i];
        else pathtxt[30] = '\n';
    }
    pathtxt[59] = CHAR_NIL;
    strcat(tempMsg, pathtxt);
    strcat(tempMsg, "]");
    alertWindow(mylist, tempMsg, "File Information");
  } else {
    infoWindow(mylist, "No file open!", "File Information");
  }
  return 0;
}

void about_info(){
  char    msg[200];
  msg[0] = '\0';
  strcat(msg, ALINE1);
  strcat(msg, ALINE2);
  strcat(msg, ALINE3);
  strcat(msg, ALINE4);
  alertWindow(mylist, msg,"ABOUT");

}
int help_info() {
  int     ok = 0;
  char    msg[500];
  msg[0] = '\0';
  strcat(msg, HELP1);		//located in user_inter.h
  strcat(msg, HELP2);		//located in user_inter.h
  strcat(msg, HELP3);		//located in user_inter.h
  strcat(msg, HELP4);		//located in user_inter.h
  strcat(msg, HELP5);		//located in user_inter.h
  strcat(msg, HELP6);		//located in user_inter.h
  strcat(msg, HELP7);		//located in user_inter.h
  strcat(msg, HELP8);		//located in user_inter.h
  strcat(msg, HELP9);		//located in user_inter.h
  strcat(msg, HELP10);		//located in user_inter.h
  strcat(msg, HELP11);		//located in user_inter.h
  strcat(msg, "\0");
  helpWindow(mylist, msg, "HELP");
  refresh_screen();
  return ok;
}

/* --------------------------------------*/
//CREDITS
/* --------------------------------------*/

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
 if (displayLogo ==1){
  for (i=0; i<ABOUT_LINES; i++){
	outputcolor(F_WHITE, B_BLUE);
	gotoxy((scW/2) - (80/2), ((scH/2) - (ABOUT_LINES/2)) + i);
        printf("%s",about_msg[i]);
  }
 }
if (openFileData.itemIndex != 0) {
    free(openFileData.item);
    free(openFileData.path);
  }
 free_buffer();
 resetTerm();
 outputcolor(B_BLUE,F_WHITE);
 gotoxy(((scW/2)-10),1);
 printf("%s\n",msg); 

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
      outputcolor(F_BLUE,B_BLACK);
      if (j<30) {
	if (j== 1 || j== 9)  printf("%c\n",cmsg[j]);   
        if (j> 22) {outputcolor(FH_BLACK,B_BLACK); printf("%c\n",cmsg[j]);}      
      }
      j++;
      }  
 }
 } while (mytimer1.ticks<53);
  printf("\n\r%c",127);
  showcursor();
  resetAnsi(0);
  showcursor();
}



