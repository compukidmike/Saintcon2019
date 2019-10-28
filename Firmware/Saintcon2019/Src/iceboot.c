/*
 * Configure the ICE40 with new bitstreams:
 *	- first from flash at offset FLASH_ICE40_START (if one exists there)
 *	- then repeatedly from uart1 or usbcdc
 */

#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"
#include "tim.h"
#include "usbd_cdc_if.h"
#include "stm32l4xx_hal.h"
#include "errno.h"
#include "stm32l433xx.h"
#include "drv_is31fl.h"
#include "matrix.h"
#include "lampboard.h"
#include "keyboard.h"
//#include "usart.h"

#define VER "<Iceboot 0.4> "

//enum { FLASH_ICE40_START = 0x0801F000, FLASH_ICE40_END = 0x08040000 };
//enum { FLASH_ICE40_START = 0x0801F000, FLASH_ICE40_END = 0x08026DDC };
enum { FLASH_ICE40_START = 0x0801F000, FLASH_ICE40_END = 0x08026DDF };
enum { OK, TIMEOUT, ICE_ERROR };
enum { UART_FIFOSIZE = 1024 };	/* must be power of 2 */

typedef int (*Reader)(uint8_t*, uint16_t*);

#define gpio_low(pin)	HAL_GPIO_WritePin(pin##_GPIO_Port, pin##_Pin, GPIO_PIN_RESET)
#define gpio_high(pin)	HAL_GPIO_WritePin(pin##_GPIO_Port, pin##_Pin, GPIO_PIN_SET)
#define gpio_ishigh(pin)	(HAL_GPIO_ReadPin(pin##_GPIO_Port, pin##_Pin) == GPIO_PIN_SET)
#define gpio_toggle(pin)	HAL_GPIO_TogglePin(pin##_GPIO_Port, pin##_Pin)
//#define select_rpi() HAL_GPIO_WritePin(GPIOC, SPI3_MUX_S_Pin, GPIO_PIN_SET)
//#define select_leds() HAL_GPIO_WritePin(GPIOC, SPI3_MUX_S_Pin, GPIO_PIN_RESET)
//#define enable_mux_out() HAL_GPIO_WritePin(GPIOC, SPI3_MUX_OE_Pin, GPIO_PIN_RESET)
//#define disable_mux_out() HAL_GPIO_WritePin(GPIOC, SPI3_MUX_OE_Pin, GPIO_PIN_SET)
//#define status_led_high() HAL_GPIO_WritePin(GPIOC, LED5_Pin, GPIO_PIN_SET)
//#define status_led_low() HAL_GPIO_WritePin(GPIOC, LED5_Pin, GPIO_PIN_RESET)
//#define status_led_toggle() HAL_GPIO_TogglePin(GPIOC, LED5_Pin)


extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern SPI_HandleTypeDef hspi1;
extern USBD_HandleTypeDef hUsbDeviceFS;

static uint16_t crc;
static uint8_t *memp, *endmem;
static int cdc_stopped;
static int uart_detached;

volatile uint8_t spiDone = 0;
volatile uint16_t dispDelay = 1000;
uint8_t framebuffer[3075];



uint8_t buffer_rx[10];

/*
 * Dummy memory allocator for newlib, so we can call snprintf
 */
/*caddr_t
_sbrk(int incr)
{
	errno = ENOMEM;
	return (caddr_t) -1;
}*/

/*
 * Readahead fifo for input
 */
static struct fifo {
	int head, tail, max;
	uint8_t buf[UART_FIFOSIZE];
} in_fifo;

static int
fifo_put(struct fifo *f, int c)
{
	int tl;
	int count;

	tl = f->tail;
	count = tl - f->head;
	if (count < 0)
		count += UART_FIFOSIZE;
	if (count > f->max)
		f->max = count;
	if (count == UART_FIFOSIZE - 1)
		return -1;
	f->buf[tl++] = c;
	f->tail = tl & (UART_FIFOSIZE-1);
	if (count > (3*UART_FIFOSIZE)/4)
		cdc_stopped = 1;
	return OK;
}

static int
fifo_get(struct fifo *f, uint8_t *b)
{
	int hd;
	int count;

	hd = f->head;
	count = f->tail - hd;
	if (count < 0)
		count += UART_FIFOSIZE;
	if (count == 0)
		return -1;
	*b = f->buf[hd++];
	f->head = hd & (UART_FIFOSIZE-1);
	if (cdc_stopped && count < UART_FIFOSIZE/4) {
		cdc_stopped = 0;
		USBD_CDC_ReceivePacket(&hUsbDeviceFS);
	}
	return OK;
}

