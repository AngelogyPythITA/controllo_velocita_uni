#include <Encoder.h>

// --- CONFIGURAZIONE PIN ---
Encoder myEnc(2, 3);

// Driver L298N
const int ENA = 9;  // Pin PWM (Velocità)
const int IN1 = 8;  // Direzione A
const int IN2 = 7;  // Direzione B

long oldPosition  = -999;

void setup() {
  Serial.begin(9600);
  
  // Configura i pin del motore come Uscita
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  
  Serial.println("--- TEST INIZIATO ---");
  Serial.println("1. Il motore dovrebbe iniziare a girare.");
  Serial.println("2. I numeri qui sotto dovrebbero cambiare.");
  Serial.println("---------------------");

  // AVVIA IL MOTORE
  // Imposto la direzione
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  
  // Do potenza (0-255)
  analogWrite(ENA, 70); 
}

void loop() {
  // LEGGE L'ENCODER
  long newPosition = myEnc.read();
  
  // Stampa solo se il numero cambia
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    Serial.print("Conteggio Encoder: ");
    Serial.println(newPosition);
  }
  
  // Piccola pausa per non intasare la seriale
  delay(10); 
}