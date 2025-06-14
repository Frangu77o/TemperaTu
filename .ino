#include <LiquidCrystal.h>

// === Definizione dei pin ===
#define RS      12
#define EN      11
#define D4       5
#define D5       4
#define D6       3
#define D7       2

#define SENSORE 10
#define MOTOR    9
#define LED      8

// === Oggetto LCD (16 colonne, 2 righe) ===
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

// === Variabili di stato ===
bool motoreAttivo = false;
int numeroMatite = 0;
unsigned long tempoUtilizzo = 0;

// === Setup iniziale ===
void setup() {
  Serial.begin(9600);

  pinMode(SENSORE, INPUT);
  pinMode(MOTOR, OUTPUT);
  pinMode(LED, OUTPUT);

  digitalWrite(LED, HIGH); // LED acceso all'avvio

  lcd.begin(16, 2);
  lcd.print("Matite: 0");
  lcd.setCursor(0, 1);
  lcd.print("Tempo: 00:00");
}

// === Loop principale ===
void loop() {
  unsigned long tempoInizio = millis();            // Momento di inizio ciclo
  int statoSensore = digitalRead(SENSORE);         // Lettura stato sensore

  // --- Caso: matita rimossa e motore attivo ---
  if (statoSensore == HIGH && motoreAttivo) {
    digitalWrite(MOTOR, LOW);                      // Spegne il motore
    numeroMatite++;                                // Conta una nuova matita

    // Aggiorna il display
    lcd.setCursor(8, 0);
    lcd.print("    ");                             // Pulisce area numerica
    lcd.setCursor(8, 0);
    lcd.print(numeroMatite);

    digitalWrite(LED, HIGH);                       // Riaccende il LED fisso
    motoreAttivo = false;
  }

  // --- Caso: matita presente ---
  else if (statoSensore == LOW) {
    digitalWrite(MOTOR, HIGH);                     // Accende il motore

    // Effetto lampeggio LED
    digitalWrite(LED, LOW);
    delay(150);
    digitalWrite(LED, HIGH);
    delay(150);

    // Aggiunge il tempo di funzionamento al totale
    tempoUtilizzo += millis() - tempoInizio;

    // Aggiorna il display con il tempo formattato
    String tempoFormattato = formatTime(tempoUtilizzo / 1000);
    lcd.setCursor(7, 1);
    lcd.print(tempoFormattato);

    motoreAttivo = true;
  }
}

// === Funzione di formattazione tempo (mm:ss) ===
String formatTime(unsigned long secondiTotali) {
  int minuti = secondiTotali / 60;
  int secondi = secondiTotali % 60;
  char buffer[6];
  sprintf(buffer, "%02d:%02d", minuti, secondi);
  return String(buffer);
}
