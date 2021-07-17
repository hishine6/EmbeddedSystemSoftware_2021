#include "global.h"
char* shmaddr_input;
char* shmaddr_output;
pid_t  pid_input, pid_output;

// Specific explanation in <global.h> 

/************************************ MODE 1 ************************************************/

void get_curr_time(){
		time_t t=time(NULL);
		struct tm timer=*localtime(&t);

		if(mode1_mode){
				temp_hour=timer.tm_hour;
				temp_min=timer.tm_min;
		}
		else{
				saved_hour=timer.tm_hour;
				saved_min=timer.tm_min;
		}
}
void initial_mode1(){
		mode1_mode=false;
		shmaddr_output[2]=saved_hour/10;
		shmaddr_output[3]=saved_hour%10;
		shmaddr_output[4]=saved_min/10;
		shmaddr_output[5]=saved_min%10;
		shmaddr_output[7]=1;
}


void send_time(){
		int i;

		sem_wait(&m_output);

		if(mode1_mode){
				shmaddr_output[2]=temp_hour/10;
				shmaddr_output[3]=temp_hour%10;
				shmaddr_output[4]=temp_min/10;
				shmaddr_output[5]=temp_min%10;
				shmaddr_output[6]=1;
				shmaddr_output[7]=2;
		}
		else{
				shmaddr_output[2]=saved_hour/10;
				shmaddr_output[3]=saved_hour%10;
				shmaddr_output[4]=saved_min/10;		
				shmaddr_output[5]=saved_min%10;
				shmaddr_output[6]=1;
				shmaddr_output[7]=1;
		}
		shmaddr_output[1]=1;
		sem_post(&m_output);
}


int main_process_mode1(){		

		if(mode1_mode && input_buffer[2]==1){	
				get_curr_time();
				send_time();
		}
		else if(input_buffer[1]==1){ //change mode of 'clock'
				printf("mode changed\n");
				if(mode1_mode){
						mode1_mode=false;
						saved_hour=temp_hour;
						saved_min=temp_min;
				}
				else{
						mode1_mode=true;
						temp_hour=saved_hour;
						temp_min = saved_min;
				}
				send_time();
		}
		else if(mode1_mode && (input_buffer[3]==1)){ //increase cur_hour
				printf("update hour\n");
				temp_hour+=1;
				temp_hour%=24;
				send_time();
		}
		else if(mode1_mode && (input_buffer[4]==1)){ //increase cur_min
				printf("update minute\n");
				temp_min+=1;
				temp_min%=60;
				send_time();
		}

		return 0;
}


/************************************* MODE 2 *******************************************/
int main_process_mode2(){
		int temp =1;
		unsigned char output_counter[4];
		if(input_buffer[1]==1){ //change counter data type
				mode2_count_type%=4;
				mode2_count_type+=1;

		}
		if(input_buffer[2]==1){ // add 100 to count
				switch(mode2_count_type){
						case 1:
								mode2_count+=100;
								break;
						case 2:
								mode2_count+=64;
								break;
						case 3:
								mode2_count+=16;
								break;
						case 4:
								mode2_count+=4;
								break;
				}							
		}
		if(input_buffer[3]==1){ // add 10 to count
				switch(mode2_count_type){
						case 1:
								mode2_count+=10;
								break;
						case 2:
								mode2_count+=8;
								break;
						case 3:
								mode2_count+=4;
								break;
						case 4:
								mode2_count+=2;
								break;
				}
		}
		if(input_buffer[4]==1){ // add 1 to count
				mode2_count+=1;
		}
		output_counter[0]=0;
		switch(mode2_count_type){
				case 1:
						temp=mode2_count%1000;
						output_counter[1]=temp/100;
						temp%=100;
						output_counter[2]=temp/10;
						output_counter[3]=temp%10;
						break;
				case 2:
						temp=mode2_count%512;
						output_counter[1]=temp/64;
						temp%=64;
						output_counter[2]=temp/8;
						output_counter[3]=temp%8;
						break;
				case 3:
						temp=mode2_count%64;
						output_counter[1]=temp/16;
						temp%=16;
						output_counter[2]=temp/4;
						output_counter[3]=temp%4;
						break;
				case 4:
						output_counter[1]=(mode2_count & 4)>>2;
						output_counter[2]=(mode2_count & 2)>>1;
						output_counter[3]=mode2_count & 1;
						break;
		}

		sem_wait(&m_output);
		//FND
		shmaddr_output[2]=output_counter[0];
		shmaddr_output[3]=output_counter[1];
		shmaddr_output[4]=output_counter[2];
		shmaddr_output[5]=output_counter[3];
		//LED
		shmaddr_output[7]=mode2_count_type;
		//Check_byte
		shmaddr_output[1]=1;
		shmaddr_output[6]=1;
		sem_post(&m_output);

		return 0;

}

