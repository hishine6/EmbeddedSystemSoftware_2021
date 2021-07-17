#ifndef __INPUT_H
#define __INPUT_H

#include "global.h"
#include <pthread.h>


#define SWITCH_DEVICE "/dev/fpga_push_switch"
#define READKEY_DEVICE "/dev/input/event0"

#define KEY_RELEASE 0
#define KEY_PRESS 1

#define KEY_HOME 102
#define KEY_BACK 158
#define KEY_PROG 116
#define KEY_VOL_UP 115
#define KEY_VOL_DOWN 114

bool Input_Termination=false; // Is input finished?
int dev_switch; // switch device fd
int dev_readkey; // readkey device fd

int input_process();
/*---------------------------------------
  input process starts here, this function
  first, gets shared memory
  second, opends devices(switch, read key)
  last, creates threads for input
  --------------------------------------*/
void *thread_func(void*);
/*---------------------------------------
  this function devides two threads, it's used
  to give both thread loop 0.2 sec of sleep
  time
  --------------------------------------*/
int readkey();
/*---------------------------------------
  Actual function that get's input from
  readkey. If KEY_PRESS is the value, it
  saves the input shared memory according 
  to that KEY
  --------------------------------------*/
int read_switch();
/*---------------------------------------
  Same function as readkey, just used for
  switch. Since it's non-blocking read, 
  switch needs an interval to read not 
  the input stae, but the input rising edge
  making it more accurate
  --------------------------------------*/


#endif
