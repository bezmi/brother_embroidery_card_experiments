#include <Arduino.h>
// #include <LittleFS.h>

#include "flashchip.h"
#include <array>

#include "SdFat.h"
// #include "flashserver.h"

// FlashServer flashServer;
FlashChip flashChip;
SdFat SD;

#define BUFFER_SIZE 4096
#define SD_CS_PIN BUILTIN_SDCARD

void writeImage() {

  Serial.println("Writing to chip");

  char buffer[BUFFER_SIZE];
  FsFile uploadedFile = SD.open("image.bin");
  if (uploadedFile) {
    Serial.println("opened image.bin");
    Serial.println("Erasing flash");
    flashChip.eraseAll();
    delay(200);

    Serial.println("Writing file contents to flash... ");
    while (uploadedFile.available()) {
      for (uint32_t addr = 0; addr < FLASH_CHIP_CAPACITY && addr <= uploadedFile.size();
          addr++) {
        if (addr % BUFFER_SIZE == 0) {
          Serial.print("Currently at: ");
          Serial.println(addr);
          uploadedFile.readBytes(buffer, BUFFER_SIZE);
        }
        byte dat = buffer[addr % BUFFER_SIZE];

        if (dat == 0xFF) continue;
        flashChip.writeOneByte(addr, dat);
      }
      Serial.println("Done writing");
    }
  } else {
    Serial.println("Couldn't open file!");
    exit(0);
  }
}

void verifyImage() {

  FsFile uploadedFile = SD.open("image.bin");
  char buffer[BUFFER_SIZE];

  Serial.println("Verifying...");
  // uploadedFile.seek(0);
  for (uint32_t addr = 0; addr < FLASH_CHIP_CAPACITY && addr <= uploadedFile.size();
       addr++) {
    if (addr % BUFFER_SIZE == 0) {
      Serial.print("Currently at: ");
      Serial.println(addr);
      uploadedFile.readBytes(buffer, BUFFER_SIZE);
    }
    byte dat1 = buffer[addr % BUFFER_SIZE];
    byte dat2 = flashChip.readOneByte(addr);

    if (dat1 != dat2) {
      Serial.print("Verification failed at ");
      Serial.print(addr);
      Serial.print(" dat1: ");
      Serial.print(dat1);
      Serial.print(" dat2: ");
      Serial.println(dat2);
      break;
    }
  }
  Serial.println("Done verifying");
}

void setup() {
  delay(1000);
  randomSeed(analogRead(31));
  Serial.begin(115200);
  Serial.println("setup");
  pinMode(13, OUTPUT);
  Serial.print("Initializing SD card...");

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  uint8_t rand_byte = random(0x00, 0xFF);
  Serial.println("initialising flashChip");
  flashChip.init();
  Serial.println("done initialising flashChip");

  Serial.println("getting flash chip information");
  std::array<uint8_t, 2> serialno = flashChip.serialVersion();
  Serial.print("Manufacturer ID returned (expect 0xBF): ");
  Serial.println(serialno[0], HEX);
  Serial.print("Device ID returned (expect 0xB7): ");
  Serial.println(serialno[1], HEX);

  if (serialno[0] != 0xBF || serialno[1] != 0xB7) {
    Serial.println("Unexpected manufacturer or device ID. Can't communicate with the flash.");
    exit(0);
  }


  Serial.println("Ready for command [(W)rite, (V)erify]");
  while (Serial.available() == 0) {}
}


void loop() {
  if (Serial.available() > 0) {
    char inp = Serial.read();
    if (inp == 'W') {
      writeImage();
    } else if (inp == 'V') {
      verifyImage();
    } else if (inp == 'R') {
      Serial.println(flashChip.readOneByte(0x100));
    } else if (inp == 'F') {
      flashChip.writeOneByte(0x100, 'b');
    } else if (inp == 'E') {
      Serial.println("Erasing chip");
      flashChip.eraseAll();
      delay(200);
      Serial.println("Erase completed");

    } else {
      Serial.println("Invalid input");
    }
  }
}
