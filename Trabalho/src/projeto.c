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
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"start $PWD USER_NAME\"\n"
			"\t service run %s -args \"score $PWD\"\n"
			"\t service run %s -args \"menu $PWD USER_NAME\"\n", argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv) {

	if (strncmp(argv[1], "start", strlen("start")) == 0) {
		if (argc != 4) {
			printf("start: wrong no. of arguments for start()\n");
			return 1;
		}
		printf("start::start(%s - %s)\n", argv[2], argv[3]);
		return start(argv[2], argv[3]);
	} else if (strncmp(argv[1], "score", strlen("score")) == 0) {
		if (argc != 3) {
			printf("score: wrong no. of arguments for viewScore()\n");
			return 1;
		}
		printf("score::viewScore(%s)\n", argv[2]);
		return viewScore(argv[2]);
	} else if (strncmp(argv[1], "menu", strlen("menu")) == 0) {
		if (argc != 4) {
			printf("menu: wrong no. of arguments for menu()\n");
			return 1;
		}
		printf("menu::menu(%s - %s)\n", argv[2], argv[3]);
		return menu(argv[2], argv[3]);
	} else if (strncmp(argv[1], "teste", strlen("teste")) == 0) {
		if (argc != 2) {
			printf("menu: wrong no. of arguments for menu()\n");
			return 1;
		}
		printf("teste::teste()\n");
		return 1;
	}

	else {
		printf("start: %s - no valid function!\n", argv[1]);
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
