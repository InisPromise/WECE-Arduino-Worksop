//#include <{FILL IN LIBRARY NAME HERE}>

//constexpr char wifiName[] = "{FILL IN WIFI NAME HERE}";
//constexpr char wifiPassword[] = "{FILL IN WIFI PASSWORD HERE}";
constexpr uint16_t serverPort = 5000;
//constexpr uint8_t ledPin = {FILL IN HERE}; //how did previous examples use the built-in LED?
constexpr uint32_t commandTimeoutMs = 500;
constexpr uint32_t reconnectIntervalMs = 1000;

WiFiClient commandClient;
bool ledOn = false;
uint32_t lastCommandMs = 0;
uint32_t lastConnectAttemptMs = 0;

void setLED(bool enabled) {
  ledOn = enabled;
  digitalWrite(ledPin, enabled ? HIGH : LOW);
}

void setup() {
  pinMode(ledPin, OUTPUT);
  setLED(false);
  Serial.begin(115200);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println(F("Wi-Fi module unavailable."));
    while (true) {
      delay(1000);
    }
  }

  Serial.print(F("Connecting to access point"));
  WiFi.begin(wifiName, wifiPassword);

  while (WiFi.status() != WL_CONNECTED) {
    setLED(false);
    delay(500);
    Serial.print('.');
  }

  Serial.println(F("\nWi-Fi connected."));
  Serial.print(F("Gateway address: "));
  Serial.println(WiFi.gatewayIP());
}

void loop() {
  const uint32_t now = millis();

  if (WiFi.status() != WL_CONNECTED) {
    setLED(false);
    commandClient.stop();

    if (now - lastConnectAttemptMs >= reconnectIntervalMs) {
      lastConnectAttemptMs = now;
      WiFi.begin(wifiName, wifiPassword);
    }
    return;
  }

  if (!commandClient.connected() &&
      now - lastConnectAttemptMs >= reconnectIntervalMs) {
    setLED(false);
    commandClient.stop();
    lastConnectAttemptMs = now;

    if (commandClient.connect(WiFi.gatewayIP(), serverPort)) {
      lastCommandMs = now;
      Serial.println(F("Connected to command server."));
    } else {
      Serial.println(F("Server connection failed."));
    }
  }

  while (commandClient.available() > 0) {
    const char command = commandClient.read();

    // Only recognized commands may change the output state.
    if (command == '1' || command == '0') {
      setLED(command == '1');
      lastCommandMs = millis();

      Serial.print(F("Received LED command: "));
      Serial.println(ledOn ? F("ON") : F("OFF"));
    }
  }

  // A lost server connection must not leave the output enabled.
  if (ledOn && millis() - lastCommandMs > commandTimeoutMs) {
    setLED(false);
    Serial.println(F("Command timeout; LED off."));
  }
}