#define SHIFT_DATA  2
#define SHIFT_CLK   3
#define SHIFT_LATCH 4
#define WRITE_EN    13

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

  Serial.begin(19200);
  /*writeAddress(0x5555, 0xaa);
  writeAddress(0x2aaa, 0x55);
  writeAddress(0x5555, 0x80);
  writeAddress(0x5555, 0xaa);
  writeAddress(0x2aaa, 0x55);
  writeAddress(0x5555, 0x20);
  writeAddress(0x0000, 0x42);*/
}

void loop() {
  while (Serial.available() <= 2);
  uint8_t action = Serial.read();
  uint16_t address = (Serial.read() << 8) + Serial.read();
  uint8_t value = 0xff;
  switch (action) {
    case 0:
      while (Serial.available() <= 0);
      value = Serial.read();
      writeAddress(address, value);
      Serial.write(value);
      break;
    case 1:
      Serial.write(readAddress(address));
      break;
    default:
      break;
  }
}
