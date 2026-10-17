//#include <{FILL IN LIBRARY NAME HERE}>

//constexpr char wifiName[] = "{FILL IN WIFI NAME HERE}";
//constexpr char wifiPassword[] = "{FILL IN WIFI PASSWORD HERE}";
constexpr uint16_t serverPort = 5000;
constexpr uint8_t buttonPin = 2;
constexpr uint32_t heartbeatMs = 100;

WiFiServer commandServer(serverPort);
WiFiClient commandClient;

bool previousButtonState = false;
uint32_t previousSendMs = 0;

void setup() {
  pinMode(buttonPin, //{FILL IN});
  Serial.begin(115200);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println(F("Wi-Fi module unavailable."));
    while (true) {
      delay(1000);
    }
  }

  if (WiFi.beginAP(wifiName, wifiPassword) != WL_AP_LISTENING) {
    Serial.println(F("Could not create access point."));
    while (true) {
      delay(1000);
    }
  }

  commandServer.begin();

  Serial.print(F("Server address: "));
  Serial.println(WiFi.localIP());
  Serial.println(F("Waiting for TCP client..."));
}

void loop() {
  const uint32_t now = millis();
  const bool buttonPressed = (digitalRead(buttonPin) == HIGH);

  if (!commandClient.connected()) {
    commandClient.stop();
    commandClient = commandServer.accept();

    if (commandClient) {
      Serial.println(F("TCP client connected."));
      previousSendMs = 0;  // A new client must receive the current state promptly.
    }
  }

  if (commandClient.connected() &&
      (buttonPressed != previousButtonState ||
       now - previousSendMs >= heartbeatMs)) {
    const char command = buttonPressed ? '1' : '0';
    commandClient.write(static_cast<uint8_t>(command));

    Serial.print(F("Sending LED command: "));
    Serial.println(buttonPressed ? F("ON") : F("OFF"));

    previousSendMs = now;
  }

  previousButtonState = buttonPressed;
}