#define SHIFT_DATA  2
#define SHIFT_CLK   3
#define SHIFT_LATCH 4
#define WRITE_EN    13

#define PARTS_SIZE 0x400

uint8_t PARTS_NUMBER = 0x00;


unsigned char program[PARTS_SIZE];

void setAddress(uint16_t addr, bool outputEnable) {
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, (addr >> 8) | (outputEnable ? 0x00 : 0x80));
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, addr);
  digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);
}

uint8_t readAddress(uint16_t addr) {
  uint8_t value = 0;
  for (uint8_t i = 5; i <= 12; i++)
    pinMode(i, INPUT);
  setAddress(addr, true);
  for (uint8_t i = 12; i >= 5; i--) {
    value = (value << 1) + digitalRead(i);
  }
  return (value);
}

void writeAddress(uint16_t addr, uint8_t value) {
  for (uint8_t i = 5; i <= 12; i++)
    pinMode(i, OUTPUT);
  setAddress(addr, false);
  for (uint8_t i = 5; i <= 12; i++) {
    digitalWrite(i, value & 1);
    value >>= 1;
  }
  digitalWrite(WRITE_EN, LOW);
  delayMicroseconds(10);
  digitalWrite(WRITE_EN, HIGH);
  delay(10);
}

void setup() {
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  
  digitalWrite(WRITE_EN, HIGH);
  pinMode(WRITE_EN, OUTPUT);

  Serial.begin(9600);
	delay(1000);
	while (Serial.available() <= 0);
	PARTS_NUMBER = Serial.read();
	for (uint8_t part = 0; part < PARTS_NUMBER; part++) {
  	for (uint16_t i = 0; i < PARTS_SIZE; i++) {
			while (Serial.available() <= 0);
			program[i] = Serial.read();
			Serial.write(1);
		}
		for (uint16_t i = 0; i < PARTS_SIZE; i++)
			Serial.write(program[i]);
		while (Serial.available() <= 0);
		if (Serial.read()) {
			for (uint16_t i = 0; i < PARTS_SIZE; i++) {
				writeAddress(i + PARTS_SIZE * part, program[i]);
				Serial.write(1);
			}
		}
	}
	for (uint16_t i = 0; i < PARTS_SIZE * PARTS_NUMBER; i++)
		Serial.write(readAddress(i));
}

void loop() {
  
}
