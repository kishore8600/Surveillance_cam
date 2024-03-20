#include <WiFi.h>
#include <WebSocketsServer.h>

const char* ssid = "Kishore 4G";
const char* password = "Abi@7400";
const int irSensorPin = 4;

WebSocketsServer webSocket = WebSocketsServer(81);

enum State {
  OFF,
  ON
};

State currentState = OFF;
int previousSensorValue = LOW;

void handleWebSocket() {
  webSocket.loop();
}

void sendStatusMessage() {
  if (currentState == ON) {
    webSocket.broadcastTXT("on");
  } else {
    webSocket.broadcastTXT("off");
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_TEXT:
      Serial.printf("[%u] Received data: %s\n", num, payload);
      webSocket.sendTXT(num, "Data received by the server");
      break;
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin((char*)ssid, password); // Cast ssid to char*
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println("IP Address: " + WiFi.localIP().toString());
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  pinMode(irSensorPin, INPUT);
}

void loop() {
  handleWebSocket();
  int currentSensorValue = digitalRead(irSensorPin);
  switch (currentState) {
    case OFF:
      if (currentSensorValue == HIGH && previousSensorValue == LOW) {
        currentState = ON;
        sendStatusMessage();
      }
      break;
    case ON:
      if (currentSensorValue == HIGH && previousSensorValue == LOW) {
        currentState = OFF;
        sendStatusMessage();
      }
      break;
  }
  previousSensorValue = currentSensorValue;
}
