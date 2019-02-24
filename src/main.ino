
#include <RCSwitch.h>
#include <SoftwareSerial.h>

#define RX_PIN 11
#define TX_PIN 12
#define DEVICE_NAME "passytx"

RCSwitch sender = RCSwitch();
SoftwareSerial HC05(TX_PIN, RX_PIN);

void setup() {
  Serial.begin(9600);
  sender.enableTransmit(3);  // On Pin 3
  sender.setProtocol(1);
  sender.setPulseLength(187);

  pinMode(LED_BUILTIN, OUTPUT);

  initHC05();
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

String reqHC05(String cmd) {
  char c;
  String res = "";

  Serial.println("Sending HC-05 cmd " + cmd);
  HC05.println(cmd);

  // I'm told that this is necessary.
  delay(500);

  while (HC05.available() > 0) {
    c = HC05.read();
    res.concat(c);
  }

  return res;
}

void initHC05() {
  String txt;
  Serial.println("Configuring HJC-05 module ...");
  HC05.begin(38400);

  // Reset command. Follower mode, 38400 baud, pw 1234, device name: "hc01.com HC-05"
  txt = reqHC05("AT+ORGL");
  if (!txt.startsWith("OK")) {
    Serial.println("HC-05 initialization error: " + txt);
    // What else am I supposed to do here?
    while(1);
  }

  // Remove paired devices.
  txt = reqHC05("AT+RMAAD");
  if (!txt.startsWith("OK")) {
    Serial.println("Failed to clean pairing list: " + txt);
    // I'm so sorry.
    while(1);
  }

  txt = reqHC05("AT+NAME=" DEVICE_NAME);
  if (!txt.startsWith("OK")) {
    Serial.println("Failed to set device name: " + txt);
    // Ugh.
    while(1);
  }

  txt = reqHC05("AT+UART?");
  if (!txt.startsWith("+UART:38400,0,0")) {
    Serial.println("Baud rate incorrect: " + txt);
    // Ugh.
    while(1);
  }

  txt = reqHC05("AT+VERSION?");
  Serial.println("BT version: " + txt);

  // Have you tried turning it off and on again?
  txt = reqHC05("AT+RESET");
  if (!txt.startsWith("OK")) {
    Serial.println("Failed to reset module: " + txt);
    // Ugh.
    while(1);
  }

  // The reset seems to take some time.
  delay(1000);

  // Cancels connections and makes it visible.
  txt = reqHC05("AT+INIT");
  Serial.println("INIT: " + txt);

  txt = reqHC05("AT+DISC");
  Serial.println("DISC: " + txt);

  txt = reqHC05("AT+STATE?");
  Serial.println("STATE: " + txt);
}

String txt = "";
void loop() {
  char tmp;
  while (HC05.available() > 0) {
    tmp = HC05.read();
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
