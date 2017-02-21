#ifndef __JOGO_H
#define __JOGO_H


/**
 * Draw a player on xi and yi position with the Character xpm
 * @param xi X position
 * @param yi Y position
 * @param xpm Character
 */
void draw_Player(unsigned short xi, unsigned short yi, char *xpm[]);


/**
 * Erase the player on xi and yi position
 * @param xi X position
 * @param yi Y position
 */
void erase_Player(unsigned short xi, unsigned short yi);


/**
 * Erase a rectangle
 * @param rectangle rectangle to erase
 */
void erase_rectangles(Sprite *rectangle);


/**
 * Create Sprite's with the default values
 * @param rectangles Array of rectangles
 */
void spawn_rectangle(Sprite **rectangles);


/**
 * Change the values of the created Sprite's
 * @param characterheight value of the heigh of the character
 * @param rectangle1 Top rectangle
 * @param rectangle2 Bottum rectangle
 */
void change_heights_of_rectangles(int characterheight, Sprite *rectangle1, Sprite *rectangle2);


/**
 * Draw the rectangles able to it (when visible = 1)
 * @param rectangles Array of rectangles
 */
void draw_rectangles(Sprite **rectangles);


/**
 * Check if any rectangle dont have the background color, if it happens, collision is made, player loose the game
 * @param rectangles Array of rectangles
 */
void check_collision(Sprite **rectangles);


/**
 * Free all rectangles
 * @param rectangles Array of rectangles
 */
void delete_retangles(Sprite **rectangles);


/**
 * Return a Y position of the mouse
 * @param packet Packet of the mouse
 * @return integer with y position of the mouse
 */
int getYPS2Info(unsigned char* packet);


/**
 * Return numbers of the keyboard
 * @return integer with the number of the keyboard
 */
int getMenuNumber();


/**
 * Show the score of players
 * @param dataDir Path of project
 */
void viewScore(char *dataDir);


/**
 * Principal function of the game, show and play the game
 * @param dataDir Path of the project
 * @param userName Name of the player
 */
void start(char *dataDir, char *userName);


/**
 * Choose the character to be played
 */
void menuChoosePlayer();


/**
 * Principal menu of the game
 * @param dataDir Path of the project
 * @param userName Name of the player
 */
void menu(char *dataDir, char *userName);

#endif
