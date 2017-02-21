#include "test4.h"
#include "timer.c"
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

int printPS2Info(unsigned char* packet) {
	int lb, rb, mb, xOvfl, yOvfl, x, y;
	/*printf("Packet 0: %x \n", (packet[0]));
	 printf("Packet 1: %x \n", (packet[1]));
	 printf("Packet 2: %x \n", (packet[2]));*/

	if (packet[0] & BIT(0))
		lb = 1;
	else
		lb = 0;

	if (packet[0] & BIT(1))
		rb = 1;
	else
		rb = 0;

	if (packet[0] & BIT(2))
		mb = 1;
	else
		mb = 0;

	if (packet[0] & BIT(4))
		x = (packet[1] | (-1 << 8));
	else
		x = packet[1];

	if (packet[0] & BIT(5))
		y = (packet[2] | (-1 << 8));
	else
		y = packet[2];

	if (packet[0] & BIT(6))
		xOvfl = 1;
	else
		xOvfl = 0;

	if (packet[0] & BIT(7))
		yOvfl = 1;
	else
		yOvfl = 0;

	printf(
			"B1=0x%02x  B2=0x%02x  B3=0x%02x  LB=%d  MB=%d  RB=%d  XOV=%d  YOV=%d  X=%d  Y=%d\n",
			packet[0], packet[1], packet[2], lb, mb, rb, xOvfl, yOvfl, x, y);

	return 0;
}

