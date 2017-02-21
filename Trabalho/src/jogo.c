#include <minix/drivers.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/com.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "highscore.h"
#include "sprite.c"
#include "test5.c"
#include "test4.c"
#include "test3.c"
#include "timer.c"
#include "pixmap.h"
#include "read_xpm.c"

#define MODE_GRAPH 		0x105
#define DIF_RECTANGLES	3

static GameSpeed = 2;
static nrrectangles = 4;
static collision = 0;
static xPlayer = 100;
static yPlayer = V_RES / 2;
char **boneco = penguin;

void draw_Player(unsigned short xi, unsigned short yi, char *xpm[]) {

	int i = 0, j = 0, a = 0;
	char *rd_xpm;
	int width, height;

	rd_xpm = read_xpm(xpm, &width, &height);

	for (j = yi; j < height + yi; j++) {
		for (i = xi; i < width + xi; i++) {
			video_mem[(i + j * H_RES)] = rd_xpm[a];
			a++;
		}
	}
}

void erase_Player(unsigned short xi, unsigned short yi, char *xpm[]) {

	int i = 0, j = 0, a = 0;
	int width, height;
	char *rd_xpm;

	rd_xpm = read_xpm(xpm, &width, &height);

	for (j = yi; j < height + yi; j++) {
		for (i = xi; i < width + xi; i++) {
			video_mem[(i + j * H_RES)] = 0;
		}
	}
}

void erase_rectangles(Sprite *rectangle) {

	int j = 0, k = 0;

	for (j = rectangle->y; j < (rectangle->y + rectangle->height); j++) {
		for (k = rectangle->x; k < rectangle->x + rectangle->width; k++) {
			video_mem[(k + j * H_RES)] = 0;
		}
	}
}

void spawn_rectangle(Sprite **rectangles) {

	int i;
	for (i = 0; i < nrrectangles; i++) {
		rectangles[i] = create_sprite(0, 0, 0, 0, 0);

	}
}

void change_heights_of_rectangles(int characterheight, Sprite *rectangle1,
		Sprite *rectangle2) {
	int retwidth = 50;
	srand(time(NULL));
	int limit = (V_RES - characterheight - 125);
	int retheight = rand() % limit;

	rectangle1->width = retwidth;
	rectangle1->height = retheight;
	rectangle1->x = H_RES - retwidth - 1;
	rectangle1->y = 0;
	rectangle1->speed = GameSpeed;

	rectangle2->width = retwidth;
	rectangle2->height = V_RES - retheight - characterheight - 125;
	rectangle2->x = H_RES - 1 - retwidth;
	rectangle2->y = retheight + characterheight + 124;
	rectangle2->speed = GameSpeed;
}

void draw_rectangles(Sprite **rectangles) {
	int i, j, k;

	for (i = 0; i < nrrectangles; i++) {
		if (rectangles[i]->visible == 1) {
			rectangles[i]->speed = GameSpeed;
			for (j = rectangles[i]->y;
					j < (rectangles[i]->y + rectangles[i]->height); j++) {
				for (k = rectangles[i]->x;
						(k < rectangles[i]->x + rectangles[i]->width); k++) {
					video_mem[(k + j * H_RES)] = 50;
				}
			}
		}
	}
}

void check_collision(Sprite **rectangles) {
	int i, j, k;

	for (i = 0; i < nrrectangles; i++) {
		if (rectangles[i]->visible == 1) {
			rectangles[i]->speed = GameSpeed;
			for (j = rectangles[i]->y;
					j < (rectangles[i]->y + rectangles[i]->height); j++) {
				for (k = rectangles[i]->x;
						(k < rectangles[i]->x + rectangles[i]->width); k++) {
					if (video_mem[(k - rectangles[i]->speed + j * H_RES)] != 0)
						collision = 1;
				}
			}
		}
	}
}

void delete_retangles(Sprite **rectangles) {
	int i, j, k;

	for (i = 0; i < nrrectangles; i++) {
		free(rectangles[i]);
	}
}

int getYPS2Info(unsigned char* packet) {
	int y;

	if (packet[0] & BIT(5))
		y = (packet[2] | (-1 << 8));
	else
		y = packet[2];

	return y;
}

