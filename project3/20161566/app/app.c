#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define DEVICE_FILE_NAME "/dev/stopwatch"

int main(void){
		int fd;
		unsigned char data;

		fd = open(DEVICE_FILE_NAME, O_RDWR);
		if(fd < 0) {
				printf("OPEN ERROR\n");
				exit(-1);
		}

		write(fd, &data, sizeof(data));
		close(fd);

		return 0;
}

