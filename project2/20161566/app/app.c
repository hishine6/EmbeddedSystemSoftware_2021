#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/ioctl.h>

#define SET_OPTION _IOW(242,1,struct param)
#define START_COMMAND _IOW(242,2,struct param)
#define KERNEL_TIMER_NAME "/dev/dev_driver" 

struct param{
		int timer_interval;
		int timer_cnt;
		int timer_init;
};


int main(int argc, char **argv)
{
	int led_fd;
	int get_number=0;
	struct param parameter;
	if(argc != 4) { 		// Exception handling
		printf("Usage : TIMER_INTERVAL[1-100] TIMER_CNT[1-100] TIMER_INIT[0001-8000]\n");
		return -1;
	}

	led_fd = open(KERNEL_TIMER_NAME, O_WRONLY);	// device file open 
	if (led_fd<0){
		printf("Open Failured!\n");
		return -1;
	}

	parameter.timer_interval = atoi(argv[1]);	// timer_interval: interval of timer expire
	parameter.timer_cnt = atoi(argv[2]);		// timer_cnt : total number of iteration
	parameter.timer_init = atoi(argv[3]);		// timer_init: initial value of the device


	
	ioctl(led_fd,SET_OPTION,&parameter);		// ioctl for initializing the state of device
	ioctl(led_fd,START_COMMAND);				// Initialize timer, Start running based on state
	close(led_fd);	// devic file close
	return 0;
}

