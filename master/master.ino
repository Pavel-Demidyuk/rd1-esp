//************************************************************
// this is a simple example that uses the painlessMesh library
//
// 1. sends a silly message to every node on the mesh at a random time between 1 and 5 seconds
// 2. prints anything it receives to Serial.print
//
//
//************************************************************
#include "painlessMesh.h"
#include <ArduinoJson.h>
#include <iostream>

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
painlessMesh mesh;

String inputString = "";         // a String to hold incoming data

Task taskPing(TASK_SECOND *PING_INTERVAL, TASK_FOREVER, &

sendPingMessage );

void sendPingMessage() {
    mesh.sendBroadcast(generateMessage());
}

// User stub
void sendMessage(); // Prototype so PlatformIO doesn't complain

void sendMessage(String message) {
    //   String msg = "Hello from node ";
    //   msg += mesh.getNodeId();
    mesh.sendBroadcast(message);
    //   taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}

// Needed for painless library
int receivedCallback(uint32_t from, String &msg) {
    toRpi(msg.c_str());
    //Serial.printf("!!!! id=%s rid=%s state=%b \n", id, rid, state);
    return 0;
}

void toRpi(String message) {
    Serial.print(message);
}

void toMesh() {

}

void newConnectionCallback(uint32_t nodeId) {
    //   Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
    // Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    // Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
    Serial.begin(115200);

    //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
    mesh.setDebugMsgTypes(ERROR | STARTUP);  // set before init() so that you can see startup messages

    mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

    //  userScheduler.addTask( taskSendMessage );
    //taskSendMessage.enable();
}

void loop() {
    mesh.update();

    while (Serial.available() > 0) {
        char received = Serial.read();
        inputString += received;

        // Process message when new line character is recieved
        if (received == '\n') {
            sendMessage(inputString);
        }
    }
    inputString = "";
}