/*
 * Write a string to usbcdc, and to uart1 if not detached,
 * adding an extra CR if it ends with LF
 */
static void
uart_puts(char *s)
{
	char *p;

	for (p = s; *p; p++)
		;
	//if (!uart_detached)
		//HAL_USART_Transmit(&huart1, (unsigned char*)s, p - s, 500);
	CDC_Transmit_FS((unsigned char*)s, p - s);
	if (p > s && p[-1] == '\n')
		uart_puts("\r");
}

/*
 * Read one byte from input fifo, waiting until one is available
 */
static int
uart_getc(uint8_t *b)
{
	while (in_fifo.head == in_fifo.tail)
		;
	return fifo_get(&in_fifo, b);
}


/*
 * Read one byte from uart1, returning TIMEOUT immediately if one is not available
 */
static int
uart_trygetc(uint8_t *b)
{
	//if (HAL_USART_Receive(&huart1, b, 1, 0) != HAL_OK)
		//return TIMEOUT;
	return OK;
}

/*
 * Interrupt callback on uart error (probably overrun)
 */
void
HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	uart_puts("UART Error!\n");
}

/*
 * Interrupt callback when a packet has been read from usbcdc
 */
static int8_t
usbcdc_rxcallback(uint8_t *data, uint32_t *len)
{
	int i;
	int n;

	n = *len;
	for (i = 0; i < n; i++) {
		if (fifo_put(&in_fifo, *data++) < 0) {
			uart_puts("Fifo overflow!\n");
			return OK;
		}
	}
	if (!cdc_stopped)
		USBD_CDC_ReceivePacket(&hUsbDeviceFS);
	return OK;
}

/*
 * Enable reading from usbcdc in interrupt mode
 */
static void
usbcdc_startread(void)
{
	USBD_Interface_fops_FS.Receive = &usbcdc_rxcallback;
	cdc_stopped = 1;
}

/*
 * Delay for n msec
 */
static void
msec_delay(int n)
{
	HAL_Delay(n);
}

static uint8_t spibuf[64];
static int nspi;

/*
 * Write to spi3 in multiple chunks (HAL_SPI_Transmit length is limited to 16 bits)
 */
static int
spi_write(uint8_t *p, uint32_t len)
{
	int ret;
	uint16_t n;

	ret = HAL_OK;
	n = 0x8000;
	while (len > 0) {
		if (len < n)
			n = len;
		ret = HAL_SPI_Transmit(&hspi1, p, n, HAL_MAX_DELAY);
		if (ret != HAL_OK)
			return ret;
		len -= n;
		p += n;
	}
	return ret;
}

/*
 * Append one byte to output buffer for spi3.
 * If arg is negative, flush the buffer.
 */
static void
spi_write1(int b)
{
	if (b >= 0)
		spibuf[nspi++] = b;
	if ((b < 0 && nspi > 0) || nspi == sizeof spibuf) {
		spi_write(spibuf, sizeof spibuf);
		nspi = 0;
	}
}

/*
 * Tri-state spi3 pins which are shared with ice40 LED1-4 signals
 */
static void
spi_detach(void)
{
	HAL_SPI_MspDeInit(&hspi1);
	HAL_GPIO_DeInit(ICE40_SPI_CS_GPIO_Port, ICE40_SPI_CS_Pin);
}

/*
 * Reconnect the spi3 pins
 */
static void
spi_reattach(void)
{
	GPIO_InitTypeDef g;

	HAL_SPI_MspInit(&hspi1);
	g.Pin = ICE40_SPI_CS_Pin;
	g.Mode = GPIO_MODE_OUTPUT_PP;
	g.Pull = GPIO_NOPULL;
	g.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ICE40_SPI_CS_GPIO_Port, &g);
	gpio_low(ICE40_SPI_CS);
}

/*
 * Tristate the uart1 rx/tx pins
 */
static void
uart_detach(void)
{
	uart_detached = 1;
	//HAL_USART_MspDeInit(&huart1);
}

/*
 * Reset the ICE40 while holding SPI_SS_B low to force a spi-slave configuration
 */
