#include <limits.h>
#include <string.h>
#include <errno.h>
#include "pixmap.h"

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char **argv) {
	/* DO NOT FORGET TO initialize service */
	sef_startup();

	if (argc == 1) { /* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	} else
		return proc_args(argc, argv);

}

static void print_usage(char **argv) {
	printf(
			"Usage: one of the following:\n"
					"\t service run %s -args \"init <decimal no. of mode - no. of delay>\"\n"
					"\t service run %s -args \"square <decimal no. of X - no. of Y - size - color>\"\n"
					"\t service run %s -args \"line <decimal no. of Xi - no. of Yi - no. of Xf - no. of Yf - color>\"\n"
					"\t service run %s -args \"xpm <decimal no. of X - no. of Y - name of xpm>\"\n"
					"\t service run %s -args \"move <decimal no. of X - no. of Y - name of xpm - hor - deta - time>\"\n"
					"\t service run %s -args \"controller <()>\"\n", argv[0],
			argv[0], argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv) {

	unsigned short mode, delay, xi, yi, xf, yf, size, hor, delta, time;
	unsigned long color;
	char* xpm;

	if (strncmp(argv[1], "init", strlen("init")) == 0) {
		if (argc != 4) {
			printf("init: wrong no. of arguments for test_init()\n");
			return 1;
		}
		mode = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		delay = parse_ulong(argv[3], 10);
		if (mode == ULONG_MAX || delay == ULONG_MAX)
			return 1;
		printf("init::test_init(%lu, %lu)\n", mode, delay);
		return test_init(mode, delay);
	}

	else if (strncmp(argv[1], "square", strlen("square")) == 0) {
		if (argc != 6) {
			printf("square: wrong no. of arguments for test_square()\n");
			return 1;
		}
		xi = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		yi = parse_ulong(argv[3], 10);
		size = parse_ulong(argv[4], 10);
		color = parse_ulong(argv[5], 10);
		if (xi == ULONG_MAX || yi == ULONG_MAX || size == ULONG_MAX
				|| color == ULONG_MAX)
			return 1;
		printf("square::test_square(%lu, %lu, %lu, %lu)\n", xi, yi, size,
				color);
		return test_square(xi, yi, size, color);
	}

	else if (strncmp(argv[1], "line", strlen("line")) == 0) {
		if (argc != 7) {
			printf("line: wrong no. of arguments for test_line()\n");
			return 1;
		}
		xi = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		yi = parse_ulong(argv[3], 10);
		xf = parse_ulong(argv[4], 10);
		yf = parse_ulong(argv[5], 10);
		color = parse_ulong(argv[6], 10);
		if (xi == ULONG_MAX || yi == ULONG_MAX || xf == ULONG_MAX
				|| yf == ULONG_MAX || color == ULONG_MAX)
			return 1;
		printf("line::test_line(%lu, %lu, %lu, %lu, %lu)\n", xi, yi, xf, yf,
				color);
		return test_line(xi, yi, xf, yf, color);
	} else if (strncmp(argv[1], "xpm", strlen("xpm")) == 0) {
		if (argc != 5) {
			printf("xpm: wrong no. of arguments for test_xpm()\n");
			return 1;
		}
		xi = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		yi = parse_ulong(argv[3], 10);

		if (xi == ULONG_MAX || yi == ULONG_MAX)
			return 1;

		if (strncmp(argv[4], "pic1", strlen("pic1")) == 0) {
			printf("xpm::test_xpm(%lu, %lu, %lu)\n", xi, yi, argv[4]);
			return test_xpm(xi, yi, pic1);
		} else if (strncmp(argv[4], "pic2", strlen("pic2")) == 0) {
			printf("xpm::test_xpm(%lu, %lu, %lu)\n", xi, yi, argv[4]);
			return test_xpm(xi, yi, pic2);
		} else if (strncmp(argv[4], "cross", strlen("cross")) == 0) {
			printf("xpm::test_xpm(%lu, %lu, %lu)\n", xi, yi, argv[4]);
			return test_xpm(xi, yi, cross);
		} else if (strncmp(argv[4], "pic3", strlen("pic3")) == 0) {
			printf("xpm::test_xpm(%lu, %lu, %lu)\n", xi, yi, argv[4]);
			return test_xpm(xi, yi, pic3);
		} else if (strncmp(argv[4], "penguin", strlen("penguin")) == 0) {
			printf("xpm::test_xpm(%lu, %lu, %lu)\n", xi, yi, argv[4]);
			return test_xpm(xi, yi, penguin);
		} else {
			printf("xpm::xpm not found\n");
			return 1;
		}
	} else if (strncmp(argv[1], "move", strlen("move")) == 0) {
		if (argc != 8) {
			printf("move: wrong no. of arguments for test_move()\n");
			return 1;
		}
		xi = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		yi = parse_ulong(argv[3], 10);
		hor = parse_ulong(argv[5], 10);
		delta = parse_ulong(argv[6], 10);
		time = parse_ulong(argv[7], 10);

		if (xi == ULONG_MAX || yi == ULONG_MAX || hor == ULONG_MAX || delta == ULONG_MAX || time == ULONG_MAX)
			return 1;

		if (strncmp(argv[4], "pic1", strlen("pic1")) == 0) {
			printf("move::test_move(%lu, %lu, %lu, %lu, %lu, %lu)\n", xi, yi, argv[4], hor, delta, time);
			return test_move(xi, yi, pic1, hor, delta, time);
		} else if (strncmp(argv[4], "pic2", strlen("pic2")) == 0) {
			printf("move::test_move(%lu, %lu, %lu, %lu, %lu, %lu)\n", xi, yi, argv[4], hor, delta, time);
			return test_move(xi, yi, pic2, hor, delta, time);
		} else if (strncmp(argv[4], "cross", strlen("cross")) == 0) {
			printf("move::test_move(%lu, %lu, %lu, %lu, %lu, %lu)\n", xi, yi, argv[4], hor, delta, time);
			return test_move(xi, yi, cross, hor, delta, time);
		} else if (strncmp(argv[4], "pic3", strlen("pic3")) == 0) {
			printf("move::test_move(%lu, %lu, %lu, %lu, %lu, %lu)\n", xi, yi, argv[4], hor, delta, time);
			return test_move(xi, yi, pic3, hor, delta, time);
		} else if (test_move(argv[4], "penguin", strlen("penguin")) == 0) {
			printf("move::test_move(%lu, %lu, %lu, %lu, %lu, %lu)\n", xi, yi, argv[4], hor, delta, time);
			return test_move(xi, yi, penguin, hor, delta, time);
		} else {
			printf("move::move not found\n");
			return 1;
		}
	}

	else {
		printf("timer: %s - no valid function!\n", argv[1]);
		return 1;
	}
}

static unsigned long parse_ulong(char *str, int base) {
	char *endptr;
	unsigned long val;

	/* Convert string to unsigned long */
	val = strtoul(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == ULONG_MAX) || (errno != 0 && val == 0)) {
		perror("strtoul");
		return ULONG_MAX;
	}

	if (endptr == str) {
		printf("timer: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}
