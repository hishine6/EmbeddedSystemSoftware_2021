#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/ioport.h>
#include <linux/version.h>
#include <linux/ioctl.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/param.h>

#include "kernel_timer.h"

// Read Parameter in ioctl
struct _param{
		int interval;	
		int cnt;
		int init;
};

// Used in timer 
struct struct_mydata {
		struct timer_list timer;
		int count;
		int interval;
};
struct struct_mydata mydata;


static struct file_operations kernel_timer_fops =
{ 
		.open = kernel_timer_open, 		// device file open function
		.unlocked_ioctl = kernel_timer_ioctl,	// ioctl command function
		.release = kernel_timer_release // device file close function
};

//****** Open Device ******

int kernel_timer_open(struct inode *minode, struct file *mfile) {
		printk("kernel_timer_open\n");
		if (kernel_timer_usage != 0) {return -EBUSY;}	// If device is being used
		kernel_timer_usage = 1;
		return 0;
}

//****** Close Device ******

int kernel_timer_release(struct inode *minode, struct file *mfile) {
		printk("kernel_timer_release\n");
		kernel_timer_usage = 0;	// Nobody is using Device
		return 0;
}

//****** ioctl ******

long kernel_timer_ioctl(struct file *inode,unsigned int cmd,unsigned long param){
		struct _param parameter;
		int flag;

		if(copy_from_user((void*) &parameter,(void*)param,sizeof(struct _param))){	// read parameter from User
				return -EFAULT;
		}

		flag = _IOC_NR(cmd);	// Define What Ioctl Command it is

		if(flag == SET_OPTION){
				init = parameter.init;	// Init value of Board
				cnt = parameter.cnt;	// Total iteration Count
				interval = parameter.interval;	// Interval Between Iteration
				init_state(init);	// Initialize State
				return 1;
		}
		else if(flag == START_COMMAND){
				start_command(cnt, interval);	// Start execution
				return 1;
		}
		else {
				printk("ERROR! FLAG IS NOT CORRECT!\n");
				return 0;
		}
}

//****** Write State to Device ******

void write_to_device(void){
		int i,j;
		unsigned short int tmp_led, tmp_lcd, tmp_dot, tmp_fnd;	// temp values to use when writing to device
		// LED
		tmp_led=led[Curr_Num];
		outw(tmp_led,(unsigned int)iom_fpga_led_addr);
		// DOT
		for(i=0;i<10;i++){
				tmp_dot = fpga_number[Curr_Num][i] & 0x7F;
				outw(tmp_dot,(unsigned int)iom_fpga_dot_addr+i*2);
		}
		// LCD
		for(i=0;i<2;i++){
				for(j=0;j<16;j++){
						tmp_lcd = (lcd[i][j] & 0xFF)<<8 | (lcd[i][j+1] & 0xFF);
						outw(tmp_lcd,(unsigned int)iom_fpga_text_lcd_addr+i*16+j);
						j++;
				}
		}
		// FND
		tmp_fnd = fnd[0] << 12 | fnd[1] <<8 | fnd[2] <<4 | fnd[3];	
		outw(tmp_fnd,(unsigned int)iom_fpga_fnd_addr);
}

//****** Update LCD State ******
/*
	LCD has a little more complex updating
	cases compared to other internal devices
	it needs to consider direction, max value,
	also the fact that the first line and second
	line are asynchronously moving!
   */
void update_lcd(void){
		int i;
		// Update First line start value, direction
		if(Lcd_Direction[0]==RIGHT){	// it's moving right currently
				Lcd_Start[0]++;
				if(Lcd_Start[0]>FIRST_LINE_MAX){
						Lcd_Start[0]=FIRST_LINE_MAX-1;
						Lcd_Direction[0]=LEFT;	// bounce to left
				}
		}
		else{	// it's moving left currently
				Lcd_Start[0]--;
				if(Lcd_Start[0]<0){
						Lcd_Start[0]=1;
						Lcd_Direction[0]=RIGHT;	// bounce to right
				}
		}

		// Update Second Line start value, direction(same as above)
		if(Lcd_Direction[1]==RIGHT){
				Lcd_Start[1]++;
				if(Lcd_Start[1]>SECOND_LINE_MAX){
						Lcd_Start[1]=SECOND_LINE_MAX-1;
						Lcd_Direction[1]=LEFT;
				}
		}
		else{
				Lcd_Start[1]--;
				if(Lcd_Start[1]<0){
						Lcd_Start[1]=1;
						Lcd_Direction[1]=RIGHT;
				}
		}

		// Write Updated Result to State
		for(i=0;i<16;i++){	// rewrite the result of update to each line
				if(i>=Lcd_Start[0] && i<Lcd_Start[0]+8)
						lcd[0][i]=First_Line[i-Lcd_Start[0]];
				else
						lcd[0][i]=' ';

				if(i>=Lcd_Start[1] && i<Lcd_Start[1]+13)
						lcd[1][i]=Second_Line[i-Lcd_Start[1]];
				else
						lcd[1][i]=' ';
		}
}

