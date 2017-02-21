#include "test3.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include <minix/com.h>
#include <unistd.h>

//DEFINE's
#define KEYBOARD0_IRQ 		1
#define OUT_BUF 			0x60
#define STAT_REG 			0x64
#define OBF					0x01
#define PAR_ERR				0x80
#define TO_ERR				0x40
#define WAIT_KBC			20000
#define BIT(n) 				(0x01<<(n))
#define LED					0xED
#define IN_BUF_KEY				0x02

static int hookId_KBC = KEYBOARD0_IRQ;

int keyboard_subscribe_int(void) {
	int bitmask = BIT(hookId_KBC);
	if (sys_irqsetpolicy(KEYBOARD0_IRQ, (IRQ_REENABLE | IRQ_EXCLUSIVE),
			&hookId_KBC) != OK) {
		printf("WARNING!!! Error on sys_irqsetpolicy() \n");
		return -1;
	}
	if (sys_irqenable(&hookId_KBC) != OK) {
		printf("WARNING!!! Error on sys_irqenable() \n");
		return -1;
	}
	return bitmask;
}

int keyboard_unsubscribe_int() {

	if (sys_irqdisable(&hookId_KBC) != OK) {
		printf("WARNING!!! Error on sys_irqdisable() \n");
		return -1;
	}
	if (sys_irqrmpolicy(&hookId_KBC) != OK) {
		printf("WARNING!!! Error on sys_irqrmpolicy() \n");
		return -1;
	}
	return 0;
}

//Function that return a key code from keyboard
unsigned long getKeyboardCode() {
	unsigned long keyCode = 0;
	unsigned long stat;

	while (1) {
		sys_inb(STAT_REG, &stat); /* assuming it returns OK */
		/* loop while 8042 output buffer is empty */
		if (stat & OBF) {
			sys_inb(OUT_BUF, &keyCode); /* assuming it returns OK */
			if ((stat & (PAR_ERR | TO_ERR)) == 0)
				return keyCode;
			else
				return -1;
		}
		tickdelay(micros_to_ticks(WAIT_KBC));
	}
}


