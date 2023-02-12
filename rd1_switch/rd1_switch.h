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

    std::transform(res.begin(), res.end(), res.begin(), to_lower);

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
  bool findAcc(std::string inputAddr, byte addr[8]) {
    net.reset_search();    
    std::string tmpAddr;
    while(net.search(addr)) {
      ESP_LOGD("getAcc", "found some device");      
      tmpAddr = this->getAccAddrStr(addr, 7);
       if (tmpAddr == inputAddr){
          return true;
       }
    }
    return false;
  }

  void readDS2413(byte addr[8], byte data[2]) {
    net.reset();
    net.select(addr);
    net.write(0xF5, 1); // Read PIO Registers command
    for (byte i = 0; i < 2; i++) {
      data[i] = net.read();
    }
  }

  void writeDS2413(byte addr[8], byte data[2]) {
    net.reset();
    net.select(addr);
    net.write(0x5A, 1); // Write to PIO Registers command
    net.write(data[0], 1);
    net.write(data[1], 1);
    delay(10);
  }

  int i = 0;

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
    publish_state(state);
  }

  void update() override {
    this->i++;
    ESP_LOGD("", "%d", this->i);
    byte addr[8];  
    if (!this->findAcc(this->sensorAddr, addr)) {
      ESP_LOGE("", "Can't find sensor by addr %s", this->sensorAddr);
      return;
    }
    if (this->i >= 5){
      if (this->i %2 == 0) {
        this->log("read");
        byte data[2];
        this->readDS2413(addr,data);
        ESP_LOGD("", "PIOA: %d", data[0]);
        ESP_LOGD("", "PIOB: %d", data[1]);
      }
      // else {
      //   this.write(addr, val)
      // }     
    }    
  }
};