#include "test4.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include <minix/com.h>
#include <unistd.h>
#include <strings.h>

//DEFINE's
#define PS2_IRQ 			12
#define OUT_BUF				0x60
#define IN_BUF				0x60
#define OBF					0x01
#define IBF					0x02
#define STAT_REG 			0x64
#define PAR_ERR				0x80
#define TO_ERR				0x40
#define WAIT_PS2			20000
#define BIT(n) 				(0x01<<(n))
#define STREAM_MODE			0xEA
#define ENABLE_STREAM_MODE	0xF4
#define DISABLE_STREAM_MODE	0xF5
#define ACK					0xFA
#define NACK				0xFE
#define ERROR				0xFC
#define WRITE_PS2			0xD4
#define STATUS_REQUEST		0xE9

static int hookId_PS2 = PS2_IRQ;

int mouse_subscribe_int(void) {
	int bitmask = BIT(hookId_PS2);

	if (sys_irqsetpolicy(PS2_IRQ, (IRQ_REENABLE | IRQ_EXCLUSIVE), &hookId_PS2)
			!= OK) {
		printf("WARNING!!! Error on sys_irqsetpolicy() \n");
		return -1;
	}
	if (sys_irqenable(&hookId_PS2) != OK) {
		printf("WARNING!!! Error on sys_irqenable() \n");
		return -1;
	}

	return bitmask;
}

int mouse_unsubscribe_int() {

	if (sys_irqdisable(&hookId_PS2) != OK) {
		printf("WARNING!!! Error on sys_irqdisable() \n");
		return -1;
	}
	if (sys_irqrmpolicy(&hookId_PS2) != OK) {
		printf("WARNING!!! Error on sys_irqrmpolicy() \n");
		return -1;
	}
	return 0;
}

char getPS2Code() {
	unsigned long keyCode = 0;
	unsigned long stat;

	while (1) {
		sys_inb(STAT_REG, &stat); /* assuming it returns OK */
		/* loop while 8042 output buffer is empty */
		if (stat & OBF) {
			sys_inb(OUT_BUF, &keyCode); /* assuming it returns OK */
			if ((stat & (PAR_ERR | TO_ERR)) == 0)
				return (unsigned char) keyCode;
			else
				return -1;
		}
		tickdelay(micros_to_ticks(WAIT_PS2));
	}
}

int aux_PS2(unsigned long command, unsigned char aux) {
	unsigned long stat;

	while (1) {
		sys_inb(STAT_REG, &stat); /* assuming it returns OK */
		/* loop while 8042 output buffer is empty */
		if ((stat & IBF) == 0) {
			sys_outb(command, aux); /* assuming it returns OK */
			if ((stat & (PAR_ERR | TO_ERR)) == 0)
				return 0;
			else
				return -1;
		}
		tickdelay(micros_to_ticks(WAIT_PS2));
	}
}