void initial_mode2(){
		mode2_count=0;
		mode2_count_type=1;
		//LED: 0
		shmaddr_output[7]=1;
}

/************************************** MODE 3 **************************************************/
void shift_string(char a){
		int i;
		lcd_last_point--;
		for(i=0;i<LCD_BUFF_SIZE-1;i++)
				lcd_string[i]=lcd_string[i+1];
		lcd_string[LCD_BUFF_SIZE-1]=a;
}

void mode3_add_next(char a){
		if(lcd_string[lcd_last_point]!=0)
				lcd_last_point++;

		if(lcd_last_point==LCD_BUFF_SIZE)
				shift_string(a);
		else
				lcd_string[lcd_last_point]=a;
}


int main_process_mode3(){
		//comming into this function means that a switch have been pushed!
		int temp_count;
		int i;
		mode3_count++;

		if(input_buffer[2]==1 && input_buffer[3]==1){ //set to 0
				memset(lcd_string,0,sizeof(lcd_string));
				lcd_last_point=0;
		}
		else if(input_buffer[5]==1 && input_buffer[6]==1){
				shmaddr_output[41]=1; // change mode
				if(mode3_mode==1)
						mode3_mode=2;
				else
						mode3_mode=1;
		}
		else if(input_buffer[8]==1 && input_buffer[9]==1){ //skip one step
				mode3_add_next(' ');
		}
		else if(input_buffer[1]==1){
				if(mode3_mode==2){	//number 
						mode3_add_next('1');
				}
				else if(lcd_string[lcd_last_point]==0){
						lcd_string[lcd_last_point]='.';
				}
				else if(lcd_string[lcd_last_point]=='Q'){
						lcd_string[lcd_last_point]='Z';
				}
				else if(lcd_string[lcd_last_point]=='Z'){
						lcd_string[lcd_last_point]='.';
				}
				else if(lcd_string[lcd_last_point]=='.'){
						lcd_string[lcd_last_point]='Q';
				}
				else{
						mode3_add_next('.');
				}
		}
		else if(input_buffer[2]==1){
				if(mode3_mode==2){
						mode3_add_next('2');
				}
				else if(lcd_string[lcd_last_point]==0){
						lcd_string[lcd_last_point]='A';
				}
				else if(lcd_string[lcd_last_point]=='A'){
						lcd_string[lcd_last_point]='B';
				}
				else if(lcd_string[lcd_last_point]=='B'){
						lcd_string[lcd_last_point]='C';
				}
				else if(lcd_string[lcd_last_point]=='C'){
						lcd_string[lcd_last_point]='A';
				}
				else{
						mode3_add_next('A');
				}
		}
		else if(input_buffer[3]==1){
				if(mode3_mode==2)
						mode3_add_next('3');
				else if(lcd_string[lcd_last_point]==0)
						lcd_string[lcd_last_point]='D';
				else if(lcd_string[lcd_last_point]=='D')
						lcd_string[lcd_last_point]='E';
				else if(lcd_string[lcd_last_point]=='E')
						lcd_string[lcd_last_point]='F';
				else if(lcd_string[lcd_last_point]=='F')
						lcd_string[lcd_last_point]='D';
				else
						mode3_add_next('D');
		}
		else if(input_buffer[4]==1){
				if(mode3_mode==2)
						mode3_add_next('4');
				else if(lcd_string[lcd_last_point]==0)
						lcd_string[lcd_last_point]='G';
				else if(lcd_string[lcd_last_point]=='G')
						lcd_string[lcd_last_point]='H';
				else if(lcd_string[lcd_last_point]=='H')
						lcd_string[lcd_last_point]='I';
				else if(lcd_string[lcd_last_point]=='I')
						lcd_string[lcd_last_point]='G';
				else
						mode3_add_next('G');

		}
		else if(input_buffer[5]==1){
				if(mode3_mode==2)
						mode3_add_next('5');
				else if(lcd_string[lcd_last_point]==0)
						lcd_string[lcd_last_point]='J';
				else if(lcd_string[lcd_last_point]=='J')
						lcd_string[lcd_last_point]='K';
				else if(lcd_string[lcd_last_point]=='K')
						lcd_string[lcd_last_point]='L';
				else if(lcd_string[lcd_last_point]=='L')
						lcd_string[lcd_last_point]='J';
				else
						mode3_add_next('J');

		}
		else if(input_buffer[6]==1){
				if(mode3_mode==2)
						mode3_add_next('6');
				else if(lcd_string[lcd_last_point]==0)
						lcd_string[lcd_last_point]='M';
				else if(lcd_string[lcd_last_point]=='M')
						lcd_string[lcd_last_point]='N';
				else if(lcd_string[lcd_last_point]=='N')
						lcd_string[lcd_last_point]='O';
				else if(lcd_string[lcd_last_point]=='O')
						lcd_string[lcd_last_point]='M';
				else
						mode3_add_next('M');

		}
		else if(input_buffer[7]==1){
				if(mode3_mode==2)
						mode3_add_next('7');
				else if(lcd_string[lcd_last_point]==0)
						lcd_string[lcd_last_point]='P';
				else if(lcd_string[lcd_last_point]=='P')
						lcd_string[lcd_last_point]='R';
				else if(lcd_string[lcd_last_point]=='R')
						lcd_string[lcd_last_point]='S';
				else if(lcd_string[lcd_last_point]=='S')
						lcd_string[lcd_last_point]='P';
				else
						mode3_add_next('P');

		}
		else if(input_buffer[8]==1){
				if(mode3_mode==2)
						mode3_add_next('8');
				else if(lcd_string[lcd_last_point]==0)
						lcd_string[lcd_last_point]='T';
				else if(lcd_string[lcd_last_point]=='T')
						lcd_string[lcd_last_point]='U';
				else if(lcd_string[lcd_last_point]=='U')
						lcd_string[lcd_last_point]='V';
				else if(lcd_string[lcd_last_point]=='V')
						lcd_string[lcd_last_point]='T';
				else
						mode3_add_next('T');

		}
		else if(input_buffer[9]==1){
				if(mode3_mode==2)
						mode3_add_next('9');
				else if(lcd_string[lcd_last_point]==0)
						lcd_string[lcd_last_point]='W';
				else if(lcd_string[lcd_last_point]=='W')
						lcd_string[lcd_last_point]='X';
				else if(lcd_string[lcd_last_point]=='X')
						lcd_string[lcd_last_point]='Y';
				else if(lcd_string[lcd_last_point]=='Y')
						lcd_string[lcd_last_point]='W';
				else
						mode3_add_next('W');

		}
		else{
		}

		//Output
		temp_count=mode3_count%10000;
		sem_wait(&m_output);
		//FND
		shmaddr_output[2]=temp_count/1000;
		temp_count%=1000;
		shmaddr_output[3]=temp_count/100;
		temp_count%=100;
		shmaddr_output[4]=temp_count/10;
		shmaddr_output[5]=temp_count%10;

		//NO LED

		///DOT MATRIX
		if(mode3_mode==1){
				for(i=0;i<10;i++)
						shmaddr_output[42+i]=fpga_number[0][i];
		}
		else if(mode3_mode==2){
				for(i=0;i<10;i++)
						shmaddr_output[42+i]=fpga_number[1][i];
		}

		//LCD
		for(i=0;i<LCD_BUFF_SIZE;i++){
				if(lcd_string[i]==0)
						shmaddr_output[i+9]=' ';
				else
						shmaddr_output[i+9]=lcd_string[i];
		}

		//Check byte
		shmaddr_output[1]=1;
		shmaddr_output[8]=1;
		sem_post(&m_output);


		return 0;

}