static int
ice40_reset(void)
{
	int timeout;

	gpio_low(ICE40_CRST);
	gpio_low(ICE40_SPI_CS);
	msec_delay(1);
	gpio_high(ICE40_CRST);
	timeout = 100;
	while (gpio_ishigh(ICE40_CDONE)) {
		if (--timeout == 0)
			return TIMEOUT;
	}
	msec_delay(2);

	//spi_detach();
	gpio_high(ICE40_SPI_CS);
	GPIO_InitTypeDef g;
	g.Pin = GPIO_PIN_3;
		g.Mode = GPIO_MODE_OUTPUT_PP;
		g.Pull = GPIO_NOPULL;
		g.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOB, &g);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
								HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
								HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
									HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
									HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
										//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
										//gpio_low(ICE40_SPI_CS);
	spi_reattach();

	/*gpio_high(ICE40_SPI_CS);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
							HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
								HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
								gpio_low(ICE40_SPI_CS);*/
	return OK;
}

/*
 * Wait for end of ICE40 configuration
 */
static int
ice40_configdone(void)
{
	uint8_t b = 0;

	uart_puts(VER);
	for (int timeout = 100; !gpio_ishigh(ICE40_CDONE); timeout--) {
		if (timeout == 0) {
			uart_puts("CDONE not set\n");
			return ICE_ERROR;
		}
		spi_write(&b, 1);
	}
	for (int i = 0; i < 7; i++)
		spi_write(&b, 1);
	uart_puts("Config done\n");
	return OK;
}

/*
 * Update bitstream checksum
 */
static void
crc_update(uint8_t b)
{
	int c, v;

	v = b << 8;
	c = crc;
	for (int i = 0; i < 8; i++) {
		int x = 0;
		if ((c ^ v) & 0x8000)
			x = 0x1021;
		c = (c << 1) ^ x;
		v <<= 1;
	}
	crc = c;
}

/*
 * Restart bitstream checksum
 */
static void
crc_reset(void)
{
	crc = 0xFFFF;
}

/*
 * Read a byte from uart, update checksum, and send it to spi3
 */
static int
rbyte_uart_send(uint8_t *b, uint16_t *crc)
{
	if (b == NULL) {
		spi_write1(-1);
		return OK;
	}
	if (uart_getc(b) != OK)
		return -1;
	spi_write1(*b);
	crc_update(*b);
	return OK;
}

/*
 * Read a byte from flash or RAM, and update checksum
 */
static int
rbyte_mem_check(uint8_t *b, uint16_t *crc)
{
	if (memp >= endmem)
		return -1;
	if (b == NULL)
		return OK;
	*b = *memp++;
	crc_update(*b);
	return OK;
}

/*
 * Read a byte from flash or RAM, update checksum, and send it to spi3
 */
static int
rbyte_mem_send(uint8_t *b, uint16_t *crc)
{
	if (memp >= endmem)
		return -1;
	if (b == NULL) {
		spi_write1(-1);
		return OK;
	}
	*b = *memp++;
	spi_write(b, 1);
	crc_update(*b);
	return OK;
}

uint8_t write_fpga()
{
	uint8_t *b;
	while(memp <= endmem){
	//if (memp >= endmem)
	//	return -1;
	//if (b == NULL) {
	//	spi_write1(-1);
	//	return OK;
	//}
	*b = *memp++;
	spi_write(b, 1);
	//crc_update(*b);
	//return OK;
	}
	return OK;
}

/*
 * Read n bytes using the given reader
 */
static int
rbytes(Reader rbyte, int n, uint8_t *b, uint16_t *crc)
{
	while (n-- > 0) {
		if (rbyte(b, crc) < 0)
			return -1;
	}
	return 0;
}

/*
 * Read and parse a bitstream using the given reader
 */
