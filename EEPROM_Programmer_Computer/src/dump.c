#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <arduino-serial-lib.h>

#define PROGRAM_SIZE 0x8000

uint8_t program[PROGRAM_SIZE];
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
    strcpy(interface, "/dev/cu.usbserial-UUT2");
  printf("Interface: %s\n", interface);
  serial = serialport_init(interface, 9600);
	if (serial == -1)
		return 1;
  sleep(1);
	serialport_writebyte(serial, 1);
	sleep(1);
	/* Part to dump eeprom progressively
	for (uint16_t i = 0; i < PROGRAM_SIZE; i++) {
		n = 0;
		printf("\rGet program %3i%%", (i + 1) * 100 / (PROGRAM_SIZE));
		fflush(stdout);
		while (n != 1)
			n = read(serial, &byte, 1);
		program[i] = byte;
	}
	printf("\rDone!           \n");
	printProgram();//*/
	//* Part to dump eeprom directly
	for (uint16_t i = 0; i < PROGRAM_SIZE; i++) {
		if (!(i % 16)) {
			if (i)
				printf("\n");
			printf("%04x: ", i);
		}
		n = 0;
		while (n != 1)
			n = read(serial, &byte, 1);
		program[i] = byte;
		printf("%02x ", program[i]);
	}
	printf("\n");//*/
}