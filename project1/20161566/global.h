#ifndef _GLOBAL_H
#define _GLOBAL_H

//basic
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<stdbool.h>

//fork
#include<unistd.h>

//IPC
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/select.h>
#include<time.h>
#include<sys/sem.h>
#include<semaphore.h>

//device 
#include<fcntl.h>

//input 
#include<dirent.h>
#include<linux/input.h>
#include<termios.h>
#include<signal.h>

#define MAX_BUTTON 9				// switch
#define LCD_BUFF_SIZE 32			// LCD size
#define INPUT_BUFFER_SIZE 20		// input buffer max size

/**************************************** MODE 1 *****************************************/
static int saved_hour;			
static int saved_min;
static int temp_hour;				// used when modifying time
static int temp_min;				// used when modifying time
static bool mode1_mode=false;		// false: not modifying, true: modifying

int main_process_mode1();
/*---------------------------------------
  Mode1 starts from here it looks into
  input buffer and change time, mode, etc
 ---------------------------------------*/

void initial_mode1();
/*---------------------------------------
  Initializing board to mode1
  FND - current time
  LED - 1(128)
  LCD - nothing
  DOT - nothing
  --------------------------------------*/

void send_time();
/*---------------------------------------
  Sets the output buffer FND part to 
  time that you want to print, it sends
  different values according to mode1_mode
  --------------------------------------*/

void get_curr_time();
/*---------------------------------------
  get the current time of the board and 
  set it to temp or saved time according to
  mode
  --------------------------------------*/

/**************************************** MODE 2 *****************************************/
static int mode2_count;				// Number to print
static int mode2_count_type;		// 1: decimal, 2: Octal, 3: quatenary, 4: binary

int main_process_mode2();
/*---------------------------------------
  Mode2 starts from here, it reads the
  input buffer and updates the saved number
  and count
  --------------------------------------*/

void initial_mode2();
/*---------------------------------------
  Initialize for Mode2
  FND - nothing(0000)
  LED - 2(64)
  LCD - nothing
  DOT - nothing
  --------------------------------------*/

/**************************************** MODE 3 *****************************************/
static unsigned char fpga_number[2][10]={
		        {0x1c,0x36,0x63,0x63,0x63,0x7f,0x7f,0x63,0x63,0x63}, // A
				        {0x0c,0x1c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x3f,0x3f} //1
};
static int mode3_count;							// number to print on FND
static int mode3_mode;							// 1: 'A', 2: '1'
static unsigned char lcd_string[LCD_BUFF_SIZE];	// string to print on LCD
static int lcd_last_point;						// last index of current 'lcd_string'

int main_process_mode3();
/*---------------------------------------
  reads from input buffer, changes the 
  last value of the string according to
  the previous character. If mode is number
  add number to the end set DOT according to
  mode('A' or '1')
  --------------------------------------*/

void initial_mode3();
/*---------------------------------------
  Initialize for Mode3
  FND - nothing(0000)
  LED - nothing
  LCD - nothing
  DOT - 'A'
  --------------------------------------*/

void mode3_add_next(char);
/*---------------------------------------
  so, this function is used to add a new
  character to the end of the string. It
  takes into consider if the string is full
  --------------------------------------*/

void shift_string(char);
/*---------------------------------------
  This function just shifts the string 
  right, and adds the new character to the
  end
  --------------------------------------*/

/**************************************** MODE 4 *****************************************/
static int mode4_count;							// number to print on FND
static int mode4_mode;							// 1: Cursor blink, 2: No Cursor
static unsigned char mode4_fpga_number[10];		// printing to DOT
static int cursor;								// Current place of Cursor

int main_process_mode4();
/*---------------------------------------
  Initialize mode 4, reads input buffer and
  changes the cursor or do the action
  according to the input
  --------------------------------------*/

void initial_mode4();
/*---------------------------------------
  Initializing mode4
  FND - nothing(0000)
  LED - nothing
  LCD - nothing
  DOT - one dot(0x40)on the top
  --------------------------------------*/

/**************************************** Main  ******************************************/
int mode;								// MODE
sem_t m_output; 								// Semaphore for input
sem_t m_input;  								// Semaphore for output
int shmid_input;							// shared memory between main <-> input
int shmid_output;						// shared memory between main <-> output
static bool initial=true;						// Is this mode Just been started?
static unsigned char input_buffer[INPUT_BUFFER_SIZE];	// Save message from 'input process'

void main_process();
/*---------------------------------------
  Starts the main process, gets information
  from input process and send it to 
  output process
   --------------------------------------*/

int initial_mode();        
/*---------------------------------------
  This function is called when modes is changed
  it first resets all the devices, then it
  sets the devices to initial state that the
  current mode needs
  ---------------------------------------*/

int check_input_to_main(); 
/*---------------------------------------
  This function is used to read from the 
  input "Shared Memory IPC". It saves the
  input switch to "input_buffer"
  ---------------------------------------*/



#endif
