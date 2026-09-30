#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "secrets.h"

// ----------- WiFi -----------
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

WebServer server(80);

// ----------- Motor Pins (L298N) -----------
int IN1 = 4;
int IN2 = 13;
int IN3 = 14;
int IN4 = 15;

// ----------- Motor control -----------
void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void right() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// ----------- Web page  -----------
const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name='viewport' content='width=device-width, initial-scale=1.0'>
<title>ESP32 Car Control</title>
<style>
body { background:#111; color:white; text-align:center; font-family:Arial; }
button {
  width:120px; height:120px; font-size:30px;
  margin:10px; background:#222; color:#0f0;
  border:3px solid #0f0; border-radius:20px;
}
button:active { background:#0f0; color:black; }
.stop-btn {
  background:red; color:white; border-color:red;
}
</style>
</head>
<body>

<h2>🚗 ESP32 CAR CONTROL</h2>

<div>
  <button onmousedown="send('forward')" onmouseup="send('stop')">▲</button>
</div>

<div>
  <button onmousedown="send('left')" onmouseup="send('stop')">◄</button>
  <button onmousedown="send('right')" onmouseup="send('stop')">►</button>
</div>

<div>
  <button onmousedown="send('backward')" onmouseup="send('stop')">▼</button>
</div>

<!-- زر توقف مستقل -->
<div>
  <button class="stop-btn" onclick="send('stop')">⏹️ STOP</button>
</div>

<script>
function send(cmd){
  fetch('/' + cmd);
}
</script>

</body>
</html>
)rawliteral";

// ----------- HTTP Handlers -----------
void handleRoot() { server.send(200, "text/html", webpage); }
void handleForward() { forward(); server.send(200, "text/plain", "OK"); }
void handleBackward() { backward(); server.send(200, "text/plain", "OK"); }
void handleLeft() { left(); server.send(200, "text/plain", "OK"); }
void handleRight() { right(); server.send(200, "text/plain", "OK"); }
void handleStop() { stopCar(); server.send(200, "text/plain", "OK"); }

// ----------- Setup -----------
void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopCar();

  WiFi.begin(ssid, password);
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // mDNS
  if (MDNS.begin("car")) {
    Serial.println("mDNS active: http://car.local/");
  }

  // Routes
  server.on("/", handleRoot);
  server.on("/forward", handleForward);
  server.on("/backward", handleBackward);
  server.on("/left", handleLeft);
  server.on("/right", handleRight);
  server.on("/stop", handleStop);

  server.begin();
  Serial.println("HTTP server started");
}

// ----------- Loop -----------
void loop() {
  server.handleClient();
}