int getMenuNumber() {
	int value = 0;
	unsigned long keyboardcode = 0;
	int quit = 0;
	int ipc_status, r, irq_set;
	message msg;
	irq_set = keyboard_subscribe_int();
	int verify = 0;

	while (!quit) {
		quit = 0;
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:
				if (msg.NOTIFY_ARG & irq_set) {
					//Verify errors
					if (irq_set == -1) {
						return -1;
					}

					//Get the keyboard code
					keyboardcode = getKeyboardCode();

					//printf("Code: 0x%02X \n", keyboardcode);

					//If keyboard code is exit key, will exit
					if (keyboardcode == 0x81) {
						quit = 1;
						break;
					}
					if (keyboardcode == 0x0B || keyboardcode == 0x52) {
						value = 0;
						quit = 1;
						break;
					}
					if (keyboardcode == 0x02 || keyboardcode == 0x4F) {
						value = 1;
						quit = 1;
						break;
					}
					if (keyboardcode == 0x03 || keyboardcode == 0x50) {
						value = 2;
						quit = 1;
						break;
					}
					if (keyboardcode == 0x04 || keyboardcode == 0x51) {
						value = 3;
						quit = 1;
						break;
					}
					if (keyboardcode == 0x05 || keyboardcode == 0x4B) {
						value = 4;
						quit = 1;
						break;
					}
					if (keyboardcode == 0x06 || keyboardcode == 0x4C) {
						value = 5;
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
	return value;

}

void viewScore(char *dataDir) {
	//getUsersInfo(dataDir);

	showBestUsersInfo(dataDir);
	//showAllUsers();
}

void start(char *dataDir, char *userName) {

	int aux = 2, valueY = 0, width, height, quit = 0, ipc_status, r, irq_setKBC,
			irq_setMOUSE, irq_setTimer, spawnrect = 1, GameTime = 0, j, i, force = 0;
	char *rd_xpm, packet[3];
	Sprite *rectangles[nrrectangles];
	unsigned long keyboardcode = 0;
	message msg;
	long status;

	getUsersInfo(dataDir);
	vg_init(MODE_GRAPH);
	draw_Player(xPlayer, yPlayer, boneco);
	rd_xpm = read_xpm(boneco, &width, &height);

	irq_setKBC = keyboard_subscribe_int();
	irq_setTimer = timer_subscribe_int();
	irq_setMOUSE = mouse_subscribe_int();

	spawn_rectangle(rectangles);

	aux_PS2(STAT_REG, WRITE_PS2);
	aux_PS2(IN_BUF, ENABLE_STREAM_MODE);
	sys_inb(OUT_BUF, &status);
	while (!quit && !collision) {
		quit = 0;
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE:

				//TIMER
				if (msg.NOTIFY_ARG & irq_setTimer) {
					draw_Player(xPlayer, yPlayer, boneco);

					if (spawnrect == 1) {
						for (i = 0; i < nrrectangles; i++) {
							if (rectangles[i]->visible == 0) {
								change_heights_of_rectangles(height,
										rectangles[i], rectangles[i + 1]);
								rectangles[i]->visible = 1;
								rectangles[i + 1]->visible = 1;
								break;
							}
						}
					}

					if ((GameTime % (10 * 60)) == 0) {
						GameSpeed += 1;
					}
					for (i = 0; i < nrrectangles; i++) {
						if (rectangles[i]->visible == 1) {

							if (rectangles[i]->x < H_RES / 2)
								spawnrect = 1;
							if (rectangles[i]->x >= H_RES / 2)
								spawnrect = 0;

							erase_rectangles(rectangles[i]);

							if (rectangles[i]->x - rectangles[i]->speed >= 5) {
								rectangles[i]->x -= rectangles[i]->speed;
							} else {
								rectangles[i]->visible = 0;
							}
						}
					}
					check_collision(rectangles);

					draw_rectangles(rectangles);

					GameTime++;
				}

				//TECLADO
				if (msg.NOTIFY_ARG & irq_setKBC) {
					//Verify errors
					if (irq_setKBC == -1) {
						break;
					}

					//Get the keyboard code
					keyboardcode = getKeyboardCode();

					//If keyboard code is exit key, will exit
					if (keyboardcode == 0x81) {
						quit = 1;
						break;
					}

					//Pause
					if (keyboardcode == 0x19) {
						keyboardcode = 0x01;

						while (keyboardcode != 0x19) {
							keyboardcode = getKeyboardCode();
							force=0;
						}

					}

					erase_Player(xPlayer, yPlayer, boneco);
					//ir para cima
					if (keyboardcode == 0x11) {
						if ((yPlayer - (GameSpeed * 5)) > 10) {
							yPlayer -= GameSpeed * 5;
						} else {
							yPlayer = 10;
						}
					}
					//ir para baixo
					if (keyboardcode == 0x1F) {
						if ((height + yPlayer + (GameSpeed * 5)) < V_RES - 10) {
							yPlayer += GameSpeed * 5;
						} else {
							yPlayer = V_RES - 10 - height;
						}
					}
					draw_Player(xPlayer, yPlayer, boneco);
					draw_rectangles(rectangles);
				}

				//RATO
				if (force == 1) {
					if (msg.NOTIFY_ARG & irq_setMOUSE) {
						switch (aux) {
						case 1:
							draw_Player(xPlayer, yPlayer, boneco);
							packet[0] = getPS2Code();
							if (packet[0] & BIT(3))
								aux = 2;
							break;
						case 2:
							packet[1] = getPS2Code();
							aux = 3;
							break;
						case 3:
							packet[2] = getPS2Code();
							valueY = getYPS2Info(packet);
							if (valueY > 10)
								valueY = 10;
							if (valueY < -10)
								valueY = -10;
							erase_Player(xPlayer, yPlayer, boneco);
							if (valueY > 0) {
								if ((yPlayer - valueY - GameSpeed > 10))
									yPlayer -= valueY + GameSpeed;
								else
									yPlayer = 10;
							} else if (valueY <= 0) {
								if ((height + yPlayer + (-valueY) + GameSpeed)
										< V_RES - 10)
									yPlayer += (-valueY) + GameSpeed;
								else
									yPlayer = V_RES - 10 - height;
							}
							draw_Player(xPlayer, yPlayer, boneco);
							draw_rectangles(rectangles);
							aux = 1;
							break;
						}
					}
				}
				force = 1;
				break;
			}
		}
	}

	mouse_unsubscribe_int();
	timer_unsubscribe_int();
	keyboard_unsubscribe_int();
	vg_exit();

	if (collision) {
		printf("Collision detected!!!\n\n\n"
				"UserName: %s\n"
				"Score: %d\n\n\n", userName, (GameTime / 60));
		addUserInfo(dataDir, userName, (GameTime / 60));

	}

	if (quit) {
		printf("Quit detected!!!\n\n\n"
				"UserName: %s\n"
				"Score: %d\n\n\n", userName, (GameTime / 60));
		addUserInfo(dataDir, userName, (GameTime / 60));
	}
	delete_retangles(rectangles);
}

void menuChoosePlayer() {
	int valor;

	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	printf("=========== Choose Character ===========\n");
	printf("1- Penguin\n");
	printf("2- Spaceship 1\n");
	printf("3- Spaceship 2\n");
	printf("\nPlease choose an option>");
	valor = getMenuNumber();
	printf("%d\n", valor);

	switch (valor) {
	case 1:
		boneco = penguin;
		break;
	case 2:
		boneco = pic1;
		break;
	case 3:
		boneco = pic3;
		break;
	}
	printf("\nCharacter changed!!!\n");
	printf("\nLeaving in 2 seconds...");
}

void menu(char *dataDir, char *userName) {
	int value;
	int played = 0;

	do {
		printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
		printf("=========== MENU ===========\n");
		printf("1- Play\n");
		printf("2- View Score\n");
		printf("3- Choose character\n");
		printf("4- Exit\n");
		printf("\nPlease choose an option>");
		value = getMenuNumber();
		printf("%d\n", value);

		if (value == 4)
			break;

		switch (value) {
		case 1:
			played = 1;
			start(dataDir, userName);
			break;
		case 2:
			printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
			viewScore(dataDir);
			//timer_test_int(2); NAO USAR
			sleep(2);
			break;
		case 3:
			menuChoosePlayer();
			//timer_test_int(2); NAO USAR
			sleep(2);
			break;
		}
	} while (played == 0);

	printf("Exiting ... \n");
}
