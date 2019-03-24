# Arduino Uno EEPROM programmer & EEPROM dumper (eeprog & eedump)

## Initialise project

### Uploading sketch
#### Arduino IDE
Open the sketch in Arduino IDE and upload it on Arduino Uno

#### USBasp
Open the sketch in Arduino IDE and export it as binary
```console
$> avrdude -p atmega328p -P usb -c usbasp -B 1 -U flash:w:[file.hex]
```

### Compile
```console
$> gcc -o eeprog -I inc/ src/arduino-serial-lib.c src/prog.c
$> gcc -o eedump -I inc/ src/arduino-serial-lib.c src/dump.c
```

## Run
Be sure that your arduino is up-to-date and is connected to your computer with serial interface available

The computer program have a default port for communication but it can be specified

### Program
#### Usage
```console
$> ./eeprog FILE [INTERFACE]
```

#### Program EEPROM
```console
$> ./eeprog firmware
```

#### Specify interface
```console
$> ./eeprog firmware /dev/ttyUSB0
```

### Dump
#### Usage
```console
$> ./eedump [INTERFACE]
```

#### Dump EEPROM
```console
$> ./eedump
```

#### Specify interface
```console
$> ./eedump /dev/ttyUSB0
```