#include <minix/drivers.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/com.h>
#include "video_gr.c"
#include "timer.c"
#include "test3.c"
#include "read_xpm.c"

#define MODE_GRAPH 		0x105

void *test_init(unsigned short mode, unsigned short delay) {

	vg_init(mode);
	timer_test_int(delay);
	vg_exit();

}

int test_square(unsigned short x, unsigned short y, unsigned short size,
		unsigned long color) {
	int i, j;

	vg_init(MODE_GRAPH);

	for (i = x; i < (size + x); i++) {
		for (j = y; j < (size + y); j++) {
			video_mem[(i + j * H_RES)] = color;
		}
	}

	//test();
	/*
	 FILE *fp;
	 fp = fopen("/home/lcom/myfile.txt", "w+");
	 for (i = 0; i < H_RES*V_RES; i++) {
	 fprintf(fp, "%d\n", video_mem[i]);
	 }

	 fclose(fp);
	 */
	unsigned long keyboardcode = 0;
	int quit = 0;
	int ipc_status, r, irq_set;
	message msg;
	irq_set = keyboard_subscribe_int();

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
	vg_exit();

}

int test_line(unsigned short xi, unsigned short yi, unsigned short xf,
		unsigned short yf, unsigned long color) {

	vg_init(MODE_GRAPH);

	int dx = abs(xf - xi);
	int sx = xi < xf ? 1 : -1;
	int dy = abs(yf - yi);
	int sy = yi < yf ? 1 : -1;
	int err = (dx > dy ? dx : -dy) / 2;
	int e2;

	while (1) {
		video_mem[(xi + yi * H_RES)] = color;
		if (xi == xf && yi == yf)
			break;
		e2 = err;
		if (e2 > -dx) {
			err -= dy;
			xi += sx;
		}
		if (e2 < dy) {
			err += dx;
			yi += sy;
		}
	}

	unsigned long keyboardcode = 0;
	int quit = 0;
	int ipc_status, r, irq_set;
	message msg;
	irq_set = keyboard_subscribe_int();

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
	vg_exit();
}

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[]) {
//	int i;
//	for(i=0;i<sizeof(xpm);i++){
//		printf("%s",xpm[i]);
//
//	}
//	printf("\n");
//	printf("%s\n",xpm);

	int i = 0, j = 0, a = 0;
	char *rd_xpm;
	int width, height;
	vg_init(MODE_GRAPH);

	rd_xpm = read_xpm(xpm, &width, &height);

	for (i = xi; i < height + xi; i++) {
		for (j = yi; j < width + yi; j++) {
			video_mem[(j + i * H_RES)] = rd_xpm[a];
			a++;
		}
	}

	unsigned long keyboardcode = 0;
	int quit = 0;
	int ipc_status, r, irq_set;
	message msg;
	irq_set = keyboard_subscribe_int();

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
	vg_exit();

}

int test_move(unsigned short xi, unsigned short yi, char *xpm[],
		unsigned short hor, short delta, unsigned short time) {

	int i = 0, j = 0, a = 0;
	char *rd_xpm;
	int width, height;
	vg_init(MODE_GRAPH);

	rd_xpm = read_xpm(xpm, &width, &height);
	for (i = xi; i < height + xi; i++) {
		for (j = yi; j < width + yi; j++) {
			video_mem[(j + i * H_RES)] = rd_xpm[a];
			a++;
		}
	}
	unsigned long keyboardcode = 0;
	int quit = 0;
	int ipc_status, r, kbcirq_set, timerirq_set, counter = 0;
	message msg;
	timerirq_set = BIT(hookId);
	timer_subscribe_int();
	kbcirq_set = keyboard_subscribe_int();

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
				if (msg.NOTIFY_ARG & kbcirq_set) { /* subscribed interrupt */
					//Verify errors
					if (kbcirq_set == -1) {
						return -1;
					}

					//Get the keyboard code
					keyboardcode = getKeyboardCode();

					//If keyboard code is exit key, will exit
					if (keyboardcode == 0x81) {
						quit = 1;
						break;
					}
				}
				if (msg.NOTIFY_ARG & timerirq_set) { /* subscribed interrupt */
					counter++;
					if (counter < (time * 60)) {
						a = 0;
						test();
						if (!hor) {
							xi += (int) delta / (int) (time * 60);
							for (i = xi; i < height + xi; i++) {
								for (j = yi; j < width + yi; j++) {
									video_mem[(j + i * H_RES)] = rd_xpm[a];
									a++;
								}
							}

						} else {
							yi += (int) delta / (int) (time * 60);
							for (i = xi; i < height + xi; i++) {
								for (j = yi; j < width + yi; j++) {
									video_mem[(j + i * H_RES)] = rd_xpm[a];
									a++;
								}
							}
						}
					}
					if (counter >= (time * 60) && keyboardcode == 0x81) {
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

	vg_exit();
}

int test_controller() {

	/* To be completed */

}

