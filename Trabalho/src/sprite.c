#include "sprite.h"
#include "video_gr.c"

Sprite *create_sprite(int width, int height, int x, int y, int speed) {
	printf("width: %d \n", width);
	printf("height: %d \n", height);
	printf("x: %d \n", x);
	printf("y: %d \n", y);
	printf("speed: %d \n", speed);

	Sprite *sprite = malloc(sizeof(Sprite));

	sprite->x = x;
	sprite->y = y;
	sprite->width = width;
	sprite->height = height;
	sprite->speed = speed;
	sprite->visible = 0;


	return sprite;
}
