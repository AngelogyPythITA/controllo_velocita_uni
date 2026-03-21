#include <Encoder.h>

// --- CONFIGURAZIONE ---
double CPR = 1343.0; // Calcolata grazie allo sketch di conteggio

// Target desiderato
double target_RPM = 150.0; 

// Parametri PID
double Kp = 1.5;   // Reazione Proporzionale
double Ki = 3.5;   // Reazione Integrale
double Kd = 0.05;  // Reazione Derivativa

// --- PIN ---
Encoder myEnc(2, 3);      // Encoder (Verde/Blu)
const int ENA = 9;        // PWM Motore
const int IN1 = 8;        // Driver IN1
const int IN2 = 7;        // Driver IN2

// Variabili
long oldPosition  = 0;
unsigned long lastTime = 0;
double input_RPM = 0;
double error_integral = 0;
double last_error = 0;

void setup() {
  Serial.begin(9600);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  
  // Imposta direzione AVANTI
  digitalWrite(IN1, HIGH); 
  digitalWrite(IN2, LOW);  
  
  delay(1000); // Aspetta 1 sec prima di partire
  Serial.println("Tempo[s],Target[RPM],Reale[RPM]"); // Intestazione per Excel
}

void loop() {
  // Eseguiamo il PID ogni 50ms (20 volte al secondo)
  if (millis() - lastTime >= 50) {
    double dt = 0.050; // 50ms = 0.05s
    
    // 1. LEGGI VELOCITÀ (Feedback)
    long newPosition = myEnc.read();
    long deltaPos = newPosition - oldPosition;
    oldPosition = newPosition;
    
    // Calcolo RPM: (Impulsi / CPR) * (60 sec / dt)
    input_RPM = (deltaPos / CPR) * 60.0 * (1.0 / dt);

    // 2. CALCOLO ERRORE
    double error = target_RPM - input_RPM;

    // 3. CALCOLO PID
    error_integral += error * dt;
    double derivative = (error - last_error) / dt;
    
    double output_signal = (Kp * error) + (Ki * error_integral) + (Kd * derivative);
    
    // 4. AZIONAMENTO MOTORE (Clamp 0-255)
    int pwm_out = (int)output_signal;
    if (pwm_out > 255) pwm_out = 255;
    if (pwm_out < 0) pwm_out = 0;
    
    analogWrite(ENA, pwm_out);

    // 5. STAMPA DATI (Per Matlab)
    // Formato CSV: Tempo, Target, Reale
    Serial.print(millis()/1000.0);
    Serial.print(",");
    Serial.print(target_RPM);
    Serial.print(",");
    Serial.println(input_RPM);

    last_error = error;
    lastTime = millis();
  }
}