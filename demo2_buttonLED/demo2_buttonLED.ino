constexpr uint8_t buttonPin = 2; //this tells you how to physically set up the board and button

void setup() {
  pinMode(buttonPin, //{FILL IN HERE});
  pinMode(LED_BUILTIN, //{FILL IN HERE});
}

void loop() {
  const bool buttonPressed = (digitalRead(buttonPin) == //{FILL IN HERE});
  // What happens if you write "LOW" in the blank? what about "HIGH"? What is the difference in the physical world?
  if (buttonPressed){ 
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else{ //turn off if button is not pressed
    digitalWrite(LED_BUILTIN, LOW);
  }
}