static int
rbits(Reader rbyte, int firstb)
{
	int preamble;
	int crc_checked = 0;
	uint8_t b;
	int cmd, len, arg;
	int width = 0, height = 0;

	/* find the synchronising marker */
	preamble = firstb;
	while (preamble != 0x7EAA997E) {
		if (rbyte(&b, &crc) < 0)
			return -1;
		preamble <<= 8;
		preamble |= b;
	}

	/* parse the bitstream to find crc reset+check commands */
	while (rbyte(&b, &crc) == 0) {
		cmd = b >> 4;
		len = b & 0xF;
		arg = 0;
		while (len-- > 0) {
			if (rbyte(&b, &crc) < 0)
				return -1;
			arg <<= 8;
			arg |= b;
		}
		switch (cmd) {
		default:	/* unknown */
			return -1;
		case 1:		/* current bank */
		case 5:		/* frequency range */
		case 8:		/* offset of section */
		case 9:		/* warm boot */
			break;
		case 2:		/* check crc */
			if (crc != 0) {
				uart_puts("CRC error..");
				return -1;
			}
			break;
		case 6:		/* width of section */
			width = arg + 1;
			break;
		case 7:		/* height of section */
			height = arg;
			break;
		case 0:
			switch (arg) {
			default:	/* unknown */
				return -1;
			case 1:		/* CRAM data */
			case 3:		/* BRAM data */
				if (rbytes(rbyte, height*width/8, &b, &crc) < 0)
					return -1;
				if (rbyte(&b, &crc) < 0 || b != 0)
					return -1;
				if (rbyte(&b, &crc) < 0 || b != 0)
					return -1;
				break;
			case 5:		/* crc reset */
				crc_reset();
				break;
			case 6:		/* wakeup */
				if (!crc_checked)
					return -1;
				/* discard the final padding byte added by icepack */
				rbyte(&b, &crc);
				rbyte(NULL, NULL);
				return OK;
			}
		}
		crc_checked = (cmd == 2);
	}
	return -1;
}


/*
 * Loop function (called repeatedly)
 *	- wait for the start of a bitstream to be received on uart1 or usbcdc
 *	- receive the bitstream and pass it to the ice40
 */
void
loop(void)
{
	/*
	uint8_t b = 0;
	static int err = 0;

	if (err) {
		//status_led_toggle();
		HAL_Delay(100);
		return;
	}
	uart_puts("Waiting for USB serial\n");
	do {
		if (cdc_stopped && USBD_CDC_ReceivePacket(&hUsbDeviceFS) == OK)
			cdc_stopped = 0;
		fifo_get(&in_fifo, &b);
	} while (b != 0x7E);
	//status_led_high();
	//disable_mux_out();
	spi_reattach();
	err = ice40_reset();
	if (err)
		return;
	crc_reset();
	spi_write(&b, 1);
	crc_update(b);
	if ((err = rbits(rbyte_uart_send, b)) != OK) {
		uart_puts("rbits failed\n");
		return;
	}
	err = ice40_configdone();
	spi_detach();
	//enable_mux_out();
	//status_led_low();

	 */

	spi_reattach();
	spi_write1(1);
	spi_write1(0);
	spi_write1(0);

	while(1){
		spi_write1(128);
		msec_delay(100);

	}

}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
 if(huart->Instance==USART1)
 {
 while(1);
 }

if(huart->Instance==USART2)
 {
	char *adata = "A";
	 if(*huart->pRxBuffPtr == *adata){
		 dispDelay = 500;
	 }
 }
}

void FPGA_Init(){
	spi_detach();
		//status_led_low();
		memp = (uint8_t*)FLASH_ICE40_START;
		endmem = (uint8_t*)FLASH_ICE40_END;
		uart_puts(VER);
		uart_puts("\n");
		crc_reset();
		if (rbits(rbyte_mem_check, 0) == OK) {
			uart_puts("Programming from flash\n");
			memp = (uint8_t*)FLASH_ICE40_START;
			crc_reset();
			//status_led_high();
			spi_reattach();
			if (ice40_reset() != OK)
				uart_puts("reset failed\n");
			else {
				//spi_reattach();
				if (rbits(rbyte_mem_send, 0) != OK)
				//if(write_fpga() != OK)
				uart_puts("rbits failed\n");
			else if (ice40_configdone() != OK)
				uart_puts("configdone failed\n");
			}
			spi_detach();
			//status_led_low();
		}
		gpio_high(ICE40_SPI_CS);
		msec_delay(100);
		//uart_puts("Setup done\n");
		//while (uart_trygetc(&b) == OK)
		//	;
		//usbcdc_startread();
		//select_leds();
		//enable_mux_out();
		//uart_detach();

		spi_reattach();
}

/*
 * Setup function (called once at powerup)
 *	- flush any input in uart buffer
 *	- if there's a bitstream in flash, send it to the ice40
 */
