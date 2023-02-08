
class MyCustomSensor : public PollingComponent, public Sensor {
 public:
  // constructor
  MyCustomSensor() : PollingComponent(100) {}
  void setup() override {
    ESP_LOGD("custom", "1111111");
    // This will be called by App.setup()
  }
  void update() override {
    ESP_LOGD("custom", "222222");
    // This will be called every "update_interval" milliseconds.
  }
};