void initial_mode3(){	
		int i;
		mode3_count=0;
		mode3_mode=1;
		memset(lcd_string,0,sizeof(lcd_string));
		lcd_last_point=0;

		//DOT MATRIX: A
		for(i=0;i<10;i++)
				shmaddr_output[42+i]=fpga_number[0][i];

}
/***************************************** MODE 4 **************************************************/

int main_process_mode4(){
		int temp_count;
		mode4_count++;
		int i;
		int x=cursor/7;
		int y=cursor%7;
		y=6-y;
		unsigned char temp= 1<<y;

		unsigned char mode4_fpga_number2[10];

		if(input_buffer[1]==1){
				memset(mode4_fpga_number,0,sizeof(mode4_fpga_number));
				cursor=0;
		}
		else if(input_buffer[2]==1){
				cursor-=7;
				if(cursor<0)
						cursor+=70;
		}
		else if(input_buffer[3]==1){
				if(mode4_mode==1)
						mode4_mode=2;
				else
						mode4_mode=1;
		}
		else if(input_buffer[4]==1){
				cursor-=1;
				if(cursor%7==6 || cursor<0)
						cursor+=7;

		}
		else if(input_buffer[5]==1){
				mode4_fpga_number[x] = mode4_fpga_number[x] | temp;
		}
		else if(input_buffer[6]==1){
				cursor+=1;
				if(cursor%7==0)
						cursor-=7;
		}
		else if(input_buffer[7]==1){
				memset(mode4_fpga_number,0,sizeof(mode4_fpga_number));
		}
		else if(input_buffer[8]==1){
				cursor+=7;
				if(cursor/7==10)
						cursor-=70;
		}
		else if(input_buffer[9]==1){ 
				for(i=0;i<10;i++){
						temp=mode4_fpga_number[i];
						temp = ~temp;
						temp && 0x7f;
						mode4_fpga_number[i]=temp;
				}
		}
		else{
		}
		x=cursor/7;
		y=cursor%7;
		y=6-y;
		temp = 1<<y;


		sem_wait(&m_output);

		//DOT
		for(i=0;i<10;i++)
				mode4_fpga_number2[i]=mode4_fpga_number[i];
		if(mode4_mode==1)
				mode4_fpga_number2[x] = mode4_fpga_number[x] ^ temp;


		for(i=0;i<10;i++)
				shmaddr_output[42+i]=mode4_fpga_number[i];
		for(i=0;i<10;i++)
				shmaddr_output[52+i]=mode4_fpga_number2[i];


		//FND
		temp_count=mode4_count%10000;
		shmaddr_output[2]=temp_count/1000;
		temp_count%=1000;
		shmaddr_output[3]=temp_count/100;
		temp_count%=100;
		shmaddr_output[4]=temp_count/10;
		shmaddr_output[5]=temp_count%10;

		shmaddr_output[41]=1;
		shmaddr_output[1]=1;

		sem_post(&m_output);
}


