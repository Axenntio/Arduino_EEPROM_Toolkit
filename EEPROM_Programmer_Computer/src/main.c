#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <arduino-serial-lib.h>

#define RESET_VECTORS 0
#define PARTS_SIZE		0x400
#define PROGRAM_SIZE 0x8000

uint8_t program[PROGRAM_SIZE];
uint8_t PARTS_NUMBER = 0x20;
uint16_t programSize = 0;
int	serial;

void printProgram();

void input()
{
	uint8_t b;
	int n = read(serial, &b, 1);
	if(n == 1)
		printf("%i\n", b);
}

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
	if (!RESET_VECTORS) {
		uint16_t min_size = 0x7ffa;
		while (program[--min_size] == 0);
		PARTS_NUMBER = (min_size - (min_size % PARTS_SIZE)) / PARTS_SIZE + 1;
	}
  if (argc == 3)
		strcpy(interface, argv[2]);
  else
    strcpy(interface, "/dev/cu.usbmodem1411");
  printf("Interface: %s\n", interface);
  serial = serialport_init(interface, 9600);
	if (serial == -1)
		return 1;
  sleep(2);
	
	serialport_writebyte(serial, PARTS_NUMBER);
	for (uint8_t part = 0; part < PARTS_NUMBER; part++) {
		for (uint16_t i = 0; i < PARTS_SIZE; i++) {
			printf("\r[%i/%i] Send program part    %3i%%", part + 1, PARTS_NUMBER, (i + 1) * 100 / PARTS_SIZE);
			fflush(stdout);
			serialport_writebyte(serial, program[i + PARTS_SIZE * part]);
			n = 0;
			while (n != 1)
				n = read(serial, &byte, 1);
		}
		isOk = 1;
		for (uint16_t i = 0; i < PARTS_SIZE; i++) {
			n = 0;
			printf("\r[%i/%i] Receive program part %3i%%", part + 1, PARTS_NUMBER, (i + 1) * 100 / PARTS_SIZE);
			fflush(stdout);
			while (n != 1)
				n = read(serial, &byte, 1);
			if (program[i + PARTS_SIZE * part] != byte)
				isOk = 0;
		}
		if (isOk) {
			serialport_writebyte(serial, 1);
			for (uint16_t i = 0; i < PARTS_SIZE; i++) {
				n = 0;
				printf("\r[%i/%i] Writing program part %3i%%", part + 1, PARTS_NUMBER, (i + 1) * 100 / PARTS_SIZE);
				fflush(stdout);
				while (n != 1)
					n = read(serial, &byte, 1);
			}
			printf("\r[%i/%i] Done...                  \n", part + 1, PARTS_NUMBER);
		}
		else {
			printf("\r[%i/%i] Part does not match...   \n", part + 1, PARTS_NUMBER);
			serialport_writebyte(serial, 0);
		}
	}
	uint16_t bytes_differ = 0;
	for (uint16_t i = 0; i < PARTS_SIZE * PARTS_NUMBER; i++) {
		n = 0;
		printf("\rVerify program integrity %3i%%", (i + 1) * 100 / (PARTS_SIZE * PARTS_NUMBER));
		fflush(stdout);
		while (n != 1)
			n = read(serial, &byte, 1);
		if (program[i] != byte)
			bytes_differ++;
	}
	if (!bytes_differ)
		printf("\rIntegrity done!              \n");
	else
		printf("\rIntegrity failed...  (0x%04x)\n", bytes_differ);
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