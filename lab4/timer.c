#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h>
#include "timer.h"
#include "i8254.h"

//============== 6.3 ==============
static int counter = 0;
static int hookId = TIMER0_IRQ;

int timer_subscribe_int(void) {

	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hookId) != OK) {
		printf("WARNING!!! Error on sys_irqsetpolicy() \n");
		return -1;
	}
	if (sys_irqenable(&hookId) != OK) {
		printf("WARNING!!! Error on sys_irqenable() \n");
		return -1;
	}
	return 1;
}

int timer_unsubscribe_int() {

	if (sys_irqdisable(&hookId) != OK) {
		printf("WARNING!!! Error on sys_irqdisable() \n");
		return -1;
	}
	if (sys_irqrmpolicy(&hookId) != OK) {
		printf("WARNING!!! Error on sys_irqrmpolicy() \n");
		return -1;
	}
	return 0;
}

void timer_int_handler() {
	counter++;
}

int timer_test_int(unsigned long time) {
	int ipc_status, r, irq_set;
	message msg;

	irq_set = timer_subscribe_int();

	while (counter < time*60) { /* You may want to use a different condition */
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					if (counter%60 == 0) {
						printf("Counter: %d \n", (counter/60));
					}
					timer_int_handler();
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

	return 0;
}

//============== 6.2 ==============
int timer_test_square(unsigned long freq) {
	long timer = 0;
	timer_set_square(timer, freq);
	return 0;
}

int timer_set_square(unsigned long timer, unsigned long freq) {
	if (freq <= 0) {
		printf("WARNING!!! Frequency should be higher than 0 \n");
		return -1;
	}

	if (timer < 0 || timer >= 3) {
		printf("WARNING!!! Timer should be: 0, 1 or 2 \n");
		return -1;
	}

	int newFreq = TIMER_FREQ / freq;

	//Left 8 digits
	int lsb = newFreq;
	//Right 8 digits
	int msb = newFreq >> 8;

	char crb = TIMER_RB_SEL(timer) | TIMER_LSB_MSB | TIMER_SQR_WAVE | TIMER_BIN;

	if (sys_outb(TIMER_CTRL, crb)) {
		printf("WARNING!!! Error on sys_outb: crb \n");
		return -1;
	}
	if (sys_outb(TIMER_0, lsb)) {
		printf("WARNING!!! Error on sys_outb: lsb \n");
		return -1;
	}
	if (sys_outb(TIMER_0, msb)) {
		printf("WARNING!!! Error on sys_outb: msb \n");
		return -1;
	}

	return 0;
}

//============== 6.1 ==============
int timer_get_conf(unsigned long timer, unsigned char *st) {
	char crb = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_STATUS_
			| TIMER_RB_SEL(timer) | 0;

	if ((sys_outb(TIMER_CTRL, crb)) != 0) {
		printf("Error on sys_outb\n");
		return 1;
	}

	if ((sys_inb(TIMER_0 + timer, (unsigned long *) st)) != 0) {
		printf("Error on sys_inb\n");
		return 1;
	}

	return 0;
}

int timer_display_conf(unsigned char conf) {

	if (BIT(7) & conf)
		printf("Output: 1\n");
	else
		printf("Output: 0\n");

	if (BIT(6) & conf)
		printf("Null Count: 1\n");
	else
		printf("Null Count: 0\n");

	if ((BIT(5) | BIT(4)) & conf)
		printf("first LSB then MSB\n");
	else if (BIT(5) & conf)
		printf("MSB\n");
	else if (BIT(4) & conf)
		printf("LSB\n");

	if (BIT(3) | BIT(2) | BIT(1) & conf)
		printf("Mode 3: Square Wave Generator\n");
	else if (BIT(2) | BIT(1) & conf)
		printf("Mode 3: Square Wave Generator\n");
	else if (BIT(3) | BIT(1) & conf)
		printf("Mode 5\n");
	else if (BIT(3) & conf)
		printf("Mode 4\n");
	else if (BIT(1) & conf)
		printf("Mode 1\n");
	else if (BIT(3) | BIT(2) & conf)
		printf("Mode 2: Rate Generator\n");
	else if (BIT(2) & conf)
		printf("Mode 2: Rate Generator\n");
	else
		printf("Mode 0\n");

	printf("BCD\n");

	return 0;
}

int timer_test_config(unsigned long timer) {
	if (timer < 0 || timer >= 3) {
		printf("WARNING!!! Timer should be: 0, 1 or 2 \n");
		return -1;
	}
	char ptr;
	timer_get_conf(timer, &ptr);
	timer_display_conf(ptr);
	return 0;
}
