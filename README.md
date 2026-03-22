# Controllo di Velocità di un Motore DC con PID

Questo repository contiene il firmware (Arduino) e gli script di analisi (MATLAB) per un progetto di modellistica e controllo di un motore in corrente continua (DC). 

L'obiettivo del progetto è identificare il modello matematico del sistema fisico, implementare un anello di retroazione e mantenere una velocità di rotazione costante al variare del carico applicato.

## Hardware Utilizzato
* **Microcontrollore:** Arduino
* [cite_start]**Driver di Potenza:** Modulo L298N (Controllo PWM e Direzione) [cite: 12, 13, 24]
* **Attuatore:** Motore DC
* [cite_start]**Sensore:** Encoder incrementale (fili Verde/Blu) per il feedback di posizione/velocità [cite: 23]

## Struttura del Repository e Fasi del Progetto

Il progetto è suddiviso in fasi sequenziali, dal test hardware fino alla simulazione teorica.

### 1. Identificazione a Catena Aperta (Hardware)
* **`1_Identificazione_Manuale_NoEncoder.ino`**: Script per il test iniziale. [cite_start]Permette di inviare via Seriale un valore PWM da 0 a 255 [cite: 4] per mappare la relazione statica tra la tensione applicata e gli RPM (calcolati esternamente). [cite_start]Il motore viene inizializzato da fermo per sicurezza[cite: 5].

### 2. Taratura del Sensore
* **`2_Conteggio_CPR_Encoder.ino`**: Modulo per la validazione dell'encoder. [cite_start]Il motore viene fatto girare a una potenza fissa (PWM 70) [cite: 18] per contare gli impulsi e ricavare il valore di CPR (Counts Per Revolution). [cite_start]L'algoritmo stampa sulla Seriale il nuovo valore solo quando la posizione dell'encoder cambia[cite: 19].

### 3. Controllo ad Anello Chiuso (PID)
* **`3_Controllo_PID_Encoder.ino`**: Il "cervello" del progetto. [cite_start]Utilizza il valore CPR calcolato in precedenza (1343.0)[cite: 21].
    * [cite_start]Il target è impostato a **150 RPM**[cite: 21].
    * Il controllore PID calcola l'errore e aziona il motore (saturando il PWM tra 0 e 255)[cite: 31, 32, 34].
    * [cite_start]I guadagni scelti sono: **Kp = 1.5, Ki = 3.5, Kd = 0.05**[cite: 22].
    * [cite_start]Il sistema stampa i dati in formato CSV (`Tempo[s],Target[RPM],Reale[RPM]`) per la successiva elaborazione su MATLAB/Excel[cite: 28, 35].

### 4. Analisi Teorica e Sperimentale (MATLAB)
* **`1_Analisi_Teorica_NoEncoder.m`**: Script che elabora i dati dell'identificazione sperimentale per ricavare la funzione di trasferimento del motore. Genera il diagramma di Bode, traccia il Luogo delle Radici e simula l'effetto di un disturbo (es. attrito/carico) confrontando la reazione del sistema con e senza PID.
* **`2_Analisi_Sperimentale_PID.m`**: Importa i dati CSV generati da Arduino durante il test fisico. Plotta la risposta reale del motore mostrando graficamente l'intervento del PID quando viene applicato e rilasciato un carico meccanico (evidenziando l'overshoot di assestamento).

## Come avviare il progetto
1.  [cite_start]Collegare i pin ENA, IN1 e IN2 rispettivamente ai pin digitali 9, 8 e 7 di Arduino[cite: 1, 2, 23, 24].
2.  [cite_start]Collegare i due canali dell'encoder ai pin 2 e 3 (necessari per gli interrupt)[cite: 23].
3.  Caricare progressivamente gli sketch seguendo la numerazione.
4.  [cite_start]Aprire il Monitor Seriale a 9600 baud per l'identificazione manuale[cite: 2].
5.  Per testare il PID, utilizzare il Serial Plotter o copiare i log generati per farli analizzare dagli script MATLAB.
