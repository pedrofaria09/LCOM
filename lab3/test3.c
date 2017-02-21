#include "test3.h"
#include "timer.c"
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
#define IN_BUF				0x02

static int hookId_KBC = KEYBOARD0_IRQ;

int keyboard_subscribe_int(void) {
	int bitmask = BIT(hookId);
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

int kbd_test_scan(unsigned short ass) {

	unsigned long keyboardcode = 0;
	int quit = 0;
	int ipc_status, r, irq_set;
	message msg;
	irq_set = keyboard_subscribe_int();
	int verifica = 0;

	if (ass == 0) {
		while (!quit) { /* You may want to use a different condition */
			/* Get a request message. */
			quit = 0;
			if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
				printf("driver_receive failed with: %d", r);
				continue;
			}
			if (is_ipc_notify(ipc_status)) { /* received notification */

				switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: /* hardware interrupt notification */
					if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
						//Verify errors
						if (irq_set == -1) {
							return -1;
						}

						//Get the keyboard code
						keyboardcode = getKeyboardCode();

						//If keyboard code is exit key, will exit
						if (keyboardcode == 0x81) {
							quit = 1;
							break;
						}
						verifica = (keyboardcode >> 7);
						//Verify if is make or break code
						if (verifica == 1) {
							printf("BreakCode: 0x%02X \n", keyboardcode);
						} else {
							printf("MakeCode: 0x%02X \n", keyboardcode);
						}
					}
					break;
				default:
					break; /* no other notifications expected: do nothing */
				}
			} else { /* received a standard message, not a notification */
				/* no standard messages expected: do nothing */
			}
		}
		keyboard_unsubscribe_int();
		return 0;
	}

}

int aux_KB_leds(unsigned char aux) {
	unsigned long stat;

	while (1) {
		sys_inb(STAT_REG, &stat); /* assuming it returns OK */
		/* loop while 8042 output buffer is empty */
		if ((stat & IN_BUF) == 0) {
			sys_outb(OUT_BUF, aux); /* assuming it returns OK */
			if ((stat & (PAR_ERR | TO_ERR)) == 0)
				return 0;
			else
				return -1;
		}
		tickdelay(micros_to_ticks(WAIT_KBC));
	}
}

int kbd_test_leds(unsigned short n, unsigned short *leds) {
	unsigned int j;
	unsigned char aux;
	unsigned long stat;
	unsigned int led0 = 0;
	unsigned int led1 = 0;
	unsigned int led2 = 0;
	int counter = 0;
	int ipc_status, r, irq_set;
	message msg;

	irq_set = timer_subscribe_int();

	for (j = 0; j < n; j++) {
		aux = LED;
		aux_KB_leds(aux);
		sys_inb(OUT_BUF, &stat);

		if (leds[j] == 0) {
			if (led0 == 0) {
				led0 = BIT(0);
				printf("Scroll lock On.\n");
			} else {
				led0 = 0;
				printf("Scroll lock Off.\n");
			}
		}
		if (leds[j] == 1) {
			if (led1 == 0) {
				led1 = BIT(1);
				printf("Numeric lock On.\n");
			} else {
				led1 = 0;
				printf("Numeric lock Off.\n");
			}
		}
		if (leds[j] == 2) {
			if (led2 == 0) {
				led2 = BIT(2);
				printf("Caps lock On.\n");
			} else {
				led2 = 0;
				printf("Caps lock Off.\n");
			}
		}

		aux = (led0 | led1 | led2);

		aux_KB_leds(aux);
		sys_inb(OUT_BUF, &stat);

		counter = 0;
		while (counter < 60) { /* You may want to use a different condition */
			/* Get a request message. */
			if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
				printf("driver_receive failed with: %d", r);
				continue;
			}
			if (is_ipc_notify(ipc_status)) { /* received notification */
				switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: /* hardware interrupt notification */
					if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
						counter++;
					}
					break;
				default:
					break; /* no other notifications expected: do nothing */
				}
			} else { /* received a standard message, not a notification */
				/* no standard messages expected: do nothing */
			}
		}

	}
	timer_unsubscribe_int();

}

int kbd_test_timed_scan(unsigned short n) {
	unsigned long keyboardcode = 0;
	int quit = 0;
	int ipc_status, r, irq_set_KBR, irq_set_TMR;
	message msg;

	irq_set_TMR = timer_subscribe_int();

	irq_set_KBR = keyboard_subscribe_int();

	int verifica = 0;
	int counter = 0;

	while (!quit) { /* You may want to use a different condition */
		/* Get a request message. */
		quit = 0;
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */

			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set_KBR) { /* subscribed interrupt */
					//Verify errors
					if (irq_set_KBR == -1) {
						return -1;
					}

					//Get the keyboard code
					keyboardcode = getKeyboardCode();

					//If keyboard code is exit key, will exit
					if (keyboardcode == 0x81) {
						quit = 1;
						break;
					}
					verifica = (keyboardcode >> 7);
					//Verify if is make or break code
					if (verifica == 1) {
						printf("BreakCode: 0x%02X \n", keyboardcode);
					} else {
						printf("MakeCode: 0x%02X \n", keyboardcode);
					}
					counter = 0;
				}
				if (msg.NOTIFY_ARG & irq_set_TMR) {
					counter++;
					if (counter >= (n*60)) {
						quit = 1;
						break;
					}
				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}

		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	timer_unsubscribe_int();
	keyboard_unsubscribe_int();
	printf("Exiting kbd_test_timed_scan() \n");
	return 0;
}
