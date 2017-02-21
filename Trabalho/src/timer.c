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
