#define IOM_FND_ADDRESS 0x08000004	// physical address of FND
#define IOM_FPGA_DOT_ADDRESS 0x08000210	//physical address of DOT
#define IOM_FPGA_TEXT_LCD_ADDRESS 0x08000090	//physical address of LCD
#define IOM_LED_ADDRESS 0x08000016	// physical address of LED

#define SET_OPTION 1
#define START_COMMAND 2
#define MAJOR_NUMBER 242
#define KERNEL_TIMER_NAME "dev_driver"

//----------------------- MAIN --------------------------/
static int kernel_timer_usage = 0;	// Usage Count
static unsigned char* iom_fpga_led_addr;	// address for LED
static unsigned char* iom_fpga_fnd_addr;	// address for FND
static unsigned char* iom_fpga_text_lcd_addr;	// address for LCD
static unsigned char* iom_fpga_dot_addr;	// address for DOT

int init;		// first parameter
int cnt;		// second parameter
int interval;	// third parameter

int kernel_timer_open(struct inode*, struct file *);				// Open Device File
int kernel_timer_release(struct inode*,struct file *);				// Close Device File
long kernel_timer_ioctl(struct file*, unsigned int, unsigned long);	// Do IOCTL command

void init_state(int);		// Initialize Device with parameter 'init'
void write_to_device(void);	// Write Current State to Device
void update_state(void);	// Update Current State every interval
void update_lcd(void);		// Update LCD specifically
void clear_device(void);	// Clear Device when all cnt ends

void kernel_timer_blink(unsigned long);	// Called every interval
void start_command(int, int);			// Starting to execute Command


//-------------------- TEXT LCD --------------------------/
#define RIGHT 0
#define LEFT 1
#define FIRST_LINE_MAX 8
#define SECOND_LINE_MAX 3
unsigned char lcd[2][16];
unsigned char First_Line[8]="20161566";
unsigned char Second_Line[13]="KWONHYUNGJOON";

int Lcd_Direction[2];	// Each Direction of two lines in LCD
int Lcd_Start[2];	// Each Starting point of two lines in LCd

//------------------------ LED ---------------------------/
unsigned short led[9]={0,128,64,32,16,8,4,2,1};


//------------------------ FND --------------------------/
unsigned char fnd[4];

int Start_Num;	// Initial Value
int Curr_Num;	// Current Number
int Curr_Place;	// Curretn Location

//------------------------ DOT --------------------------/
unsigned char fpga_number[10][10] = {
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // blank
	{0x0c,0x1c,0x1c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x1e}, // 1
	{0x7e,0x7f,0x03,0x03,0x3f,0x7e,0x60,0x60,0x7f,0x7f}, // 2
	{0xfe,0x7f,0x03,0x03,0x7f,0x7f,0x03,0x03,0x7f,0x7e}, // 3
	{0x66,0x66,0x66,0x66,0x66,0x66,0x7f,0x7f,0x06,0x06}, // 4
	{0x7f,0x7f,0x60,0x60,0x7e,0x7f,0x03,0x03,0x7f,0x7e}, // 5
	{0x60,0x60,0x60,0x60,0x7e,0x7f,0x63,0x63,0x7f,0x3e}, // 6
	{0x7f,0x7f,0x63,0x63,0x03,0x03,0x03,0x03,0x03,0x03}, // 7
	{0x3e,0x7f,0x63,0x63,0x7f,0x7f,0x63,0x63,0x7f,0x3e}, // 8
};