int test_packet(unsigned short cnt) {
	int counter = 0;
	int ipc_status, r, irq_set;
	long status;
	message msg;
	irq_set = BIT(hookId_PS2);
	mouse_subscribe_int();
	int aux = 1;
	char packet[3];

	aux_PS2(STAT_REG, WRITE_PS2);
	aux_PS2(IN_BUF, ENABLE_STREAM_MODE);
	sys_inb(OUT_BUF, &status);

	while (counter < cnt) { /* You may want to use a different condition */
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					switch (aux) {
					case 1:
						packet[0] = getPS2Code();
						if (packet[0] & BIT(3)) {
							aux = 2;
							break;
						}
					case 2:
						packet[1] = getPS2Code();
						aux = 3;
						break;
					case 3:
						packet[2] = getPS2Code();
						printPS2Info(packet);
						counter++;
						aux = 1;
						break;
					default:
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

	aux_PS2(IN_BUF, DISABLE_STREAM_MODE);
	mouse_unsubscribe_int();
	printf("Sai do test_packet() \n");
	return 0;
}

int test_async(unsigned short idle_time) {
	int counter = 0;
	int ipc_status, r, irq_set_PS2, irq_set_TIMER;
	long status;
	message msg;

	irq_set_TIMER = timer_subscribe_int();
	irq_set_PS2 = mouse_subscribe_int();

	int aux = 1;
	char packet[3];

	aux_PS2(STAT_REG, WRITE_PS2);
	aux_PS2(IN_BUF, ENABLE_STREAM_MODE);
	sys_inb(OUT_BUF, &status);

	while (counter < idle_time * 60) { /* You may want to use a different condition */
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set_TIMER) {
					counter++;
					printf("DENTRO: %d\n", counter);
				}
				if (msg.NOTIFY_ARG & irq_set_PS2) { /* subscribed interrupt */
					switch (aux) {
					case 1:
						packet[0] = getPS2Code();
						if (packet[0] & BIT(3)) {
							aux = 2;
							break;
						}
					case 2:
						packet[1] = getPS2Code();
						aux = 3;
						break;
					case 3:
						packet[2] = getPS2Code();
						printPS2Info(packet);
						aux = 1;
						break;
					default:
						break;
					}
					counter = 0;
				}
				//printf("FORA: %d\n", counter);

				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	aux_PS2(IN_BUF, DISABLE_STREAM_MODE);
	sys_inb(OUT_BUF, &status);
	mouse_unsubscribe_int();
	timer_unsubscribe_int();

	printf("Sai do test_async() \n");
	return 0;
}

int printPS2Config(char* packet) {

	if ((packet[0] & BIT(0)))
		printf("Right button is currently pressed\n");
	else
		printf("Right button is currently released\n");

	if ((packet[0] & BIT(1)))
		printf("Middle button is currently pressed\n");
	else
		printf("Middle button is currently released\n");

	if ((packet[0] & BIT(2)))
		printf("Left button is currently pressed\n");
	else
		printf("Left button is currently released\n");

	if ((packet[0] & BIT(4)))
		printf("Scaling is 2:1\n");
	else
		printf("Scaling is 1:1\n");

	if ((packet[0] & BIT(5)))
		printf("Data reporting enabled\n");
	else
		printf("Data reporting disable\n");

	if ((packet[0] & BIT(6)))
		printf("Remote (polled) mode\n");
	else
		printf("Stream mode\n");

	if ((packet[1] & BIT(0)))
		printf("Resolution set to 2 counts per mm\n");
	else if ((packet[1] & BIT(1)))
		printf("Resolution set to 4 counts per mm\n");
	else if ((packet[1] & (BIT(0) | BIT(1))))
		printf("Resolution set to 8 counts per mm\n");
	else
		printf("Resolution set to 1 counts per mm\n");

	printf("Sample Rate: %ld\n", packet[2]);

	return 0;
}
int checkStatus() {
	unsigned long status;
	while (1) {
		aux_PS2(IN_BUF, STATUS_REQUEST);
		sys_inb(OUT_BUF, &status);
		int j;
		if (status == ACK) {
			return 0;
		} else if (status == NACK) {
			return -1;
		} else if (status == ERROR) {
			return 1;
		}
		tickdelay(micros_to_ticks(WAIT_PS2));
	}
}

int test_config(void) {
	int irq_set_PS2, ipc_status;
	long status;
	char packet[3];
	unsigned long aux;
	irq_set_PS2 = mouse_subscribe_int();
	message msg;

	aux_PS2(STAT_REG, WRITE_PS2);
	aux_PS2(IN_BUF, DISABLE_STREAM_MODE);
	int j;
	while (1) {
		j = checkStatus();
		if (!j)
			break;
		if (j == -1)
			continue;
		if (j)
			aux_PS2(STAT_REG, WRITE_PS2);
	}

	for (j = 0; j < 3; j++) {
		sys_inb(OUT_BUF, &aux);
		packet[j] = aux;
	}

	printPS2Config(packet);

	aux_PS2(IN_BUF, DISABLE_STREAM_MODE);
	mouse_unsubscribe_int();
	printf("Sai do test_config() \n");
}

typedef enum {
	INIT, DRAW, COMP
} state_t;
typedef enum {
	RDOW, RUP, MOVEUP, MOVEDOWN
} ev_type_t;

static state_t st = INIT;
static short mouseY;

void check_hor_line(ev_type_t evt, short length, short deltaY) {
	switch (st) {
	case INIT:
		mouseY = 0;
		if (evt == RDOW)
			st = DRAW;
		break;
	case DRAW:
		if (evt == MOVEUP) {
			if (length > 0) {
				if (deltaY > 0) {
					mouseY += deltaY;
					if (mouseY >= length)
						st = COMP;
				} else {
					mouseY = 0;
				}
			}
		} else if (evt == MOVEDOWN) {
			if (length < 0) {
				if (deltaY < 0) {
					mouseY -= deltaY;
					if (mouseY <= length)
						st = COMP;
				} else {
					mouseY = 0;
				}
			}
		} else if (evt == RUP)
			st = INIT;
		break;
	default:
		break;
	}
}

int test_gesture(short length) {
	int ipc_status, r, irq_set;
	long status;
	message msg;
	irq_set = mouse_subscribe_int();
	int aux = 1;
	char packet[3];
	short deltaY = 0;

	aux_PS2(STAT_REG, WRITE_PS2);
	aux_PS2(IN_BUF, ENABLE_STREAM_MODE);
	sys_inb(OUT_BUF, &status);

	while (st != COMP) { /* You may want to use a different condition */
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & irq_set) { /* subscribed interrupt */
					switch (aux) {
					case 1:
						packet[0] = getPS2Code();

						if (packet[0] & BIT(3)) {
							aux = 2;

						}

						if (packet[0] & BIT(5))
							deltaY = (packet[2] | (-1 << 8));
						else
							deltaY = packet[2];

						printf("DELTAY = %ld \n", deltaY);
						if ((packet[0] & BIT(1)) && st == INIT) {
							check_hor_line(RDOW, length, deltaY);
						} else if (!(packet[0] & BIT(1)) && st == DRAW) {
							check_hor_line(RUP, length, deltaY);
						} else if (deltaY > 0 && st == DRAW) {
							check_hor_line(MOVEUP, length, deltaY);
						} else if (deltaY < 0 && st == DRAW) {
							check_hor_line(MOVEDOWN, length, deltaY);
						}
						break;

					case 2:
						packet[1] = getPS2Code();
						aux = 3;
						break;
					case 3:
						packet[2] = getPS2Code();
						printPS2Info(packet);
						aux = 1;
						break;
					default:
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

	aux_PS2(IN_BUF, DISABLE_STREAM_MODE);
	mouse_unsubscribe_int();
	printf("Sai do test_gesture() \n");
}
