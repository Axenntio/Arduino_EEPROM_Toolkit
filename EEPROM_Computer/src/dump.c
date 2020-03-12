#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <arduino-serial-lib.h>

#define PROGRAM_SIZE 0x8000

int	serial;

int main(int argc, char **argv)
{
	char interface[32];
	uint8_t byte;
	int n;
	
	if (argc < 1 || argc > 2) {
		printf("USAGE: %s [INTERFACE]\n", argv[0]);
		return (1);
	}
	if (argc == 2)
		strcpy(interface, argv[1]);
	else
		strcpy(interface, "/dev/ttyUSB0");
	printf("Interface: %s\n", interface);
	serial = serialport_init(interface, 19200);
	if (serial == -1)
		return 1;
	sleep(1);
	char line[17];
	
	line[16] = '\0';
	for (uint16_t i = 0; i < PROGRAM_SIZE; i++) {
		if (!(i % 16)) {
			if (i) {
				printf("\t%s\n", line);
			}
			printf("%04x: ", i);
		}
		serialport_writebyte(serial, 1);
		serialport_writebyte(serial, (i >> 8) & 0xff);
		serialport_writebyte(serial, i & 0xff);
		n = 0;
		while (n != 1)
			n = read(serial, &byte, 1);
		if (isprint(byte))
			line[i % 16] = byte;
		else
			line[i % 16] = '.';
		printf("%02x", byte);
		if (i % 2)
			printf(" ");
	}
	printf("\n");
}