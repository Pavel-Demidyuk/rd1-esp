
#define DS2413_ONEWIRE_PIN (8)
#define DS2413_FAMILY_ID 0x3A
#define DS2413_ACCESS_READ 0xF5
#define DS2413_ACCESS_WRITE 0x5A
#define DS2413_ACK_SUCCESS 0xAA
#define DS2413_ACK_ERROR 0xFF
OneWire oneWire(DS2413_ONEWIRE_PIN);
uint8_t address[8] = {0, 0, 0, 0, 0, 0, 0, 0};
void printBytes(uint8_t *addr, uint8_t count, bool newline = 0)
{
    for (uint8_t i = 0; i < count; i++)
    {
        ESP_LOGD("custom", "!!!!!!!!!!!!");
        char buffer1[20];
        char buffer2[20];
        sprintf(buffer1, "%02X", addr[i] >> 4);
        sprintf(buffer2, "%02X", addr[i] & 0x0f);
        ESP_LOGD("custom", buffer1);
        ESP_LOGD("custom", buffer2);
        // Serial.print(addr[i]>>4, HEX);
        // Serial.print(addr[i]&0x0f, HEX);
        // Serial.print(" ");
    }
    if (newline)
    {
        // Serial.println();
    }
}

byte read(void)
{
    bool ok = false;
    uint8_t results;

    oneWire.reset();
    oneWire.select(address);
    oneWire.write(DS2413_ACCESS_READ);

    results = oneWire.read();                 /* Get the register results   */
    ok = (!results & 0x0F) == (results >> 4); /* Compare nibbles            */
    results &= 0x0F;                          /* Clear inverted values      */

    oneWire.reset();

    // return ok ? results : -1;
    return results;
}

bool write(uint8_t state)
{
    uint8_t ack = 0;

    /* Top six bits must '1' */
    state |= 0xFC;

    oneWire.reset();
    oneWire.select(address);
    oneWire.write(DS2413_ACCESS_WRITE);
    oneWire.write(state);
    oneWire.write(~state); /* Invert data and resend     */
    ack = oneWire.read();  /* 0xAA=success, 0xFF=failure */
    if (ack == DS2413_ACK_SUCCESS)
    {
        oneWire.read(); /* Read the status byte      */
    }
    oneWire.reset();

    return (ack == DS2413_ACK_SUCCESS ? true : false);
}

class MyCustomSensor : public PollingComponent, public Sensor
{
public:
    // constructor
    MyCustomSensor() : PollingComponent(3000) {}

    void setup() override
    {
        // This will be called by App.setup()

        /* Try to find a device on the bus */
        oneWire.reset_search();
        delay(250);
        if (!oneWire.search(address))
        {
            printBytes(address, 8);
            ESP_LOGD("custom", "No device found on the bus!");
            oneWire.reset_search();
            while (1)
                ;
        }

        /* Check the CRC in the device address */
        if (OneWire::crc8(address, 7) != address[7])
        {
            ESP_LOGD("custom", "Invalid CRC!");
            while (1)
                ;
        }

        /* Make sure we have a DS2413 */
        if (address[0] != DS2413_FAMILY_ID)
        {
            printBytes(address, 8);
            ESP_LOGD("custom", " is not a DS2413!");
            while (1)
                ;
        }

        ESP_LOGD("custom", "Found a DS2413: ");
        printBytes(address, 8);
        ESP_LOGD("custom", "");
    }
    void update() override
    {
        // OneWire  ds(21);  // on pin 10
        publish_state(42.0);
        ESP_LOGD("custom", "This is a custom debug message");

        // byte i;
        // byte present = 0;
        // byte data[12];
        // byte addr[8];

        // if ( !ds.search(addr)) {
        //   ESP_LOGD("custom","No more addresses.\n");
        //   ds.reset_search();
        //   delay(250);
        //   return;
        // }
        // ESP_LOGD("custom", "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

        // ESP_LOGD("custom", "R=");
        // for( i = 0; i < 8; i++) {
        //   // ESP_LOGD("custom",addr[i], HEX);
        //   ESP_LOGD("custom","addr[i], HEX");
        //   ESP_LOGD("custom"," ");
        // }

        // if ( OneWire::crc8( addr, 7) != addr[7]) {
        //     ESP_LOGD("custom","CRC is not valid!\n");
        //     return;
        // }

        // if ( addr[0] != 0x10) {
        //     ESP_LOGD("custom","Device is not a DS18S20 family device.\n");
        //     return;
        // }

        // // The DallasTemperature library can do all this work for you!

        // ds.reset();
        // ds.select(addr);
        // ds.write(0x44,1);         // start conversion, with parasite power on at the end

        // delay(1000);     // maybe 750ms is enough, maybe not
        // // we might do a ds.depower() here, but the reset will take care of it.

        // present = ds.reset();
        // ds.select(addr);
        // ds.write(0xBE);         // Read Scratchpad

        // ESP_LOGD("custom","P=");
        // ESP_LOGD("custom","present,HEX");
        // ESP_LOGD("custom"," ");
        // for ( i = 0; i < 9; i++) {           // we need 9 bytes
        //   data[i] = ds.read();
        //   ESP_LOGD("custom","data[i], HEX");
        //   ESP_LOGD("custom"," ");
        // }
        // ESP_LOGD("custom"," CRC=");
        // ESP_LOGD("custom", "OneWire::crc8( data, 8), HEX");
        // ESP_LOGD("custom","\n");
    }
};