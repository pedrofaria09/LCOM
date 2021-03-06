#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "vbe.h"
#include "video_gr.h"
/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */
#define VRAM_PHYS_ADDR		0xE0000000
#define H_RES             	1024
#define V_RES		  		768
#define BITS_PER_PIXEL	 	8

/* Private global variables */

static char *video_mem; /* Process address to which VRAM is mapped */

static unsigned h_res; /* Horizontal screen resolution in pixels */
static unsigned v_res; /* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

void *vg_init(unsigned short mode) {
	struct reg86u reg;
	int r;
	struct mem_range mr;
	unsigned int vram_base = VRAM_PHYS_ADDR;
	unsigned int vram_size = H_RES * V_RES;

	reg.u.w.ax = 0x4F02;
	reg.u.w.bx = (1 << 14 | mode); // set bit 14: linear framebuffer
	reg.u.b.intno = 0x10;

	/* Allow memory mapping */
	mr.mr_base = (phys_bytes) vram_base;
	mr.mr_limit = mr.mr_base + vram_size;

	if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("sys_privctl (ADD_MEM) failed: %d\n", r);

	/* Map memory */
	video_mem = vm_map_phys(SELF, (void *) mr.mr_base, vram_size);

	if (video_mem == MAP_FAILED)
		panic("couldn’t map video memory");

	if (sys_int86(&reg) != OK) {
		printf("set_vbe_mode: sys_int86() failed \n");
	}
}

void test() {
	int i;
	unsigned int vram_size = H_RES * V_RES;
	for (i = 0; i < vram_size; i++) {
		(video_mem[i]) = 0;
	}
}

int vg_exit() {
	struct reg86u reg86;

	reg86.u.b.intno = 0x10; /* BIOS video services */

	reg86.u.b.ah = 0x00; /* Set Video Mode function */
	reg86.u.b.al = 0x03; /* 80x25 text mode*/

	if (sys_int86(&reg86) != OK) {
		printf("\tvg_exit(): sys_int86() failed \n");
		return 1;
	} else
		return 0;
}