void initial_mode4(){
		int i;
		memset(mode4_fpga_number,0,sizeof(mode4_fpga_number));
		cursor=0;
		mode4_mode=1;
		mode4_count=0;

		shmaddr_output[52]=0x40;

}

/*************************************** MAIN ******************************************************/



int initial_mode(){
		int i=0;
		initial=false;
		sem_wait(&m_output);
		for(i=1;i<62;i++)
				shmaddr_output[i]=0;

		switch(mode){
				case 0:
						initial_mode1();
						break;
				case 1:
						initial_mode2();
						break;
				case 2:
						initial_mode3();
						break;
				case 3: 
						initial_mode4();
						break;
				case 4:
						break;
		}
		shmaddr_output[0]=mode;
		shmaddr_output[1]=1;
		shmaddr_output[6]=1;
		shmaddr_output[8]=1;
		shmaddr_output[41]=1;

		sem_post(&m_output);
}


int check_input_to_main(){
		int i;
		int temp;
		memset(input_buffer,0,INPUT_BUFFER_SIZE);

		sem_wait(&m_input);

		//is there anything to read?
		if(shmaddr_input[0]==0 && shmaddr_input[2]==0){
				sem_post(&m_input);
				return 0;
		}

		//read read_key

		if(shmaddr_input[0]==1){
				temp=shmaddr_input[1];
				if(temp== 1){//end system
						mode=4;
						initial_mode();
						usleep(500000);
						printf("End System, Good bye\n");
						kill(pid_output,SIGKILL);
						exit(0);
				}
				else if(temp== 2){//mode++
						initial=true;
						mode+=1;
						mode%=4;
						shmaddr_output[0]=mode;
						printf("mode = %d\n",mode+1);
				}
				else if(temp== 3){//mode--
						initial=true;
						mode+=3;
						mode%=4;
						shmaddr_output[0]=mode;
						printf("mode = %d\n",mode+1);
				}
				else if(temp== 0){

				}
				else{
				}
				shmaddr_input[0]=0;
				shmaddr_input[1]=0;
		}
		if(shmaddr_input[2]==1){
				//read switch
				for(i=0;i<MAX_BUTTON;i++){
						input_buffer[i+1]=shmaddr_input[i+3];
						shmaddr_input[i+3]=0;
				}
				shmaddr_input[2]=0;
		}
		sem_post(&m_input);
		return 1;		
}

