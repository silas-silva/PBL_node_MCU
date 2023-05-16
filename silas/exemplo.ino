#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <unistd.h>

#ifndef STASSID
#define STASSID "INTELBRAS"
#define STAPSK  "Pbl-Sistemas-Digitais"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const char* host = "ESP-10.0.0.107";

int led_pin = LED_BUILTIN;
#define N_DIMMERS 3
int dimmer_pin[] = {14, 5, 15};

void setup() {
  Serial.begin(115200);

  /* switch on led */
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);

  Serial.println("Booting");
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    Serial.println("Retrying connection...");
  }
  /* switch off led */
  digitalWrite(led_pin, HIGH);

  /* configure dimmers, and OTA server events */
  analogWriteRange(1000);
  analogWrite(led_pin, 990);

  for (int i = 0; i < N_DIMMERS; i++) {
    pinMode(dimmer_pin[i], OUTPUT);
    analogWrite(dimmer_pin[i], 50);
  }

  ArduinoOTA.setHostname(host);
  ArduinoOTA.onStart([]() { // switch off all the PWMs during upgrade
    for (int i = 0; i < N_DIMMERS; i++) {
      analogWrite(dimmer_pin[i], 0);
    }
    analogWrite(led_pin, 0);
  });

  ArduinoOTA.onEnd([]() { // do a fancy thing with our board led at end
    for (int i = 0; i < 30; i++) {
      analogWrite(led_pin, (i * 100) % 1001);
      delay(50);
    }
  });

  ArduinoOTA.onError([](ota_error_t error) {
    (void)error;
    ESP.restart();  
  });

  /* setup the OTA server */
  ArduinoOTA.begin();
  Serial.println("Ready");
}


#define NODE_ID 0x1
#define DESELECT_NODE 0x81
#define INVALID_COMMAND 0xED

#define FOLLOW_BIT_MASK 15
#define CONSULT_BIT_MASK 15
#define INVALID_COMMAND 255

#define TWO_MILISECONDS 2000
//                            A0  D0 D1
unsigned char sensors_pin = { 17, 16, 5 };
unsigned char array_command[] = {
                                  0xC0, 0xF0, 0xC1, 0xF1, 0xC2, 0xF2, 0xC3,
                                  0xF3, 0xC4, 0xF4, 0xC5, 0xF5, 0xC6, 0xF6
                                };

bool selectPlaca = false;
unsigned char digitalSensorData = 0;
unsigned short int analogSensorData = 0;
unsigned char analogBytes[2];

// Realiza uma busca binaria e *comands*, por *command*
bool isValidCommand(unsigned char commands[], unsigned char command) {
    unsigned int array_s = commands.size();
    unsigned int init = 0;
    unsigned int end = array_s;
    while(init <= end) {
      unsigned int mid = array_s / 2;
      if(commands[mid] == command) return true;
      else if(commands[mid] > command) end = mid - 1;
      else init = mid + 1;
    }
    return false;
}

unsigned char extract_command_info(unsigned char command) {
  unsigned char info;
  if(isValidCommand(command)) {
    info = FOLLOW_BIT_MASK & command;
    if (info <= 7) {
      return info
    }
    return (CONSULT_BIT_MASK & command)
  }
  return INVALID_COMMAND;
}

unsigned char* short2bytes(unsigned short int data) {
  unsigned char bytes[2];
  bytes[0] = data & 0xFF;
  bytes[1] = (data >> 8) & 0xFF;
  return bytes;
}


void loop() {
  ArduinoOTA.handle();
  if (Serial.available() > 0 ){
      char r = Serial.read();
      if(r == NODE_ID) nodeSelected = true;
      while(r != DESELECT_NODE && nodeSelected) {
        if (Serial.available() > 0) {
          r = Serial.read();
          if(isValidCommand(r)) {
            r = extract_command_info(r);
            // Pegar dado do pino analogico
            if(!r) {
              analogSensorData = (unsigned short int) analogRead(sensors_pin[r]);
              analogBytes = short2bytes(analogSensorData);
              Serial.write(analogBytes[0]);
              usleep(TWO_MILISECONDS);
              Serial.write(analogBytes[1]);
            }
            // Pegar dado dos pinos digitais
            else {
              digitalSensorData = digitalRead(sensors_pin[r]);
              Serial.write(digitalSensorData);
            }
          }
        }
      }
      nodeSelected = false;
  }
} 
