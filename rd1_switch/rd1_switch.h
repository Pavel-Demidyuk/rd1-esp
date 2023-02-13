#define PIN 14
#define POLLING_INTERVAL    2000
#define DS2413_ACCESS_READ  0xF5
#define DS2413_ACCESS_WRITE 0x5A
#define DS2413_ACK_SUCCESS  0xAA
#define DS2413_ACK_ERROR    0xFF

#include "esphome.h"
#include <OneWire.h>

OneWire net(PIN);  // on pin 10

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
    net.reset();
    net.reset_search();  
    delay(10);  
    std::string tmpAddr;
    while(net.search(addr)) {
      delay(10);
      if (OneWire::crc8(addr, 7) != addr[7]) {
        ESP_LOGD("getAcc", "DEVICE IS BROKEN");      
        return false;
      }
      else {
         ESP_LOGI("getAcc", "DEVICE IS OK");     
      }
      return true;
      ESP_LOGD("getAcc", "found some device");      
      tmpAddr = this->getAccAddrStr(addr, 7);
       if (tmpAddr == inputAddr){
          return true;
       }
    }
    return false;
  }

  byte readDS2413(uint8_t* deviceaddress) {
    bool ok = false;
    uint8_t results;
    net.reset();
    net.select(deviceaddress);
    net.write(0xF5);     
    results = net.read();
    ok = (~results & 0x0F) == (results >> 4);
    if (!ok) {
      this->log("NOT OK!!");
    }
    else {
      this->log("OK!!!!");
    }
    results &= 0x0F;                         
    net.reset();
    return results;

    // for (byte i = 0; i < 2; i++) {
    //   data[i] = net.read();
    //   delay(10);
    // }
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
      this->log("______________________________________________________");   
      this->printAddr(addr);
      this->log("read");
      byte data[8];
      int PIO_data = 0;
      PIO_data = this->readDS2413(addr);
      ESP_LOGD("", "RES %d", PIO_data);
      // delay(10);
      // ESP_LOGD("", "PIO0: %d", data[0]);
      // ESP_LOGD("", "PIO1: %d", data[1]);
      // ESP_LOGD("", "PIO2: %d", data[2]);
      // ESP_LOGD("", "PIO3: %d", data[3]);
      // ESP_LOGD("", "PIO4: %d", data[4]);
      // ESP_LOGD("", "PIO5: %d", data[5]);
      // ESP_LOGD("", "PIO6: %d", data[6]);
      // ESP_LOGD("", "PIO7: %d", data[7]);
      // ESP_LOGD("", "PIO8: %d", data[8]);

    //   if ( data[0] == DS2413_ACK_ERROR) {
    //       ESP_LOGD("", "PIOA ERROR");
    //   }
    //   if ( data[1] == DS2413_ACK_ERROR) {
    //       ESP_LOGD("", "PIOB ERROR");
    //   }
    
    //   data[0] ^= 0xFF;
    //   data[1] ^= 0xFF;

    //   this->writeDS2413(addr, data);

    // // // Read back to verify the write
    //   this->readDS2413(addr, data);
    //   ESP_LOGD("TAG", "PIOA: %d", data[0]);
    //   ESP_LOGD("TAG", "PIOB: %d", data[1]);

  
    }    
  }
};