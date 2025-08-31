#include <IRremote.h>

void decodePacket(uint32_t code, uint16_t bits) {
  if (bits != 14 && bits != 24) return;  // проверяем длину пакета

  // Проверка типа пакета
  if (bits == 24) {
    Serial.println("command!");

    uint8_t byte1 = (code >> 16) & 0xFF;    // Старший байт
    uint8_t byte2 = (code >> 8) & 0xFF;           // Младший байт

    Serial.print("Byte 1: 0x");
    Serial.println(byte1, HEX);
    Serial.print("Byte 2: 0x");
    Serial.println(byte2, HEX);

    // TODO расшифровка команд
  } else {
    Serial.println("shoot!");

    uint8_t team = (code >> 4) & 0x3;
    uint8_t dmg = code & 0xF;
    uint8_t id = (code >> 6) & 0x7F;

    Serial.print("ID: ");
    Serial.println(id);
    Serial.print("Team: ");
    Serial.println(team);
    Serial.print("Damage: ");
    Serial.println(dmg);
  }
}

void setup() {
  Serial.begin(115200);
  IrReceiver.begin(2, ENABLE_LED_FEEDBACK);  // Start the receiver
}

void loop() {
  if (IrReceiver.decode()) {
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    IrReceiver.printIRResultShort(&Serial);
    IrReceiver.printIRSendUsage(&Serial);
    IrReceiver.printIRResultRawFormatted(&Serial);

    if (IrReceiver.decodedIRData.protocol == PULSE_WIDTH) {
      uint16_t bits = IrReceiver.decodedIRData.numberOfBits;
      // биты идут в обратном порядке, надо или как-то учитывать это или переворачивать
      // https://github.com/Arduino-IRremote/Arduino-IRremote?tab=readme-ov-file#how-to-convert-old-msb-first-32-bit-ir-data-codes-to-new-lsb-first-32-bit-ir-data-codes
      uint32_t data = IrReceiver.decodedIRData.decodedRawData;
      uint32_t dataBitRev = bitreverse32Bit(data);
      uint32_t dataBitRevFix = dataBitRev >> (32 - bits);

      Serial.println(data, BIN);
      Serial.println(dataBitRev, BIN);
      Serial.println(dataBitRevFix, BIN);

      decodePacket(dataBitRevFix, bits);
    }

    IrReceiver.resume();
  }
}
