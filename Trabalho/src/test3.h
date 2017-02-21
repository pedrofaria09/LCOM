#ifndef __TEST3_H
#define __TEST3_H

/** @defgroup test3 test3
 * @{
 *
 * Functions for testing the kbd code
 */

/**
 * Function developed in lab3 - Keyboard subscribe
 * @return Return 0 upon success and non-zero otherwise
 */
int keyboard_subscribe_int(void);

/**
 * Function developed in lab3 - Keyboard unsubscribe
 * @return Return 0 upon success and non-zero otherwise
 */
int keyboard_unsubscribe_int();

/**
 * Function developed in lab3 - get the code of keyboard
 * @return long with the keyboard code
 */
unsigned long getKeyboardCode();


#endif /* __TEST3_H */
