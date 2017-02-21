#ifndef __TEST5_H
#define __TEST5_H

/** @defgroup lab5 lab5
 * @{
 *
 * Functions for outputing data to screen in graphics mode
 */

/**
 * @brief Tests initialization of video card in graphics mode
 * 
 * Uses the VBE INT 0x10 interface to set the desired
 *  graphics mode, and resets Minix's default text mode after 
 *  a short delay. Before exiting, displays on the console the
 *  physical address of VRAM in the input graphics mode.
 * 
 * @param mode 16-bit VBE mode to set
 * @param delay delay in seconds after which returns to text mode
 * @return Virtual address VRAM was mapped to. NULL, upon failure.
 */
void *test_init(unsigned short mode, unsigned short delay);

 
#endif /* __TEST5_H */
