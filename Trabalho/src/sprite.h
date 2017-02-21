/**
 *  @author Joao Cardoso (jcard@fe.up.pt) ????
 *  Added by pfs@fe.up.pt
 */

#ifndef _SPRITE_H_
#define _SPRITE_H_

/** A Sprite is an "object" that contains all needed information to
 * create, animate, and destroy a pixmap.  The functions assume that
 * the background is BLACK and they take into account collision with
 * other graphical objects or the screen limits.
 */
typedef struct {
  int x,y;             	/**< current sprite position */
  int width, height;   	/**< sprite dimensions */
  int speed;  			/**< current speeds in the x direction */
  int visible;
} Sprite;

/** Creates with random speeds (not zero) and position
 * (within the screen limits), a new sprite with pixmap "pic", in
 * memory whose address is "base";
 * Returns NULL on invalid pixmap.
 */
Sprite *create_sprite(int width, int height, int x, int y, int speed);


#endif
