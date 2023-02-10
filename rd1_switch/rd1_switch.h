#include "esphome.h"
#include <OneWire.h>
#define POLLING_INTERVAL    5000
#define DS2413_ACCESS_READ  0xF5
#define DS2413_ACCESS_WRITE 0x5A
#define DS2413_ACK_SUCCESS  0xAA
#define DS2413_ACK_ERROR    0xFF

OneWire net(12);  // on pin 10

char to_lower(char c)
{
    return tolower(c);
}
// using namespace std;


// void lookWire() {
//    byte addr[8];

//   if (!net.search(addr)) {
//     ESP_LOGD("custom", "No more addresses.\n");
//     net.reset_search();
//     return;
//   }

//   if (OneWire::crc8(addr, 7) != addr[7]) {
//     ESP_LOGD("custom", "CRC is not valid!\n");
//     return;
//   }

//   if (addr[0] != 0x29) {
//     PrintBytes(addr, 8);
//     ESP_LOGD("custom", " is not a DS2408.\n");
//     return;
//   }
//   ESP_LOGD("custom", "  Reading DS2408 ");
//   PrintBytes(addr, 8);
//   Serial.println();

//   uint8_t buf[13];  // Put everything in the buffer so we can compute CRC easily.
//   buf[0] = 0xF0;    // Read PIO Registers
//   buf[1] = 0x88;    // LSB address
//   buf[2] = 0x00;    // MSB address
//   net.write_bytes(buf, 3);
//   net.read_bytes(buf+3, 10);     // 3 cmd bytes, 6 data bytes, 2 0xFF, 2 CRC16
//   net.reset();

//   if (!OneWire::check_crc16(buf, 11, &buf[11])) {
//     ESP_LOGD("custom", "CRC failure in DS2408 at ");
//     PrintBytes(addr, 8, true);
//     return;
//   }
//   ESP_LOGD("custom", "  DS2408 data = ");
//   // First 3 bytes contain command, register address.
//   // ESP_LOGD("custom", "  DS2408 data = ");
//   Serial.println(buf[3], BIN);
// }

class Rd1Switch : public PollingComponent, public Switch {
 private:
  std::string switchAddr;
  std::string sensorAddr;
  char switchPio;
  char sensorPio;
  bool bis_config;
  bool invert_config;

  void log (std::string message) {
    ESP_LOGD("", message.c_str());
  }
  
  std::string getAccAddrStr(const uint8_t* addr, const uint8_t count) {
    std::string res = "";
    const char hex_lookup[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    for (uint8_t i = 0; i < count; i++) {
      res += hex_lookup[addr[i] >> 4];
      res += hex_lookup[addr[i] & 0x0f];
    }

    // std::transform(switch_addr.begin(), switch_addr.end(), switch_addr.begin(), to_lower);

    return res;     
  }

  /*
    Print address of found accessory
  */
  void printAddr(const uint8_t* addr) {
    ESP_LOGD("PRINTING ADDRESS", this->getAccAddrStr(addr, 7).c_str());
  }

  /*
    Getting accessory by string address
  */
  byte* findAcc(std::string inputAddr) {
    this->log("Looking for match");
    byte addr[8];    
    net.reset_search();    
    std::string tmpAddr;
    while(net.search(addr)) {
      ESP_LOGD("getAcc", "found some device");      
      tmpAddr = this->getAccAddrStr(addr, 7);
      
       if (tmpAddr == inputAddr){
          ESP_LOGD("", "!!!!!!!Found device by addr: !!!!!!!");
          // ESP_LOGD("", inputAddr.c_str());
          return addr;
       }
       else {
           ESP_LOGD("", tmpAddr.c_str());
       }
      
    }
     return addr;
    
    ESP_LOGD("", "Can't find device by address: ");
    ESP_LOGD("", inputAddr.c_str());
    // while (1);
  }

 public:
  Rd1Switch(std::string switch_addr, std::string sensor_addr) : PollingComponent(POLLING_INTERVAL) {
    std::transform(switch_addr.begin(), switch_addr.end(), switch_addr.begin(), to_lower);
    switch_addr.erase(std::remove(switch_addr.begin(), switch_addr.end(), '.'), switch_addr.end());
    std::transform(sensor_addr.begin(), sensor_addr.end(), sensor_addr.begin(), to_lower);
    sensor_addr.erase(std::remove(sensor_addr.begin(), sensor_addr.end(), '.'), sensor_addr.end());

    this->switchPio = switch_addr[switch_addr.length() - 1];
    this->sensorPio = sensor_addr[sensor_addr.length() - 1];
    switch_addr.erase(switch_addr.length() - 2, 2);
    sensor_addr.erase(sensor_addr.length() - 2, 2);
    
    this->switchAddr = switch_addr;
    this->sensorAddr = sensor_addr;
  }
  
  // float get_setup_priority() const override { return esphome::setup_priority::HARDWARE; }

  void bis (bool val) {
    this->bis_config = val;
  }

  void invert (bool val) {
    this->invert_config = val;
  }

  void setup() override {
    
  }
  void write_state(bool state) override {
    ESP_LOGD("custom", "!!!!!!!!!!!!!!!!!!write!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    // This will be called every time the user requests a state change.

    // digitalWrite(5, state);

    // Acknowledge new state by publishing it
    // publish_state(state);
  }
  void update() override {
    // return;
    ESP_LOGD("", "update");
    ESP_LOGD("TAG", "Sensor addr: %s", this->sensorAddr.c_str());
    byte* result = this->findAcc(this->sensorAddr);
    // printAddr(result);




    // this->log(this->convertInputAddr(this->switch_addr));
    // this->printAddr(this->switch_addr);
    // 12.9506E1000000
    // 129506e100000081
    // 129506E1000000
    // 129506e1000000
    // byte addr[8];
    // net.reset_search();
    // while(net.search(addr)) {
    //    if ()getAccAddr(addr, 7);
    // }

    // if (!net.search(addr)) {
    //   ESP_LOGD("custom", "No more addresses.\n");
    //   net.reset_search();
    //   return;
    // }

    // if (OneWire::crc8(addr, 7) != addr[7]) {
    //   ESP_LOGD("custom", "CRC is not valid!\n");
    //   return;
    // }

    // if (addr[0] != 0x29) {
    //   PrintBytes(addr, 8);
    //   ESP_LOGD("custom", " is not a DS2408.\n");
    //   return;
    // }
    // ESP_LOGD("custom", "  Reading DS2408 ");
    // PrintBytes(addr, 7);
    // Serial.println();
  }
};



// class Rd1Switch : public Component, public Switch {
//  public:
//   // constructor
//   Rd1Switch() : PollingComponent(POLLING_INTERVAL) {}

//   float get_setup_priority() const override { return esphome::setup_priority::HARDWARE; }

//   void setup() override {
//     ESP_LOGD("custom", "1111111");
//     // This will be called by App.setup()
//   }
//   void update() override {
//     lookWire();
//     ESP_LOGD("custom", "222222");
//     // This will be called every "update_interval" milliseconds.
//   }
// };