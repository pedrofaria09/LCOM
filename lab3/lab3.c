#include "test3.h"

//int main(){
//
//	sef_startup();
//
//	//Function will receive 0 to process in c
//	//kbd_test_scan(0);
//
//	unsigned short elem[] = {2,2,1,1,0,0};
//	unsigned short n = sizeof(elem)/sizeof(elem[0]);
//
//	//Receive n numbers of elements in array elem[] and its array.
//	kbd_test_leds(n,elem);
//
//
//	return 0;
//}

//============================
#include <limits.h>
#include <string.h>
#include <errno.h>

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char **argv)
{
	/* DO NOT FORGET TO initialize service */
	sef_startup();

	if (argc == 1) {					/* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);

}

static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"scan <decimal no.- C or Assembly>\"\n"
			"\t service run %s -args \"leds <decimal no. - number of elements - elements>\"\n"
			"\t service run %s -args \"timed_scan <decimal no. - time>\"\n",
			argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{
	unsigned short scan, freq, time;
	unsigned short elem[]={};
	unsigned int aux, i;

	if (strncmp(argv[1], "scan", strlen("scan")) == 0) {
		if (argc != 3) {
			printf("scan: wrong no. of arguments for kbd_test_scan()\n");
			return 1;
		}
		scan = parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		if (scan == ULONG_MAX)
			return 1;
		printf("scan::kbd_test_scan(%lu)\n", scan);
		return kbd_test_scan(scan);
	}

	else if (strncmp(argv[1], "leds", strlen("leds")) == 0) {
		aux = parse_ulong(argv[2], 10);
		if (argc != (3+aux)) {
			printf("leds: wrong no. of arguments for kbd_test_leds()\n");
			return 1;
		}
						/* Parses string to unsigned long */
		if (aux == ULONG_MAX)
			return 1;


		for(i=0; i< aux; i++){
			elem[i] = parse_ulong(argv[(3+i)], 10);
		}
		printf("leds::kbd_test_leds(%lu,%s)\n", aux, elem);
		return kbd_test_leds(aux, elem);
	}

	else if (strncmp(argv[1], "timed_scan", strlen("timed_scan")) == 0) {
		if (argc != 3) {
			printf("timer: wrong no of arguments for kbd_test_timed_scan()\n");
			return 1;
		}
		time = parse_ulong(argv[2], 10);						/* Parses string to unsigned long */
		if (time == ULONG_MAX)
			return 1;
		printf("timer::kbd_test_timed_scan(%lu)\n", time);
		return kbd_test_timed_scan(time);
	}

	else {
		printf("timer: %s - no valid function!\n", argv[1]);
		return 1;
	}
}

static unsigned long parse_ulong(char *str, int base)
{
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
