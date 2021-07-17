#include "global.h"
#include "input_process.h"

char* shmaddr;

int input_process(){
		int tid;
		pthread_t p_thread[2];
		char t1[]="read_key";
		char t2[]="switch";

		//shared memory
		shmaddr = (char*)shmat((shmid_input),(char*)NULL,0);
		//open switch
		if((dev_switch=open("/dev/fpga_push_switch",O_RDWR))==-1){
				printf("ERROR: Device Open Error: %s\n",SWITCH_DEVICE);
				exit(1);
		}
		//open readkey
		if((dev_readkey=open(READKEY_DEVICE,O_RDONLY))==-1){
				printf("ERROR: Device Open Error: %s\n",READKEY_DEVICE);
				exit(1);
		}

		//thread create
		if((tid=pthread_create(&p_thread[0],NULL,thread_func,(void*)t1))<0){
				printf("ERROR: Thread creation failed\n");
				exit(1);
		}
		if((tid=pthread_create(&p_thread[1],NULL,thread_func,(void*)t2))<0){
				printf("ERROR: Thread creation failed\n");
				exit(1);
		}

		usleep(100000);

		pthread_join(p_thread[0],0);
		pthread_join(p_thread[1],0);

		close(dev_switch);
		close(dev_readkey);
		shmdt((char*)shmaddr);
		exit(0);
}

void*thread_func(void *data){
		while(!Input_Termination){
				if(!strcmp((const char*)data, "read_key")){
						readkey();
				}
				else{
						read_switch();
				}

				//switch reads so fast, needs to wait for 0.2 sec
				usleep(200000);
		}
}

int readkey(){
		struct input_event ev[64];
		int rd;
		int size=sizeof(struct input_event);
		if((rd=read(dev_readkey,ev,size*64))<size){
				printf("ERROR: Readky failed\n");
				return -1;
		}

		if(ev[0].value==KEY_PRESS){	//if pressed
				sem_wait(&m_input);
				if(shmaddr[0]==0){
						switch(ev[0].code){		//save the pressed status
								case KEY_BACK:
										shmaddr[1]=1;
										Input_Termination=true;
										break;
								case KEY_VOL_UP:
										shmaddr[1]=2;
										break;
								case KEY_VOL_DOWN:
										shmaddr[1]=3;
										break;
								default:
										shmaddr[0]=0;
										break;

						}
						shmaddr[0]=1;
				}
				sem_post(&m_input);

		}
}

int read_switch(){
		int i, t;
		int target;
		int flag=0;
		unsigned char push_sw_buff[MAX_BUTTON];
		unsigned char prev_sw_buff[MAX_BUTTON];

		memset(prev_sw_buff,0,MAX_BUTTON);

		sem_wait(&m_input);
		if(shmaddr[2]==0){
				for(t=0;t<5000;t++){	//give it 5000 interval to read the 'rising edge'
						read(dev_switch, &push_sw_buff,MAX_BUTTON);
						for(i=0;i<MAX_BUTTON;i++){
								if(prev_sw_buff[i]==0 && push_sw_buff[i]==1){
										flag=1;
										shmaddr[i+3]=1;
								}
								prev_sw_buff[i]=push_sw_buff[i];
						}
				}
				if(flag==1)
						shmaddr[2]=1;
		}
		sem_post(&m_input);
}

