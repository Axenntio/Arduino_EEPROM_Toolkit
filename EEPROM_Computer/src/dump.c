#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <arduino-serial-lib.h>

int serial;

uint8_t dump(uint16_t address);

int main(int argc, char **argv)
{
	uint8_t byte;
	uint16_t address = 0;
	uint16_t start_address = 0;
	uint16_t end_address = 0x8000;
	bool display_address = false;
	char *interface = strdup("/dev/ttyUSB0");

	char opt;
	while ((opt = getopt(argc, argv, ":i:a:s:e:h")) != -1) {
		switch (opt) {
			case 'i':
				free(interface);
				interface = strdup(optarg);
				break;
			case 's':
				start_address = strtol(optarg, NULL, 16);
				break;
			case 'e':
				end_address = strtol(optarg, NULL, 16);
				break;
			case 'a':
				display_address = true;
				address = strtol(optarg, NULL, 16);
				break;
			case '?':
				printf("Unrecognized option '-%c'\n\n", optopt);
			case ':':
			case 'h':
                printf("USAGE: %s\n", argv[0]);
				printf("Flags:\n", argv[0]);
				printf("\t-i <interface>: the interface of the programmer. DEFAULT = /dev/ttyUSB0\n", argv[0]);
				printf("\t-s <address>: start address of the dump (base 16). DEFAULT = 0x0000\n", argv[0]);
				printf("\t-e <address>: end address of the dump (base 16). DEFAULT = 0x8000\n", argv[0]);
				printf("\t-a <address>: dump a specific address (base 16) of the EEPROM.\n", argv[0]);
				return (1);
		}
	}
	printf("Interface: %s\n", interface);
	serial = serialport_init(interface, 19200);
	if (serial == -1)
		return (1);

	char line[17];
	line[16] = '\0';

	if (display_address) {
		byte = dump(address);
		printf("%04x: %02x (%c)\n", address, byte, (isprint(byte)) ? byte : '.');
		return (0);
	}
	for (address = start_address; address < (end_address + 1); address++) {
		if (!(address % 16)) {
			if (address != start_address) {
				printf("\t%s\n", line);
			}
			if (address < end_address)
				printf("%04x: ", address);
		}
		byte = dump(address);
		if (isprint(byte))
			line[address % 16] = byte;
		else
			line[address % 16] = '.';
		if (address < end_address) {
			printf("%02x", byte);
			if (address % 2)
				printf(" ");
		}
	}
	if (!(address % 16))
		printf("\n");
	return (0);
}

uint8_t dump(uint16_t address)
{
	uint8_t byte;
	int n = 0;

	serialport_writebyte(serial, 1);
	serialport_writebyte(serial, (address >> 8) & 0xff);
	serialport_writebyte(serial, address & 0xff);
	while (n != 1) {
		n = read(serial, &byte, 1);
		usleep(1000);
	}
	return (byte);
}