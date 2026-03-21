// PIN DEFINITIONS
const int ENA_PIN = 9;  // Pin PWM per la velocità
const int IN1_PIN = 8;  // Pin Direzione 1
const int IN2_PIN = 7;  // Pin Direzione 2

void setup() {
  // Inizializza la comunicazione col PC
  Serial.begin(9600);
  
  // Configura i pin come uscita
  pinMode(ENA_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  
  Serial.println("--- SYSTEM IDENTIFICATION TEST ---");
  Serial.println("Inserisci un valore PWM (0 - 255) e premi Invio.");
  Serial.println("Esempio: 100 per bassa velocità, 255 per massima.");
  
  // Ferma il motore all'avvio
  stopMotor();
}

void loop() {
  if (Serial.available() > 0) {
    // Legge il numero intero
    int pwmValue = Serial.parseInt();
    
    // Pulisce il buffer da caratteri extra
    while(Serial.available() > 0) { Serial.read(); }

    // Controllo di sicurezza sui valori
    if (pwmValue >= 0 && pwmValue <= 255) {
      Serial.print("Comando ricevuto. Imposto PWM a: ");
      Serial.println(pwmValue);
      driveMotor(pwmValue);
    } else {
      Serial.println("Errore: Inserisci un numero tra 0 e 255.");
    }
  }
}

// Funzione per muovere il motore
void driveMotor(int speed) {
  // Imposta la direzione (AVANTI)
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);

  // Imposta la potenza
  analogWrite(ENA_PIN, speed);
}

// Funzione per fermare tutto
void stopMotor() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  analogWrite(ENA_PIN, 0);
}