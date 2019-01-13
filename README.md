# Arduino Uno EEPROM programmer (eeprog)

## Initialise project

### Uploading sketch
Open the sketch in Arduino IDE and upload it on Arduino Uno

### Compile
```console
$> gcc -o eeprog -I inc/ src/*
```

## Run
Be sure that your arduino is up-to-date and is connected to your computer with serial interface available

The computer program have a default port for communication but it can be specified

### Usage
```console
$> ./eeprog FILE [INTERFACE]
```

### Program EEPROM
```console
$> ./eeprog firmware
```

### Specify interface
```console
$> ./eeprog firmware /dev/ttyUSB0
```