//****** Update State of Device ******

void update_state(void){
		int i;
		// LED, DOT
		Curr_Num = (Curr_Num)%8+1;
		// LCD
		update_lcd();
		// FND
		if(Curr_Num==Start_Num){
				Curr_Place=(Curr_Place+1)%4;
		}
		for(i=0;i<4;i++){
				if(i==Curr_Place)
						fnd[i]=Curr_Num;
				else
						fnd[i]=0;
		}

}

//****** Clear State of Device ******

void clear_device(void){
		int i,j;
		// FND
		for(i=0;i<4;i++)
				fnd[i]=0;
		// LCD
		for(i=0;i<2;i++)
				for(j=0;j<16;j++)
						lcd[i][j]=' ';

		// LED, DOT
		Curr_Num=0;
		write_to_device();
}

//****** Called Periodically ******

void kernel_timer_blink(unsigned long timeout) {
		struct struct_mydata *p_data = (struct struct_mydata*)timeout;

		if(p_data->count ==0){
				clear_device();	// If cnt is all done
				return;
		}
		printk("[RUNNING] Left cnt : %d\n", p_data->count);
		update_state();	// Update State of Device
		write_to_device();	// Write the updated state

		p_data->count--;
		mydata.timer.expires = get_jiffies_64() + (p_data->interval*HZ/10);	// expire after 'p_data->interval*HZ'
		mydata.timer.data = (unsigned long)&mydata;	// Send Data to next function
		mydata.timer.function = kernel_timer_blink;	// Set next function

		add_timer(&mydata.timer);
}

//****** Initialize State of Device ******

void init_state(int init){
		int i;

		// extracting 
		// Curr_Num, Curr_Place, Start_Num

		if(init/1000 > 0){
				Curr_Num = init/1000;
				Curr_Place = 0;
		}
		else if(init/100 >0){
				Curr_Num = init/100;
				Curr_Place = 1;	
		}
		else if(init/10 >0){
				Curr_Num = init/10;
				Curr_Place = 2;
		}
		else{
				Curr_Num = init;
				Curr_Place = 3;
		}

		// LED, DOT
		Start_Num = Curr_Num;
		// FND
		for(i=0;i<4;i++){
				if(i==Curr_Place)
						fnd[i]=Curr_Num;
				else
						fnd[i]=0;
		}
		// LCD
		Lcd_Direction[0]=RIGHT;
		Lcd_Direction[1]=RIGHT;
		Lcd_Start[0]=-1;
		Lcd_Start[1]=-1;
		update_lcd();

		write_to_device();
}

//****** Starting command ******

void start_command(int cnt,int interval){
		mydata.count = cnt;	// Set cnt	
		mydata.interval = interval;	// Set Interval

		del_timer_sync(&mydata.timer);	// Defunction existing timer

		mydata.timer.expires = jiffies + (interval)*HZ/10;	// Call next function after 'interval*HZ'
		mydata.timer.data = (unsigned long)&mydata;
		mydata.timer.function = kernel_timer_blink;

		add_timer(&mydata.timer);
}


//****** insmod ******

int __init kernel_timer_init(void)
{
		int ret;
		printk("kernel_timer_init\n");

		ret = register_chrdev(MAJOR_NUMBER, KERNEL_TIMER_NAME, &kernel_timer_fops);	// Static allocation of 'MAJOR NUMBER'
		if(ret <0) {
				printk( "error %d\n",ret);
				return ret;
		}

		// Mapping IO with Each devices
		iom_fpga_dot_addr = ioremap(IOM_FPGA_DOT_ADDRESS, 0x10);	// Dot device
		iom_fpga_text_lcd_addr = ioremap(IOM_FPGA_TEXT_LCD_ADDRESS, 0x32);	// LCD device
		iom_fpga_led_addr = ioremap(IOM_LED_ADDRESS,0x1);	// LED device
		iom_fpga_fnd_addr = ioremap(IOM_FND_ADDRESS,0x4);	// FND device

		printk( "Init Module!\n dev_file : /dev/%s , major : %d\n",KERNEL_TIMER_NAME,MAJOR_NUMBER);

		init_timer(&(mydata.timer));
		return 0;
}


//****** rmmod ******
void __exit kernel_timer_exit(void)
{
		kernel_timer_usage = 0;
		del_timer_sync(&mydata.timer);

		// Unmapping IO with Each devices
		iounmap(iom_fpga_text_lcd_addr);
		iounmap(iom_fpga_led_addr);
		iounmap(iom_fpga_dot_addr);
		iounmap(iom_fpga_fnd_addr);

		unregister_chrdev(MAJOR_NUMBER, KERNEL_TIMER_NAME);	// unregister device driver

		printk("kernel_timer_exit\n");
}

module_init(kernel_timer_init);
module_exit( kernel_timer_exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("author");
