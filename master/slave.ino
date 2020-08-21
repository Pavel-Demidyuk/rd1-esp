// TODO remove unnecessary Serial print
#include "painlessMesh.h"
#include <ArduinoJson.h>
#include <iostream>
#include <EEPROM.h>

#define   MESH_PREFIX       "whateverYouLike"
#define   MESH_PASSWORD     "somethingSneaky"
#define   MESH_PORT         5555
#define   PING_INTERVAL     60
#define   CURRENT_STATE_PIN 4
#define   POWER_PIN         5
#define   LAST_UPDATED_ADDR 6

// StaticJsonDocument<256> output;
// StaticJsonDocument<256> input;


Scheduler userScheduler; // to control your personal task
painlessMesh mesh;

void sendPingMessage();

Task taskPing(TASK_SECOND *

PING_INTERVAL , TASK_FOREVER, &sendPingMessage );

void sendPingMessage() {
    mesh.sendBroadcast(generateMessage());
}

String generateMessage() {
    StaticJsonDocument<256> outputJson;
    outputJson["i"] = mesh.getNodeId();; // id
    outputJson["t"] = mesh.getNodeTime();
    outputJson["s"] = getCurrentState(); // state
    String output;
    serializeJson(outputJson, output);
    return output;
}

bool getCurrentState() {
    return digitalRead(CURRENT_STATE_PIN) == HIGH;
}

int receivedCallback(uint32_t from, String &msg) {
    DeserializationError error = deserializeJson(input, msg.c_str());
    Serial.printf("\n\n Incoming message: %s \n", msg.c_str());
    if (error) {
        Serial.println(error.c_str());
        return 1;
    }
    JsonArray inputJson = input.to<JsonArray>();

    if (isForCurrentNode(inputJson) && shouldUpdate(inputJson)) {
        updateNodeState(inputJson["s"], inputJson["t"]);
    } else {
        Serial.println("Message is not for current node or state don't need to be changed");
        Serial.printf("Is for this node %d, shouldUpdate %d \n\n", isForCurrentNode(inputJson), shouldUpdate(inputJson));
        // @TODO should we even broadcast it
        //broadcastMessage(message)
    }
    return 0;
}

bool shouldUpdate(JsonArray inputJson) {
    return (
            inputJson["t"] > getLastUpdatedTime() &&
            inputJson["s"] != isOn() // check that status really changed
    );
}

bool isOn() {
//     Serial.println("Reading PIN");
//     Serial.println(digitalRead(CURRENT_STATE_PIN));
//     Serial.println(digitalRead(CURRENT_STATE_PIN) == LOW);
    return digitalRead(CURRENT_STATE_PIN) == HIGH;
}

int getLastUpdatedTime() {
    int t;
    EEPROM.get(LAST_UPDATED_ADDR, t);
    Serial.println("Last upadate time");
    Serial.println(t);
//     Serial.print("Last updated time");
//     Serial.println(time);
    return t;
}

void setLastUpdatedTime(int t) {
    Serial.print("\n\n\n--------------------------------SETTING TIME!!!!---------------------------------------------\n\n\n");
    Serial.print(t);
    EEPROM.put(LAST_UPDATED_ADDR, t);
}

// void updateNodeState(JsonObject inputJson) {
//     // if (message)
//     // setLastUpdatedTime(message.t);
// }

bool isForCurrentNode(JsonArray inputJson) {
//     @TODO remove debug code
    return true;
//     return inputJson["id"] == mesh.getNodeId();
}

void updateNodeState(bool state, int t) {
    Serial.println("******** UPDATING NODE State*************");
    Serial.println(state);
    Serial.println("Timestamp");
    Serial.println(t);

    if (getCurrentState() == state) {
        return; // no need to update
    }


    int currentPowerPin = (int) digitalRead(POWER_PIN) == HIGH;
    digitalWrite(POWER_PIN, ((1 - currentPowerPin) == 1) ? HIGH : LOW);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
    Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void meshInit() {
    mesh.setDebugMsgTypes(ERROR | STARTUP);  // set before init() so that you can see startup messages
    mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
}

void tasksInit() {
    userScheduler.addTask(taskPing);
    taskPing.enable();
}

void gpioInit() {
    pinMode(CURRENT_STATE_PIN, INPUT);
    pinMode(POWER_PIN, OUTPUT);
}

void beginHArdware() {
    Serial.begin(115200);
    EEPROM.begin(2048);
}

void setup() {
    beginHardware();
    meshInit();
    tasksInit();
    gpioInit();
    EEPROM.put(LAST_UPDATED_ADDR, 0);
    Serial.println(mesh.getNodeId());
}

void loop() {
    getLastUpdatedTime();
//     Serial.Println("Directly to serial");
//     EEPROM.get(LAST_UPDATED_ADDR, Serial)
//     delay(1000);
    // it will run the user scheduler as well
   mesh.update();
}


