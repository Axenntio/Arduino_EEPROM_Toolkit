#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arduino-serial-lib.h>

#define PROGRAM_SIZE 0x8000

unsigned char program[PROGRAM_SIZE];
int	serial;

uint8_t read_eeprom(uint16_t address);
void write_eeprom(uint16_t address, uint8_t byte);


int main(int argc, char **argv)
{
	FILE *file;
	char *interface = strdup("/dev/ttyUSB0");
	uint8_t isOk = 0;
	int n;
	
	if (argc < 2 || argc > 3) {
		printf("USAGE: %s FILENAME [INTERFACE]\n", argv[0]);
		return (1);
	}
	file = fopen(argv[1], "rb");
	fread(program, sizeof(program), PROGRAM_SIZE, file);
	if (argc == 3) {
		free(interface);
		interface = strdup(argv[2]);
	}
	printf("Interface: %s\n", interface);
	serial = serialport_init(interface, 19200);
	if (serial == -1)
		return 1;
	uint16_t program_size = 0x7ffa;
	while (program[--program_size] == 0 || program[program_size] == 0xff);
	program_size++;
	sleep(1);
	for (uint16_t address = 0; address < program_size; address++) {
		printf("\r[0x%04x/0x%04x] Writing program %3i%%", address, program_size, (address) * 100 / program_size);
		fflush(stdout);
		write_eeprom(address, program[address]);
	}
	printf("\r[0x%04x/0x%04x] Writing program 100%%\n", program_size, program_size);
	for (uint16_t address = 0x7ffa; address < PROGRAM_SIZE; address++) {
		printf("\r[0x%04x/0x%04x] Writing vectors %3i%%", address, PROGRAM_SIZE - 1, (address) * 100 / PROGRAM_SIZE - 1);
		fflush(stdout);
		write_eeprom(address, program[address]);
	}
	printf("\r[0x%04x/0x%04x] Writing vectors 100%%\n", PROGRAM_SIZE - 1, PROGRAM_SIZE - 1);

	uint16_t bytes_differ = 0;
	for (uint16_t address = 0; address < program_size; address++) {
		uint8_t byte = 0xff;
		printf("\r[0x%04x/0x%04x] Verify program integrity %3i%%", address, program_size, (address) * 100 / program_size);
		fflush(stdout);
		byte = read_eeprom(address);
		if (program[address] != byte)
			bytes_differ++;
	}
	printf("\r[0x%04x/0x%04x] Verify program integrity 100%%\n", program_size, program_size);
	if (!bytes_differ)
		printf("Integrity done!	  \n");
	else
		printf("Integrity failed...	   (0x%04x)\n", bytes_differ);
	serialport_close(serial);
	free(interface);
	return 0;
}

uint8_t read_eeprom(uint16_t address)
{
	uint8_t byte;
	int n = 0;

	serialport_writebyte(serial, 1);
	serialport_writebyte(serial, (address >> 8) & 0xff);
	serialport_writebyte(serial, address & 0xff);
	while (n != 1) {
		n = read(serial, &byte, 1);
		usleep(100);
	}
	return (byte);
}

void write_eeprom(uint16_t address, uint8_t byte)
{
	int n = 0;

	serialport_writebyte(serial, 0);
	serialport_writebyte(serial, (address >> 8) & 0xff);
	serialport_writebyte(serial, address & 0xff);
	serialport_writebyte(serial, byte);
	while (n != 1) {
		n = read(serial, &byte, 1);
		usleep(100);
	}
}