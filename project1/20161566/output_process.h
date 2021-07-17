#ifndef __OUTPUT_H
#define __OUTPUT_H

#include "global.h"
#include <pthread.h>
#include <sys/mman.h>


#define FND_DEVICE "/dev/fpga_fnd"
#define LED_DEVICE "/dev/fpga_led"
#define DOT_DEVICE "/dev/fpga_dot"
#define LCD_DEVICE "/dev/fpga_text_lcd"


#define FPGA_BASE_ADDRESS 0x08000000 //fpga_base address
#define LED_ADDR 0x16

#define FND_MAX_DIGIT 4

bool Output_Terminated=false;
static int output_mode;

//MODE3
#define MAX_FPGA_SIZE 10



unsigned char fpga_set_blank[10]={
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};


void output_process();
/*----------------------------------
  Starts the output process first 
  connects to shared memory, than
  makes 4 threads, each for output
  devices. Than waits for each thread
  to be created 
  ----------------------------------*/

/************************** FND *****************************/

void *FND();
/*----------------------------------
  Thread for FND, it reads index<1~5>
  Prints to the FND device what it got
  ----------------------------------*/

/************************** LED ****************************/

void *LED();
/*----------------------------------
  Thread for LED, it reads index<6~7>
  Actually it reads the 'mode'of each 
  mode, and prints the output according
  to that. it calls the functions below
  --------------------------------*/

  void LED_MODE1(int, unsigned char*);
/*----------------------------------
  prints the output according to mode
  ----------------------------------*/

void LED_MODE2(int, unsigned char*);
/*----------------------------------
  prints the output according to mode
  ----------------------------------*/

void LED_MODE3_4(unsigned char*);
/*----------------------------------
  prints nothing. IT doesn't actually
  even needs to read form the shared
  memory
  ----------------------------------*/

/************************** LCD ***************************/

void LCD_DEFAULT(int);
/*----------------------------------
  Makes the LCD to print nothing,
  used to print during other modes
  rather than mode 3
  ----------------------------------*/

void *LCD();
/*----------------------------------
  LCD thread, prints  the char[32] string
  to the LCD board
  ----------------------------------*/

/************************** DOT ****************************/

void DOT_DEFAULT(int);
/*----------------------------------
  wirtes a blank DOT MATRIX 
  ----------------------------------*/

void DOT_MODE4(int, unsigned char a[10], unsigned char b[10]);
/*----------------------------------
  on mode4 the cursor should blink,
  to make this happen we use two DOT
  MATRIX boards, one with the cursor
  and one without. interval its 1sec
  and if the cursor off mode is on the
  two boards are the same
  ----------------------------------*/

void *DOT();
/*----------------------------------
  DOT thread starts, Calls either 
  DOT_DEFAULT or DOT_MODE4 or just
  writes 'A' or '1' which was sent
  from main process
  ----------------------------------*/

#endif
