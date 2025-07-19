#include <painlessMesh.h>

#define   MESH_PREFIX     "MyMeshNetwork"
#define   MESH_PASSWORD   "MeshPass123"
#define   MESH_PORT       5555

painlessMesh  mesh;

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("Received from %u: %s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("→ New mesh connection, node %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("→ Mesh connections changed. Total nodes: %u\n", mesh.getNodeList().size());
}

void setup() {
  Serial.begin(115200);
  delay(100);

  // Initialize mesh
  mesh.setDebugMsgTypes(ERROR | STARTUP);  // optional: pause other debug msgs
  mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT);
  
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
}

unsigned long lastBroadcast = 0;
void loop() {
  mesh.update();

  // Every 10s, broadcast a heartbeat
  if (millis() - lastBroadcast > 10000) {
    String heartbeat = String("Hello from node ") + mesh.getNodeId();
    mesh.sendBroadcast(heartbeat);
    lastBroadcast = millis();
  }
}
