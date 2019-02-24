
#include <RCSwitch.h>
#include <SoftwareSerial.h>

#define RX_PIN 11
#define TX_PIN 12

RCSwitch sender = RCSwitch();

void setup() {
  Serial.begin(9600);
  sender.enableTransmit(3);  // On Pin 3
  sender.setProtocol(1);
  sender.setPulseLength(187);

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("Hello.");
}

void switchOn() {
  digitalWrite(LED_BUILTIN, HIGH);
  sender.sendTriState("FFF0FFFF1001");
}

void switchOff() {
  digitalWrite(LED_BUILTIN, LOW);
  sender.sendTriState("FFF0FFFF1010");
}

String txt = "";

void loop() {
  char tmp;
  while (Serial.available() > 0) {
    tmp = Serial.read();
    txt.concat(tmp);

    if (tmp == '\n') {
      Serial.print(txt);
      if (txt.startsWith("on")) {
        Serial.print("Switching on");
        switchOn();
      } else if (txt.startsWith("off")) {
        Serial.print("Switching off");
        switchOff();
      }

      txt = "";
    }
    
  }
}
