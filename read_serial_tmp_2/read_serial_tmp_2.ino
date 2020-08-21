#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); // RX, TX

void setup()
{
  Serial.begin(115200);
  mySerial.begin(115200);
  delay(5000);
  Serial.println("Started!!!!!");
}

void loop() {

  String IncomingString = "";
  boolean StringReady = false;

  while (mySerial.available()) {
    IncomingString = mySerial.readString();
    StringReady = true;
  }

  if (StringReady) {
    Serial.println("Received String: " + IncomingString);
  }

  Serial.println("another loop...");
  delay(100);
}
