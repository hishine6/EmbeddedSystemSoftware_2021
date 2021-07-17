#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <asm/irq.h>
#include <mach/gpio.h>
#include <linux/platform_device.h>
#include <asm/gpio.h>
#include <linux/wait.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/ioport.h>
#include <linux/version.h>
#include <linux/cdev.h>
#include <linux/workqueue.h>
#include <linux/sched.h>

#define MAJOR_NUM 242
#define DEVICE_FILE_NAME "stopwatch"
#define IOM_FND_ADDRESS 0x08000004


static int inter_usage;		// Device file Usage
static dev_t inter_dev;		
static struct cdev inter_cdev;

struct struct_mydata {
		struct timer_list timer;
};
static unsigned char* iom_fpga_fnd_addr;	// FND Device Address
struct struct_mydata stopwatch_timer;		// Stopwatch Timer
struct struct_mydata end_timer;				// End Timer
static int stopwatch_time;					// Stopwatch time
static unsigned long resume_time;			// Left time interval After Pause
static int status;							// Timer Status

// Basic file operations (fops)
static int inter_open(struct inode *, struct file *);
static int inter_release(struct inode *, struct file *);
static int inter_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);

// Interrupt handlers (Top Half)
irqreturn_t home_handler(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t back_handler(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t volup_handler(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t voldown_handler(int irq, void* dev_id, struct pt_regs* reg);

// Bottom half, using Tasklet
void short_do_tasklet(void);
DECLARE_TASKLET(short_tasklet, short_do_tasklet,0);

// Additional Functions
void kernel_timer_blink(unsigned long timeout);
void fnd_update(void);
void fnd_reset(void);
void end_stopwatch(void);

// For Sleep
wait_queue_head_t wq_write;
DECLARE_WAIT_QUEUE_HEAD(wq_write);

static struct file_operations inter_fops ={
		.open = inter_open,
		.write = inter_write,
		.release = inter_release,
};


// ------------------ Bottom Half -----------------------
/*
   - Wakes up process 
 */
void short_do_tasklet(){
		__wake_up(&wq_write,1,1,NULL);
		printk("Wake up Process\n");
}


//------------------ 'Home' Interrupt Handler -----------------------
/*
   - Starts Stopwatch
   - Deletes existing Stopwatch
   - If it was paused, next interval is remaining interval
   - Need immediate response
 */
irqreturn_t home_handler(int irq, void* dev_id, struct pt_regs* reg) {

		printk(KERN_ALERT "Pressed HOME\n");

		if(status==0){
				// Delete if timer exists
				del_timer(&stopwatch_timer.timer);
				status=1;

				// If it was paused
				if(resume_time){
						stopwatch_timer.timer.expires = get_jiffies_64() + resume_time;
						resume_time=0;
				}
				else
						stopwatch_timer.timer.expires = get_jiffies_64() + HZ;
				stopwatch_timer.timer.data = (unsigned long)&stopwatch_timer;
				stopwatch_timer.timer.function = kernel_timer_blink;

				add_timer(&stopwatch_timer.timer);
		}
		return IRQ_HANDLED;
}

//------------------ 'Back' Interrupt Handler -----------------------
/*
   - Stops Stopwatch
   - Saves remaining time of interval
   - Need immediate response
 */

irqreturn_t back_handler(int irq, void* dev_id, struct pt_regs* reg) {
		printk(KERN_ALERT "Pressed BACK\n");

		// HZ=100, meaning jiffies updates every 0.01 seconds
		if(status){
				resume_time = stopwatch_timer.timer.expires - get_jiffies_64();
				resume_time = ((int)resume_time /10)*10;
				status=0;
				// Delete timer for now
				del_timer(&stopwatch_timer.timer);
		}
		return IRQ_HANDLED;
}


//------------------ 'Volume Up' Interrupt Handler -----------------------
/*
   - Resets Stopwatch
   - Reset stopwatch_time
   - Reset FND Device to '0000'
   - Need immediate response
 */
irqreturn_t volup_handler(int irq, void* dev_id,struct pt_regs* reg) {
		printk(KERN_ALERT "Pressed VOL+\n");

		// delete timer 
		del_timer(&stopwatch_timer.timer);
		resume_time=0;

		// reset status
		fnd_reset();

		// restart timer
		if(status){
				stopwatch_timer.timer.expires = get_jiffies_64()+HZ;
				stopwatch_timer.timer.data = (unsigned long)&stopwatch_timer;
				stopwatch_timer.timer.function = kernel_timer_blink;
				add_timer(&stopwatch_timer.timer);
		}
		return IRQ_HANDLED;
}

//------------------ 'Volume Down' Interrupt Handler -----------------------
/*
   - Terminates Stopwatch
   - If Pressed, starts a new timer
   - If Released, end the new timer
 */
irqreturn_t voldown_handler(int irq, void* dev_id, struct pt_regs* reg) {
		unsigned int pressed = gpio_get_value(IMX_GPIO_NR(5,14));

		if(pressed==0){	// pressed

				printk(KERN_ALERT "Pressed VOL- \n");
				end_timer.timer.expires = get_jiffies_64() + 3*HZ;
				end_timer.timer.function = end_stopwatch;
				add_timer(&end_timer.timer);
				printk("press for 3 second!\n");
		}
		else{	// released

				printk(KERN_ALERT "Released VOL- \n");
				del_timer(&end_timer.timer);
		}
		return IRQ_HANDLED;
}

//------------------ Terminate Stopwatch -----------------------
/*
   - Called by end_stopwatch
   - Clears FND and stops timer
   - Need immediate response
   - Wake up processs can wait!!
 */
void end_stopwatch(void){	
		del_timer(&stopwatch_timer.timer);
		fnd_reset();
		printk("End Stopwatch\n");
		tasklet_schedule(&short_tasklet);	// Waking up process can wait
}

//------------------ OUTPUT to FND Device -----------------------
/*
   - Outputs to FND Device (current stopwatch time)
 */
void fnd_update(void){
		unsigned short fnd_val = 0;
		int min = (stopwatch_time/ 60) % 60;
		int sec = stopwatch_time % 60;
		fnd_val = fnd_val | ((min / 10) << 12);
		fnd_val = fnd_val | ((min % 10) << 8);
		fnd_val = fnd_val | ((sec / 10) << 4);
		fnd_val = fnd_val | ((sec % 10));
		outw(fnd_val, (unsigned int)iom_fpga_fnd_addr);
}

//------------------ Resets FND Device -----------------------
/*
   - Resets FDN Device, Stopwatch time
 */
void fnd_reset(void){
		stopwatch_time=0;
		outw(0, (unsigned int)iom_fpga_fnd_addr);
}

//------------------ Timer interrupt function -----------------------
/*
   - Called at every timer interrupt
   - Increases stopwatch_time
   - Update FND output
 */
void kernel_timer_blink(unsigned long timeout){
		stopwatch_time++;
		fnd_update();
		printk(KERN_WARNING "Tick\n");

		stopwatch_timer.timer.expires = get_jiffies_64() + HZ;
		stopwatch_timer.timer.data = (unsigned long)&stopwatch_timer;
		stopwatch_timer.timer.function = kernel_timer_blink;

		add_timer(&stopwatch_timer.timer);
}

//------------------ Write operation -----------------------
/*
   - Put's process to sleep
 */
static int inter_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos ){
		del_timer(&stopwatch_timer.timer);
		printk("sleep\n");
		interruptible_sleep_on(&wq_write);

		return 0;
}

//------------------ Open Operation -----------------------
/*
   - Called when device open
   - request_irq
 */
static int inter_open(struct inode* minode, struct file* mfile) {
		int ret;
		int irq;

		printk(KERN_ALERT "Open Module\n");
		if (inter_usage != 0) {
				return -EBUSY;
		}
		inter_usage = 1;

		gpio_direction_input(IMX_GPIO_NR(1, 11));
		irq = gpio_to_irq(IMX_GPIO_NR(1, 11));
		printk(KERN_ALERT "IRQ Number : %d\n", irq);
		ret = request_irq(irq, home_handler, IRQF_TRIGGER_FALLING, "home", 0);

		gpio_direction_input(IMX_GPIO_NR(1, 12));
		irq = gpio_to_irq(IMX_GPIO_NR(1, 12));
		printk(KERN_ALERT "IRQ Number : %d\n", irq);
		ret = request_irq(irq, back_handler, IRQF_TRIGGER_FALLING, "back", 0);

		gpio_direction_input(IMX_GPIO_NR(2, 15));
		irq = gpio_to_irq(IMX_GPIO_NR(2, 15));
		printk(KERN_ALERT "IRQ Number : %d\n", irq);
		ret = request_irq(irq, volup_handler, IRQF_TRIGGER_FALLING, "volup", 0);

		gpio_direction_input(IMX_GPIO_NR(5, 14));
		irq = gpio_to_irq(IMX_GPIO_NR(5, 14));
		printk(KERN_ALERT "IRQ Number : %d\n", irq);
		ret = request_irq(irq, voldown_handler, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "voldown", 0);

		return 0;
}

//------------------ Close Operation -----------------------
/*
   - Called when device Closed
   - free_irq
 */
static int inter_release(struct inode* minode, struct file* mfile) {
		free_irq(gpio_to_irq(IMX_GPIO_NR(1, 11)), NULL);
		free_irq(gpio_to_irq(IMX_GPIO_NR(1, 12)), NULL);
		free_irq(gpio_to_irq(IMX_GPIO_NR(2, 15)), NULL);
		free_irq(gpio_to_irq(IMX_GPIO_NR(5, 14)), NULL);
		inter_usage = 0;

		printk(KERN_ALERT "Release Module\n");
		return 0;
}

//------------------ Register Device -----------------------
static int inter_register_cdev(void)
{
		int error;

		inter_dev = MKDEV(MAJOR_NUM, 0);
		error = register_chrdev_region(inter_dev,1,"inter");

		if(error<0) {
				printk(KERN_WARNING "inter: can't get major %d\n", MAJOR_NUM);
				return -1;
		}

		printk(KERN_ALERT "dev_file: /dev/%s , major number = %d\n", DEVICE_FILE_NAME, MAJOR_NUM);

		cdev_init(&inter_cdev, &inter_fops);

		inter_cdev.owner = THIS_MODULE;
		inter_cdev.ops = &inter_fops;

		error = cdev_add(&inter_cdev, inter_dev, 1);

		if(error){
				printk(KERN_NOTICE "inter Register Error %d\n", error);
		}
		return 0;
}

//------------------ insmod -----------------------
static int __init inter_init(void) {
		int result;
		if((result = inter_register_cdev()) < 0 )
				return result;

		iom_fpga_fnd_addr = ioremap(IOM_FND_ADDRESS,0x4);   // FND device
		init_timer(&(stopwatch_timer.timer));	// Stopwatch Timer
		init_timer(&(end_timer.timer));			// End Timer

		printk(KERN_ALERT "Init Module Success \n");
		return 0;
}

//------------------ rmmod -----------------------
static void __exit inter_exit(void) {
		cdev_del(&inter_cdev);
		unregister_chrdev_region(inter_dev, 1);

		del_timer(&stopwatch_timer.timer);	// End Timer
		del_timer(&end_timer.timer);		// Stopwatch Timer
		iounmap(iom_fpga_fnd_addr);			// FND device

		printk(KERN_ALERT "Remove Module Success \n");
}

module_init(inter_init);
module_exit(inter_exit);
MODULE_LICENSE("GPL");
