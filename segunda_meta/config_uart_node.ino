#include <SoftwareSerial.h>

SoftwareSerial orangePiSerial(D1, D2); // Define os pinos D1 e D2 como RX e TX, respectivamente.

void setup() {
  Serial.begin(9600); // Inicializa a comunicação serial para debug.
  orangePiSerial.begin(9600); // Inicializa a comunicação serial para receber dados da Orange Pi PC Plus.
}

void loop() {
  if (orangePiSerial.available()) { // Verifica se há dados disponíveis na porta serial da Orange Pi PC Plus.
    String data = orangePiSerial.readString(); // Lê a string recebida.
    Serial.println(data); // Imprime a string recebida na porta serial do NodeMCU para debug.
    // Fazer algo com a string recebida...
  }
}