void main_process(){
		unsigned char input_buffer[INPUT_BUFFER_SIZE];
		shmaddr_input=(char*)shmat(shmid_input,(char*)NULL,0);
		shmaddr_output=(char*)shmat(shmid_output,(char*)NULL,0);


		get_curr_time();
		initial_mode();
		while(1){
				//check input shared memory
				if(check_input_to_main()==0){
						continue;
				}
				if(initial){
						initial_mode();
						continue;
				}
				switch(mode){
						case 0:
								main_process_mode1();
								break;
						case 1:
								main_process_mode2();
								break;
						case 2:
								main_process_mode3();
								break;
						case 3:
								main_process_mode4();
								break;
				}
		}


		shmdt((char*)shmaddr_input);
		shmdt((char*)shmaddr_output);
		shmctl(shmid_input,IPC_RMID,(struct shmid_ds*)NULL);
		shmctl(shmid_output,IPC_RMID,(struct shmid_ds*)NULL);
}




int main(int argc, char **argv){
		mode=0;
		//make shared memory for IPC
		shmid_input = shmget(IPC_PRIVATE,10,IPC_CREAT|0666); // parent-child IPC-> IPC_PRIVATE
		if(shmid_input == -1){
				perror("shmget");
				exit(1);
		}
		shmid_output = shmget(IPC_PRIVATE,10,IPC_CREAT|0666); 
		if(shmid_output == -1){
				perror("shmget");
				exit(1);
		}
		//make semaphore for shared memory
		sem_init(&m_input,1,1);
		sem_init(&m_output,1,1);
		switch(pid_input=fork()){
				case -1:
						perror("fork");
						exit(1);
						break;
				case 0:		
						input_process();
						break;
				default:
						switch(pid_output=fork()){
								case -1: 			
										perror("fork");
										exit(1);
										break;
								case 0:	
										output_process();
										break;
								default:				
										main_process();
						}
		}
		return 0;
}


