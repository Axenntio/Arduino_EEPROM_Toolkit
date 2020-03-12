#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <arduino-serial-lib.h>

#define PROGRAM_SIZE 0x8000

unsigned char program[PROGRAM_SIZE];
int	serial;

void printProgram();

int main(int argc, char **argv)
{
	FILE *file;
	char interface[32];
	uint8_t isOk = 0;
	uint8_t byte;
	int n;
	
	if (argc < 2 || argc > 3) {
		printf("USAGE: %s FILENAME [INTERFACE]\n", argv[0]);
		return (1);
	}
	file = fopen(argv[1], "rb");
	fread(program, sizeof(program), PROGRAM_SIZE, file);
	if (argc == 3)
		strcpy(interface, argv[2]);
	else
    strcpy(interface, "/dev/ttyUSB0");
	printf("Interface: %s\n", interface);
	serial = serialport_init(interface, 19200);
	if (serial == -1)
		return 1;
	uint16_t program_size = 0x7ffa;
	while (program[--program_size] == 0 || program[program_size] == 0xff);
	sleep(1);
	for (uint16_t address = 0; address < program_size; address++) {
		printf("\r[0x%04x/0x%04x] Writing program %3i%%", address, program_size, (address) * 100 / program_size);
		fflush(stdout);
		serialport_writebyte(serial, 0);
		serialport_writebyte(serial, (address >> 8) & 0xff);
		serialport_writebyte(serial, address & 0xff);
		serialport_writebyte(serial, program[address]);
		n = 0;
		while (n != 1)
			n = read(serial, &byte, 1);
	}
	printf("\r[0x%04x/0x%04x] Writing program 100%%\n", program_size, program_size);
	for (uint16_t address = 0x7ffa; address < PROGRAM_SIZE; address++) {
		printf("\r[0x%04x/0x%04x] Writing vectors %3i%%", address, PROGRAM_SIZE, (address) * 100 / PROGRAM_SIZE);
		fflush(stdout);
		serialport_writebyte(serial, 0);
		serialport_writebyte(serial, (address >> 8) & 0xff);
		serialport_writebyte(serial, address & 0xff);
		serialport_writebyte(serial, program[address]);
		n = 0;
		while (n != 1)
			n = read(serial, &byte, 1);
	}
	printf("\r[0x%04x/0x%04x] Writing vectors 100%%\n", PROGRAM_SIZE, PROGRAM_SIZE);
	uint16_t bytes_differ = 0;
	for (uint16_t address = 0; address < program_size; address++) {
		uint8_t byte = 0xff;
		printf("\r[0x%04x/0x%04x] Verify program integrity %3i%%", address, program_size, (address) * 100 / program_size);
		fflush(stdout);
		serialport_writebyte(serial, 1);
		serialport_writebyte(serial, (address >> 8) & 0xff);
		serialport_writebyte(serial, address & 0xff);
		n = 0;
		while (n != 1)
			n = read(serial, &byte, 1);
		if (program[address] != byte)
			bytes_differ++;
	}
	if (!bytes_differ)
		printf("\rIntegrity done!                              \n");
	else
		printf("\rIntegrity failed...                   (0x%04x)\n", bytes_differ);
	serialport_close(serial);
	return 0;
}

void printProgram()
{
	for (uint16_t i = 0; i < PROGRAM_SIZE; i++) {
		if (!(i % 16)) {
			if (i)
				printf("\n");
			printf("%04x: ", i);
		}
		printf("%02x ", program[i]);
	}
	printf("\n");
}