void
setup(void)
{
	HAL_Init();
	SystemClock_Config();

	MX_GPIO_Init();
	  MX_I2C1_Init();
	  MX_I2C2_Init();
	  MX_SPI1_Init();
	  MX_SPI2_Init();
	  MX_USART1_UART_Init();
	  MX_USART2_UART_Init();
	  MX_USB_DEVICE_Init();
	  MX_TIM16_Init();
	  HAL_TIM_Base_Start_IT(&htim16);

	//uint8_t b;
	//disable_mux_out();

	spi_detach();
	//status_led_low();
	memp = (uint8_t*)FLASH_ICE40_START;
	endmem = (uint8_t*)FLASH_ICE40_END;
	uart_puts(VER);
	uart_puts("\n");
	crc_reset();
	if (rbits(rbyte_mem_check, 0) == OK) {
		uart_puts("Programming from flash\n");
		memp = (uint8_t*)FLASH_ICE40_START;
		crc_reset();
		//status_led_high();
		spi_reattach();
		if (ice40_reset() != OK)
			uart_puts("reset failed\n");
		else {
			//spi_reattach();
			if (rbits(rbyte_mem_send, 0) != OK)
			//if(write_fpga() != OK)
			uart_puts("rbits failed\n");
		else if (ice40_configdone() != OK)
			uart_puts("configdone failed\n");
		}
		spi_detach();
		//status_led_low();
	}
	gpio_high(ICE40_SPI_CS);
	msec_delay(100);
	//uart_puts("Setup done\n");
	//while (uart_trygetc(&b) == OK)
	//	;
	//usbcdc_startread();
	//select_leds();
	//enable_mux_out();
	//uart_detach();

	spi_reattach();
	/*
	 * gpio_high(ICE40_SPI_CS);
	msec_delay(100);

	gpio_low(ICE40_SPI_CS);
	uint8_t data = 1;
		spi_write(&data,1);
		data = 0;
		spi_write(&data,1);
		spi_write(&data,1);
		*/
	uint8_t data = 1;


		  //MX_SPI1_Init();
		  //HAL_SPI_MspInit(&hspi1);

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1,GPIO_PIN_SET); //Enable LED Driver

	  drv_is31fl_init();

	  //uint8_t buffer[10];

	  drv_is31fl_set_page(1);

	  //while(1);

	  while(1){
		matrix_drawRect(0,0,64,16,COLOR_RED);
		matrix_drawRect(1,1,62,14,COLOR_GREEN);
		matrix_drawRect(2,2,60,12,COLOR_BLUE);
		matrix_drawRect(3,3,58,10,COLOR_WHITE);
		matrix_drawRect(4,4,56,8,COLOR_RED);
		matrix_drawRect(5,5,54,6,COLOR_GREEN);
		matrix_drawRect(6,6,52,4,COLOR_BLUE);
		matrix_drawRect(7,7,50,2,COLOR_WHITE);
		matrix_update();
		while(1){
			uint32_t buttons = 0;
			buttons = keyboardUpdate();
			for(int x=0; x<26; x++){
				if((buttons>>x) & 1){
					lampboard_setLamp(x, COLOR_BLUE);
				} else {
					lampboard_setLamp(x, COLOR_GREEN);
				}
			}
			if((buttons>>31)&1){
				for(int x=0; x<26; x++){
					lampboard_setLamp(x, COLOR_RED);
				}
			}
			lampboard_update();
			msec_delay(50);
		}
		msec_delay(dispDelay);
		  for(int x=0; x<64; x++){
			  for(int y = 0; y<16; y++){
				  matrix_drawPixel(x,y,(RGB){128,0,0});
			  }
		  }
		  matrix_update();
		  msec_delay(dispDelay);
		  for(int x=0; x<64; x++){
		  			  for(int y = 0; y<16; y++){
		  				matrix_drawPixel(x,y,(RGB){0,128,0});
		  			  }
		  		  }
		  matrix_update();
		  msec_delay(dispDelay);
		  for(int x=0; x<64; x++){
		  			  for(int y = 0; y<16; y++){
		  				matrix_drawPixel(x,y,(RGB){0,0,128});
		  			  }
		  		  }
		  matrix_update();
		  msec_delay(dispDelay);
		  for(int x=0; x<64; x++){
		  			  for(int y = 0; y<16; y++){
		  				matrix_drawPixel(x,y,(RGB){128,128,128});
		  			  }
		  		  }
		  matrix_update();
		  msec_delay(dispDelay);
	  }

	while(1){
		/*if(gpio_ishigh(TEST)){ //BOOT0 pin gpio test
			dispDelay = 500;
		} else {
			dispDelay = 1000;
		}*/

		/*gpio_low(ICE40_SPI_CS);
		uint8_t data = 1;
		spi_write(&data,1);
		data = 0;
		spi_write(&data,1);
		spi_write(&data,1);
		data = 128;
		spi_write(&data,1);
		//msec_delay(100);
		data = 0;
		spi_write(&data,1);
		data = 0;
		spi_write(&data,1);
		gpio_high(ICE40_SPI_CS);
		msec_delay(1000);

		gpio_low(ICE40_SPI_CS);
		data = 1;
		spi_write(&data,1);
		data = 0;
		spi_write(&data,1);
		spi_write(&data,1);
		data = 0;
		spi_write(&data,1);
		//msec_delay(100);
		data = 128;
		spi_write(&data,1);
		data = 0;
		spi_write(&data,1);
		gpio_high(ICE40_SPI_CS);
		msec_delay(1000);

		gpio_low(ICE40_SPI_CS);
		data = 1;
		spi_write(&data,1);
		data = 0;
		spi_write(&data,1);
		spi_write(&data,1);
		data = 0;
		spi_write(&data,1);
		//msec_delay(100);
		data = 0;
		spi_write(&data,1);
		data = 128;
		spi_write(&data,1);
		gpio_high(ICE40_SPI_CS);
		msec_delay(1000);*/
		//loop();

		/*gpio_low(ICE40_SPI_CS);
		data = 1;
		spi_write(&data,1);
		data = 0;
		spi_write(&data,1);
		spi_write(&data,1);

		for(int x=0; x<256; x++){
			data = gamma8[x];
			spi_write(&data,1);
			data = 0;
			spi_write(&data,1);
			spi_write(&data,1);
		}
		for(int x=0; x<256; x++){
			data = 0;
			spi_write(&data,1);
			data = gamma8[x];
			spi_write(&data,1);
			data = 0;
			spi_write(&data,1);
		}
		for(int x=0; x<256; x++){
			data = 0;
			spi_write(&data,1);
			data = 0;
			spi_write(&data,1);
			data = gamma8[x];
			spi_write(&data,1);
		}
		for(int x=0; x<256; x++){
			data = gamma8[x];
			spi_write(&data,1);
			spi_write(&data,1);
			spi_write(&data,1);
		}
		gpio_high(ICE40_SPI_CS);

		msec_delay(dispDelay);*/

/*
		framebuffer[0] = 1;
		framebuffer[1] = 0;
		framebuffer[2] = 0;
		for(int x=3; x<3075; x+=3){
			framebuffer[x] = 127;
			framebuffer[x+1] = 127;
			framebuffer[x+2] = 127;
		}
		//gpio_low(ICE40_SPI_CS);
		spiDone = 0;
		if(HAL_SPI_Transmit_DMA(&hspi1, framebuffer, 100) != HAL_OK){
			while(1);
		}
		while(spiDone == 0);
		//gpio_high(ICE40_SPI_CS);

		framebuffer[0] = 1;
		framebuffer[1] = 0;
		framebuffer[2] = 0;
		for(int x=3; x<3075; x+=3){
			framebuffer[x] = 127;
			framebuffer[x+1] = 0;
			framebuffer[x+2] = 0;
		}
		gpio_low(ICE40_SPI_CS);
		HAL_SPI_Transmit_DMA(&hspi1, framebuffer, 100);
		gpio_high(ICE40_SPI_CS);

		framebuffer[0] = 1;
		framebuffer[1] = 0;
		framebuffer[2] = 0;
		for(int x=3; x<3075; x+=3){
			framebuffer[x] = 0;
			framebuffer[x+1] = 127;
			framebuffer[x+2] = 0;
		}
		gpio_low(ICE40_SPI_CS);
		HAL_SPI_Transmit_DMA(&hspi1, framebuffer, 3075);
		gpio_high(ICE40_SPI_CS);

		framebuffer[0] = 1;
		framebuffer[1] = 0;
		framebuffer[2] = 0;
		for(int x=3; x<3075; x+=3){
			framebuffer[x] = 0;
			framebuffer[x+1] = 0;
			framebuffer[x+2] = 127;
		}
		gpio_low(ICE40_SPI_CS);
		HAL_SPI_Transmit_DMA(&hspi1, framebuffer, 3075);
		gpio_high(ICE40_SPI_CS);
*/

		//Fill RED
		gpio_low(ICE40_SPI_CS);
		data = 1;
		spi_write(&data,1);
		data = 0;
		spi_write(&data,1);
		spi_write(&data,1);

		for(int x=0; x<1024; x++){
			data = 128;
			spi_write(&data,1);
			data = 0;
			spi_write(&data,1);
			spi_write(&data,1);
		}
		gpio_high(ICE40_SPI_CS);

		for(int x = 0; x<200; x++){
			  			  drv_is31fl_send_value(x,0);
			  		  }
			  	for(int x = 32; x<48; x+=2){
			  			  drv_is31fl_send_value(x,128);
			  		  }
			  		  for(int x = 80; x<96; x+=2){
			  		  		  drv_is31fl_send_value(x,128);
			  		  	  }
			  		  for(int x = 128; x<144; x+=2){
			  		  		  drv_is31fl_send_value(x,128);
			  		  	  }
			  		for(int x = 176; x<180;x+=2){
			  			  	  		  drv_is31fl_send_value(x,128);
			  			  	  	  }

		msec_delay(dispDelay);

		//Fill BLUE
		gpio_low(ICE40_SPI_CS);
		data = 1;
		spi_write(&data,1);
		data = 0;
		spi_write(&data,1);
		spi_write(&data,1);

		for(int x=0; x<1024; x++){
			data = 0;
			spi_write(&data,1);
			data = 127;
			spi_write(&data,1);
			data = 0;
			spi_write(&data,1);
		}
		gpio_high(ICE40_SPI_CS);

		for(int x = 0; x<200; x++){
						  drv_is31fl_send_value(x,0);
					  }
		for(int x = 0; x<16; x+=2){
				  drv_is31fl_send_value(x,128);
			  }
			  for(int x = 48; x<64; x+=2){
			  		  drv_is31fl_send_value(x,128);
			  	  }
			  for(int x = 96; x<112; x+=2){
			  		  drv_is31fl_send_value(x,128);
			  	  }
			  for(int x = 144; x<148; x+=2){
			  	  		  drv_is31fl_send_value(x,128);
			  	  	  }


		msec_delay(dispDelay);

		//Fill GREEN
		gpio_low(ICE40_SPI_CS);
		data = 1;
		spi_write(&data,1);
		data = 0;
		spi_write(&data,1);
		spi_write(&data,1);

		for(int x=0; x<1024; x++){
			data = 0;
			spi_write(&data,1);
			data = 0;
			spi_write(&data,1);
			data = 127;
			spi_write(&data,1);
		}
		gpio_high(ICE40_SPI_CS);

		for(int x = 0; x<200; x++){
				  drv_is31fl_send_value(x,0);
			  }
			  for(int x = 16; x<32; x+=2){
			  		  drv_is31fl_send_value(x,128);
			  	  }
			  	  for(int x = 64; x<80; x+=2){
			  	  		  drv_is31fl_send_value(x,128);
			  	  	  }
			  	  for(int x = 112; x<128; x+=2){
			  	  		  drv_is31fl_send_value(x,128);
			  	  	  }
			  	for(int x = 160; x<164; x+=2){
			  		  	  		  drv_is31fl_send_value(x,128);;
			  		  	  	  }

		msec_delay(dispDelay);

		//Fill WHITE
		gpio_low(ICE40_SPI_CS);
		data = 1;
		spi_write(&data,1);
		data = 0;
		spi_write(&data,1);
		spi_write(&data,1);

		for(int x=0; x<1024; x++){
			data = 127;
			spi_write(&data,1);
			spi_write(&data,1);
			spi_write(&data,1);
		}
		gpio_high(ICE40_SPI_CS);

		for(int x = 0; x<200; x++){
								  drv_is31fl_send_value(x,128);
							  }


		msec_delay(dispDelay);

		for(int x = 0; x<200; x++){
						  drv_is31fl_send_value(x,0);
					  }

		matrix_clear();
/*
		char *rdata = " ";
		char *adata = "A";
		uint8_t data = 1;
		while(data){
		//HAL_USART_Receive(&husart2, (unsigned char *)rdata, 1, 500);
			HAL_UART_Receive_IT(&huart2, buffer_rx, 10);

		//if(*rdata == *adata){ //A
		if(buffer_rx[0] == *adata){ //A
			dispDelay = 500;
			data = 0;
		}
		}*/
	}
}



