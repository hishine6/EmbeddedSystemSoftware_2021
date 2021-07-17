#include "global.h"
#include "output_process.h"

#define OUTPUT_BUFFER_SIZE 30
char* output_shmaddr;
/********************* FND  ****************/

void *FND(){
		int dev;
		int retval;
		int i;
		unsigned char fnd_data[FND_MAX_DIGIT];
		dev=open(FND_DEVICE,O_RDWR);
		if(dev<0){
				printf("Device open error : %s\n",FND_DEVICE);
				exit(1);
		}
		
		while(!Output_Terminated){
				memset(fnd_data,0,sizeof(fnd_data));
				output_mode=output_shmaddr[0];
				if(output_shmaddr[1]==1){
						for(i=0;i<FND_MAX_DIGIT;i++){
							fnd_data[i]=output_shmaddr[i+2];
						}
						output_shmaddr[1]=0;
						retval=write(dev,&fnd_data,FND_MAX_DIGIT);
						if(retval<0){
								printf("FND error\n");
								exit(1);
						}
				}
				sem_post(&m_output);

				usleep(500);

		}
		
		close(dev);
}

/********************* LED *****************/


void LED_MODE1(int led_mode, unsigned char*led_addr){
		unsigned char data;
		if(led_mode==1){
				data=128;
				*led_addr=data;
		}
		else if(led_mode==2){
				data=32;
				*led_addr=data;
				usleep(1000000);
				if(output_mode==0){
					data=16;
					*led_addr=data;
					usleep(1000000);
				}
		}
}
void LED_MODE2(int led_mode, unsigned char* led_addr){
		unsigned char data;
		if(led_mode==1){
				data=64;
		}
		else if(led_mode==2){
				data=32;
		}
		else if(led_mode==3){
				data=16;
		}
		else{
				data=128;
		}
		*led_addr=data;
}

void LED_MODE3_4(unsigned char* led_addr){
		*led_addr=0;
}

void *LED(){
		int fd, i;
		int led_mode=1;
		unsigned long *fpga_addr=0;
		unsigned char *led_addr=0;
		unsigned char data;

		fd=open("/dev/mem",O_RDWR | O_SYNC);
		if(fd<0){
				perror("/dev/mem open error");
				exit(1);
		}

		fpga_addr = (unsigned long*)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, FPGA_BASE_ADDRESS);
		if(fpga_addr==MAP_FAILED){
				printf("mmap error!\n");
				close(fd);
				exit(1);
		}
		led_addr=(unsigned char*)((void*)fpga_addr+LED_ADDR);
		//read data
		//if mode changed turn off
		//if not, decide if it's mode1 or mode2
	
		while(1){
				sem_wait(&m_output);
				output_mode=output_shmaddr[0];
				if(output_shmaddr[6]==1){
						led_mode=output_shmaddr[7];
						output_shmaddr[6]=0;	
				}
				sem_post(&m_output);

				switch(output_mode){
						case 0:
								LED_MODE1(led_mode,led_addr);
								break;
						case 1:
								LED_MODE2(led_mode,led_addr);
								break;
						default:
								LED_MODE3_4(led_addr);
								break;
				}
				usleep(400);
		}
		
		munmap(led_addr,4096);
		close(fd);

}

/********************* LCD *****************/

void LCD_DEFAULT(int lcd_dev){
		unsigned char string[32];
		memset(string,' ',sizeof(string));
		write(lcd_dev,string,LCD_BUFF_SIZE);
}

void *LCD(){
		int i;
		int lcd_dev;
		unsigned char string[32];
		memset(string,' ',sizeof(string));

		lcd_dev=open(LCD_DEVICE,O_WRONLY);
		if(lcd_dev<0){
				printf("Device open err: %s\n",LCD_DEVICE);
				exit(1);
		}
		while(1){
				output_mode=output_shmaddr[0];
				if(output_mode!=2){
						memset(string,0,sizeof(string));
						LCD_DEFAULT(lcd_dev);
						continue;
				}
				sem_wait(&m_output);
				if(output_shmaddr[8]==1){
						for(i=0;i<LCD_BUFF_SIZE;i++)
								string[i]=output_shmaddr[i+9];
						output_shmaddr[8]=0;
						for(i=0;i<LCD_BUFF_SIZE;i++)
								if(string[i]==0)
										string[i]=' ';
						write(lcd_dev,string,LCD_BUFF_SIZE);
				}
				sem_post(&m_output);
				
		}
		close(lcd_dev);
}

/******************** DOT ******************/

void DOT_DEFAULT(int dot_dev){
		write(dot_dev,fpga_set_blank,MAX_FPGA_SIZE);
}

void DOT_MODE4(int dot_dev, unsigned char mode4_output[10], unsigned char mode4_output_cursor[10]){
		write(dot_dev,mode4_output,MAX_FPGA_SIZE);
		usleep(1000000);
		if(output_mode==3){
			write(dot_dev,mode4_output_cursor,MAX_FPGA_SIZE);
			usleep(1000000);
		}
}

void *DOT(){
		int i;
		int dot_dev;
		unsigned char mode4_output[10];
		unsigned char mode4_output_cursor[10];

		dot_dev=open(DOT_DEVICE,O_WRONLY);
		if(dot_dev<0){
				printf("Device open error : %s\n",DOT_DEVICE);
				exit(1);
		}

		while(1){
				sem_wait(&m_output);
				output_mode=output_shmaddr[0];
				if(output_shmaddr[41]==1){
					for(i=0;i<10;i++)
							mode4_output[i]=output_shmaddr[42+i];
					if(output_mode==3){
							for(i=0;i<10;i++)
									mode4_output_cursor[i]=output_shmaddr[52+i];
					}
					output_shmaddr[41]=0;
				
				}
				sem_post(&m_output);
				
				switch(output_mode){
						case 2:
								memset(mode4_output_cursor,0,sizeof(mode4_output_cursor));
								write(dot_dev,mode4_output,MAX_FPGA_SIZE);
								break;
						case 3:
								DOT_MODE4(dot_dev, mode4_output,mode4_output_cursor);
								break;
						default:
								memset(mode4_output_cursor,0,sizeof(mode4_output_cursor));
								memset(mode4_output,0,sizeof(mode4_output));
								DOT_DEFAULT(dot_dev);
								break;

				}

		}
		close(dot_dev);
}


void output_process(){ //where it starts
		pthread_t p_thread[4];
		output_mode=0;
		//connect with IPC
		output_shmaddr= (char*)shmat(shmid_output,(char*)NULL,0);
		
		//FND
		if(pthread_create(&p_thread[0],NULL,FND,NULL)<0){
				printf("FND thread creation failed!\n");
				exit(-1);
		}
		//LED
		if(pthread_create(&p_thread[1],NULL,LED,NULL)<0){ 
				printf("LED thread creation failed!\n");
				exit(-1);
		}
		//LCD
		if(pthread_create(&p_thread[2],NULL,LCD,NULL)<0){
				printf("LCD thread creation failed!\n");
				exit(-1);
		}
		//DOT_MATRIX
		if(pthread_create(&p_thread[3],NULL,DOT,NULL)<0){
				printf("DOT MATRIX thread creation failed!\n");
				exit(-1);
		}


		usleep(200000);

		pthread_join(p_thread[0],0);
		pthread_join(p_thread[1],0);
		pthread_join(p_thread[2],0);
		pthread_join(p_thread[3],0);

		//disconnect IPC
		shmdt((char*)output_shmaddr);
		exit(0);
}

