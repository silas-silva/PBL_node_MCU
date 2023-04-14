void setup() {
    pinMode(D1, INPUT); //Configurar porta do botão
    pinMode(4, OUTPUT); //Configurar porta do led
    Serial.begin(9600);
}

void loop() {
    if(digitalRead(D1) == HIGH) { // Se o botão foi pressionado
        digitalWrite(4, HIGH); // Acender o LED
    } else {
        digitalWrite(4, LOW); // Apagar o LED
    }
    delay(50); // Pequeno delay para evitar bouncing
}