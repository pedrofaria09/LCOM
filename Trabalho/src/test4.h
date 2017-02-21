#ifndef __TEST4_H
#define __TEST4_H

/** @defgroup test4 test4
 * @{
 *
 * Functions for testing the kbd code
 */

/**
 * Function developed in lab4 - Mouse subscribe
 * @return Return 0 upon success and non-zero otherwise
 */
int mouse_subscribe_int(void);

/**
 * Function developed in lab4 - Mouse unsubscribe
 * @return Return 0 upon success and non-zero otherwise
 */
int mouse_unsubscribe_int();

/**
 * Function developed in lab4 - get the code of mouse
 * @return Return 0 upon success and non-zero otherwise
 */
char getPS2Code();

/**
 * Function developed in lab4 - function auxiliar for PS2
 * @return Return 0 upon success and non-zero otherwise
 */
int aux_PS2(unsigned long command, unsigned char aux);

#endif /* __TEST_4.H */
