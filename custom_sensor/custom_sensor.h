#include "esphome.h"
#include <OneWire.h>
#define POLLING_INTERVAL 2500

OneWire net(12);  // on pin 10

void PrintBytes(const uint8_t* addr, uint8_t count, bool newline=false) {
  String res = "";
  // String hexString2 = "";
  for (uint8_t i = 0; i < count; i++) {
    res += String(addr[i]>>4, HEX) + String(addr[i]&0x0f, HEX);
    // hexString2 += String(addr[i]&0x0f, HEX);
     
    // hexString1 = String(addr[i]>>4, HEX);
    // hexString2 = String(addr[i]&0x0f, HEX);
    // ESP_LOGD("", hexString1.c_str());
    // ESP_LOGD("", hexString2.c_str());
     
  }
    ESP_LOGD("", res.c_str());
    // ESP_LOGD("", hexString2.c_str());
  // if (newline)
  //   Serial.println();
}

void lookWire() {
   byte addr[8];

  if (!net.search(addr)) {
    ESP_LOGD("custom", "No more addresses.\n");
    net.reset_search();
    // delay(1000);
    return;
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
    ESP_LOGD("custom", "CRC is not valid!\n");
    return;
  }

  if (addr[0] != 0x29) {
    PrintBytes(addr, 8);
    ESP_LOGD("custom", " is not a DS2408.\n");
    return;
  }
  ESP_LOGD("custom", "  Reading DS2408 ");
  PrintBytes(addr, 8);
  Serial.println();

  uint8_t buf[13];  // Put everything in the buffer so we can compute CRC easily.
  buf[0] = 0xF0;    // Read PIO Registers
  buf[1] = 0x88;    // LSB address
  buf[2] = 0x00;    // MSB address
  net.write_bytes(buf, 3);
  net.read_bytes(buf+3, 10);     // 3 cmd bytes, 6 data bytes, 2 0xFF, 2 CRC16
  net.reset();

  if (!OneWire::check_crc16(buf, 11, &buf[11])) {
    ESP_LOGD("custom", "CRC failure in DS2408 at ");
    PrintBytes(addr, 8, true);
    return;
  }
  ESP_LOGD("custom", "  DS2408 data = ");
  // First 3 bytes contain command, register address.
  // ESP_LOGD("custom", "  DS2408 data = ");
  Serial.println(buf[3], BIN);
}

class MyCustomSensor : public PollingComponent, public Sensor {
 public:
  // constructor
  MyCustomSensor() : PollingComponent(POLLING_INTERVAL) {}

  float get_setup_priority() const override { return esphome::setup_priority::HARDWARE; }

  void setup() override {
    ESP_LOGD("custom", "1111111");
    // This will be called by App.setup()
  }
  void update() override {
    lookWire();
    ESP_LOGD("custom", "222222");
    // This will be called every "update_interval" milliseconds.
  }
};