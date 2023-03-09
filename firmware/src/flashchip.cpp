#include "flashchip.h"

void FlashChip::init() {
  pinMode(CE, OUTPUT);
  pinMode(OE, OUTPUT);
  pinMode(WE, OUTPUT);

  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(A6, OUTPUT);
  pinMode(A7, OUTPUT);
  pinMode(A8, OUTPUT);
  pinMode(A9, OUTPUT);
  pinMode(A10, OUTPUT);
  pinMode(A11, OUTPUT);
  pinMode(A12, OUTPUT);
  pinMode(A13, OUTPUT);
  pinMode(A14, OUTPUT);
  pinMode(A15, OUTPUT);
  pinMode(A16, OUTPUT);
  pinMode(A17, OUTPUT);
  pinMode(A18, OUTPUT);


  digitalWrite(CE, HIGH);
  digitalWrite(WE, HIGH);
  digitalWrite(OE, HIGH);
  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
  digitalWrite(A4, LOW);
  digitalWrite(A5, LOW);
  digitalWrite(A6, LOW);
  digitalWrite(A7, LOW);
  digitalWrite(A8, LOW);
  digitalWrite(A9, LOW);
  digitalWrite(A10, LOW);
  digitalWrite(A11, LOW);
  digitalWrite(A12, LOW);
  digitalWrite(A13, LOW);
  digitalWrite(A14, LOW);
  digitalWrite(A15, LOW);
  digitalWrite(A16, LOW);
  digitalWrite(A17, LOW);
  digitalWrite(A18, LOW);

  // digitalWrite(Q0, LOW);
  // digitalWrite(Q1, LOW);
  // digitalWrite(Q2, LOW);
  // digitalWrite(Q3, LOW);
  // digitalWrite(Q4, LOW);
  // digitalWrite(Q5, LOW);
  // digitalWrite(Q6, LOW);
  // digitalWrite(Q7, LOW);

  // for (int i = 0; i <= MCP23016_PIN_GPIO1_7; i++) {
  //   while (!mcp.detected()) {
  //     Serial.println("Waiting for MCP");
  //     delay(100);
  //   }
  //   // mcp.pinMode(i, OUTPUT);
  // }

  // while (!mcp.detected()) {
  //   Serial.println("Waiting for MCP");
  //   delay(100);
  // }
  DATA_MODE(INPUT);
}

void FlashChip::DATA_MODE(uint8_t mod)
{                        
  if (dataState != mod) {
    dataState = mod;     
    for (uint8_t addr = 0; addr < DATA_ADDRESSES_SIZE; addr++) {
      pinMode(data_addresses[addr], mod);                       
    }                                                           
  }                                                             
}

void FlashChip::setDataLines(uint8_t data) {
  for (uint8_t bit = 0; bit < DATA_ADDRESSES_SIZE; bit++) {

    digitalWrite(data_addresses[bit], (data >> bit) & 1);
  }
}

void FlashChip::setAddressLines(uint32_t addr)  {
  digitalWrite(A0,  addr >> 0  & 1);
  digitalWrite(A1,  addr >> 1  & 1);
  digitalWrite(A2,  addr >> 2  & 1);
  digitalWrite(A3,  addr >> 3  & 1);
  digitalWrite(A4,  addr >> 4  & 1);
  digitalWrite(A5,  addr >> 5  & 1);
  digitalWrite(A6,  addr >> 6  & 1);
  digitalWrite(A7,  addr >> 7  & 1);
  digitalWrite(A8,  addr >> 8  & 1);
  digitalWrite(A9,  addr >> 9  & 1);
  digitalWrite(A10, addr >> 10 & 1);
  digitalWrite(A11, addr >> 11 & 1);
  digitalWrite(A12, addr >> 12 & 1);
  digitalWrite(A13, addr >> 13 & 1);
  digitalWrite(A14, addr >> 14 & 1);
  digitalWrite(A15, addr >> 15 & 1);
  digitalWrite(A16, addr >> 16 & 1);
  digitalWrite(A17, addr >> 17 & 1);
  digitalWrite(A18, addr >> 18 & 1);
}

void FlashChip::sendCmd(uint32_t addr, uint8_t dat) {
  // digitalWrite(CE, LOW);
  // digitalWrite(OE, HIGH);
  // digitalWrite(WE, HIGH);
  DATA_MODE(OUTPUT);
  setAddressLines(addr);
  setDataLines(dat);

  digitalWrite(CE, LOW);
  digitalWrite(WE, LOW);
  delayNanoseconds(1); // TODO: fix this based on actual values
  digitalWrite(WE, HIGH);
  digitalWrite(CE, HIGH);
  DATA_MODE(INPUT);
}

void FlashChip::writeOneByte(uint32_t addr, uint8_t dat) {
  if (addr >= FLASH_CHIP_CAPACITY) {
    Serial.println("Address exceeds flash chip capacity");
    return;
  }
  sendCmd(0x5555, 0xAA);
  sendCmd(0x2AAA, 0x55);
  sendCmd(0x5555, 0xA0);
  sendCmd(addr, dat);

  uint8_t old_val;
  uint8_t val;
  while (true) {
    old_val = readOneByte(addr);
    val = readOneByte(addr);

    if (((val >> 6) & 0x01) == ((old_val >> 6) & 0x01))
      break;
  }
  // digitalWrite(CE, HIGH);
  // digitalWrite(OE, LOW);
  // delayMicroseconds(20);
}

uint8_t FlashChip::readOneByte(uint32_t addr) {
  if (addr >= FLASH_CHIP_CAPACITY) {
    Serial.println("Address exceeds flash chip capacity");
    return 0;
  }
  DATA_MODE(INPUT);
  setAddressLines(addr);
  digitalWrite(CE, LOW);
  digitalWrite(OE, LOW);

  uint8_t dat = 0;
  for (uint8_t bit = 0; bit < DATA_ADDRESSES_SIZE; bit++) {
    dat |= digitalRead(data_addresses[bit]) << bit;
    delayNanoseconds(50);
  }

  digitalWrite(CE, HIGH);
  digitalWrite(OE, HIGH);

  return dat;
}

void FlashChip::eraseAll() {
  sendCmd(0x5555, 0xAA);
  sendCmd(0x2AAA, 0x55);
  sendCmd(0x5555, 0x80);
  sendCmd(0x5555, 0xAA);
  sendCmd(0x2AAA, 0x55);
  sendCmd(0x5555, 0x10);
  delay(200);
}

std::array<uint8_t,2> FlashChip::serialVersion() {
  sendCmd(0x5555, 0xAA);
  sendCmd(0x2AAA, 0x55);
  sendCmd(0x5555, 0x90);

  std::array<uint8_t, 2> version;
  version[0] = readOneByte(0);
  version[1] = readOneByte(1);

  sendCmd(0x5555, 0xAA);
  sendCmd(0x2AAA, 0x55);
  sendCmd(0x5555, 0xF0);

  return version;

}