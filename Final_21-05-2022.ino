#include <Arduino_JSON.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

#include <Wire.h>
#include <I2CKeyPad.h>
#include <time.h>
#include <stdlib.h>

int deviceId = 1;
int doorOpenTime = 10000;

const char* ssid = "Wifitest1";
const char* wifiPassword = "testes123123";

HTTPClient http;
WiFiClient client;
const char* api = "***********";

const uint8_t KEYPAD_ADDRESS = 0x20; // I2C adress where keyboard is located
I2CKeyPad keyPad(KEYPAD_ADDRESS);
char keymap[19] = "123A456B789C*0#D";

char attempKey[5];
int z = 0;

const uint8_t relayInputPIN = D0;
const uint8_t redLEDPIN = D7;
const uint8_t greenLEDPIN = D8;
const uint8_t buttonSwitchPIN = D5;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println(__FILE__);

  pinMode(relayInputPIN, OUTPUT);
  pinMode(redLEDPIN, OUTPUT);
  pinMode(greenLEDPIN, OUTPUT);
  pinMode(buttonSwitchPIN, INPUT);

  Wire.begin();
  Wire.setClock(400000);
  if (keyPad.begin() == false)
  {
    Serial.println("\nERROR: cannot communicate to keypad.\nPlease reboot.\n");
    while (1);
  }
  
  keyPad.loadKeyMap(keymap);

  WiFi.begin(ssid, wifiPassword);
  
  while(WiFi.status() != WL_CONNECTED){
    delay(2000);
    Serial.println("Connecting to " + String(ssid));
    }
  Serial.println("");
  Serial.println("Connected to " + String(ssid));
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());

  srand(time(NULL));
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(relayInputPIN, HIGH);
  digitalWrite(redLEDPIN, HIGH);
  int buttonState = digitalRead(buttonSwitchPIN);

  if(buttonState == HIGH){
    accessGranted();
    }
  
  char input = keyPad.getChar();
  delay(100);
  
  if(input){
     Serial.println(input);
     switch(input){
      case '*':
        z = 0;
        memset(attempKey, '\0', sizeof(attempKey));
        break;
      case '#':
        delay(1000);
        checkKey();
        attempKey[z] = '\0';
        break;
      default:
        attempKey[z] = input;
        z++;
      }
    }
}

String httpRequestData = "";
void checkKey(){
  if(WiFi.status() == WL_CONNECTED) {
    
    http.begin(client, api);
    http.addHeader("Content-Type", "text/plain");
   
    httpRequestData = "{\"id\":\"" + String(deviceId) + "\",\"pin\":\"" + String(attempKey) + "\",\"picture\":\"data:image/jpeg;base64,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\"}";
    
    int httpResponse = http.POST(httpRequestData);
    
    Serial.println(httpRequestData);
    Serial.println(String(httpResponse));

    if(httpResponse == 200) {
      JSONVar responsePayload = JSON.parse(http.getString());
      String accessStatus = JSON.stringify(responsePayload["success"]);
      Serial.println( JSON.stringify(responsePayload));

      if(accessStatus == "true"){
        accessGranted();
      }else{
        accessDenied();
      }
    }else if(httpResponse < 0){
      checkKey();
    }
    
    http.end();
    }else{
      Serial.println("cannot connected to internet");
    }
}

void accessGranted(){
  digitalWrite(relayInputPIN, LOW);
  digitalWrite(redLEDPIN, LOW);
  digitalWrite(greenLEDPIN, HIGH);
  delay(doorOpenTime);
  digitalWrite(relayInputPIN, HIGH);
  digitalWrite(greenLEDPIN, LOW);
  digitalWrite(redLEDPIN, HIGH);
}

void accessDenied(){
  digitalWrite(redLEDPIN, LOW);
  delay(500);
  digitalWrite(redLEDPIN, HIGH);
  delay(500);
  digitalWrite(redLEDPIN, LOW);
  delay(500);
  digitalWrite(redLEDPIN, HIGH);
}
