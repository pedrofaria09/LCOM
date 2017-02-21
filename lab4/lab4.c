#include <limits.h>
#include <string.h>
#include <errno.h>

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
					"\t service run %s -args \"packet <decimal no. of packets>\"\n"
					"\t service run %s -args \"async <decimal no. of seconds without packets>\"\n"
					"\t service run %s -args \"config <VOID>\"\n"
					"\t service run %s -args \"gesture <decimal no. minimum length of movement>\"\n",
			argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv) {
	unsigned short packets, asyncs, gestures;

	if (strncmp(argv[1], "packet", strlen("packet")) == 0) {
		if (argc != 3) {
			printf("packet: wrong no. of arguments for test_packet()\n");
			return 1;
		}
		packets = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		if (packets == ULONG_MAX)
			return 1;
		printf("packet::test_packet(%lu)\n", packets);
		return test_packet(packets);
	}

	else if (strncmp(argv[1], "async", strlen("async")) == 0) {
		if (argc != 3) {
			printf("async: wrong no. of arguments for test_async()\n");
			return 1;
		}
		asyncs = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		if (asyncs == ULONG_MAX)
			return 1;
		printf("async::test_async(%lu)\n", asyncs);
		return test_async(asyncs);
	}

	else if (strncmp(argv[1], "config", strlen("config")) == 0) {
		return test_config();
	}

	else if (strncmp(argv[1], "gesture", strlen("gesture")) == 0) {
		if (argc != 3) {
			printf("gesture: wrong no of arguments for test_gesture()\n");
			return 1;
		}
		gestures = parse_ulong(argv[2], 10); /* Parses string to unsigned long */
		if (gestures == ULONG_MAX)
			return 1;
		printf("gesture::test_gesture(%lu)\n", gestures);
		return test_gesture(gestures);
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
