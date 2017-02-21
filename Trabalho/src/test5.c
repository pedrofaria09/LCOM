#include <minix/drivers.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/com.h>


#include "test5.h"

#define MODE_GRAPH 		0x105

void *test_init(unsigned short mode, unsigned short delay) {

	vg_init(mode);
	timer_test_int(delay);
	vg_exit();

}

