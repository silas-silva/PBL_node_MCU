#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

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

/*Unidade 0*/
#define unidade_0 0b11000001

/* Tabela de REQUISICAO*/
//#define situacao_sensor  0b00000001


void setup() {
  
  /* switch on led */
  pinMode(led_pin, OUTPUT);
  pinMode(D0, INPUT);
  
  Serial.begin(9600); // BaudRate
  
  //Serial.println("Booting");
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    //Serial.println("Tentando nova conexão...");
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
  //Serial.println("Tudo pronto.");
}

void loop() {
  ArduinoOTA.handle();

  digitalWrite(led_pin, HIGH);
  if(Serial.available() > 0) { // Verifica se tem algo sendo recebido
      char c = Serial.read(); //Lê o pino RX
      if(c == 0b000000){
        //Serial.write("Dado recebido");
        digitalWrite(led_pin, LOW);
        delay(2000);
      }
  }
}