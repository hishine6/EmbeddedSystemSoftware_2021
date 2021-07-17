#include <jni.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <fcntl.h>

#include "android/log.h"
#define MAX_BUTTON 9
#define LOG_TAG "MyTag"
#define LOGV(...)   __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

jlong JNICALL Java_com_example_androidex_MultiPlayer_gettime(JNIEnv *env, jobject this){
	unsigned long n = syscall(376);
	LOGV("%lu",n);
	return n;
}


jint JNICALL Java_com_example_androidex_MultiPlayer_readjni(JNIEnv *env, jobject this)
{
	int i,j;
	int dev;
	int buff_size;

	unsigned char saved[MAX_BUTTON];


	dev = open("/dev/fpga_push_switch",O_RDWR);
	if(dev<0){
		close(dev);
		return -1;
	}
	buff_size = sizeof(saved);
	read(dev, &saved, buff_size);

	close(dev);

	if(saved[1]==1)
		return 1;
	else if(saved[7]==1)
		return 2;
	else if(saved[4]==1)
		return 3;
	else if(saved[2]==1)
		return 4;
	else
		return 0;
}


void JNICALL Java_com_example_androidex_MultiPlayer_fndjni(JNIEnv *env, jobject this, jint n){
	int i;
	int dev;
	unsigned char data[4];
	data[0] = 0;
	data[1] = n/100;
	data[2] = (n%100)/10;
	data[3] = n%10;

	dev = open("/dev/fpga_fnd",O_RDWR);
	if(dev>=0)
		write(dev,&data,4);
	close(dev);
}

void JNICALL Java_com_example_androidex_MultiPlayer_dotjni(JNIEnv *env, jobject this, jint n){

	int i;
	int dev;
	char temp =n;
	dev = open("/dev/fpga_dot",O_WRONLY);
	if(dev>=0)
		write(dev,&temp,sizeof(temp));
	close(dev);
}
