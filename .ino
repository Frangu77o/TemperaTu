#include <LiquidCrystal.h>
#include <TaskScheduler.h>

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
unsigned long tempoInizioMotore = 0;
bool ledState = false;

// === Scheduler e Tasks ===
Scheduler runner;

// Dichiarazione forward delle callback
void checkSensorCallback();
void blinkLedCallback();
void updateTimeCallback();

// Task per controllare il sensore
Task checkSensorTask(50, TASK_FOREVER, &checkSensorCallback);

// Task per lampeggiare il LED
Task blinkLedTask(150, TASK_FOREVER, &blinkLedCallback);

// Task per aggiornare il tempo
Task updateTimeTask(1000, TASK_FOREVER, &updateTimeCallback);

// === Callback: controllo sensore ===
void checkSensorCallback() {
  int statoSensore = digitalRead(SENSORE);

  // --- Caso: matita rimossa e motore attivo ---
  if (statoSensore == HIGH && motoreAttivo) {
    digitalWrite(MOTOR, LOW);
    numeroMatite++;

    // Aggiorna il display
    lcd.setCursor(8, 0);
    lcd.print("    ");
    lcd.setCursor(8, 0);
    lcd.print(numeroMatite);

    digitalWrite(LED, HIGH);
    motoreAttivo = false;

    // Disabilita tasks del motore
    blinkLedTask.disable();
    updateTimeTask.disable();
  }
  // --- Caso: matita presente ---
  else if (statoSensore == LOW && !motoreAttivo) {
    digitalWrite(MOTOR, HIGH);
    motoreAttivo = true;
    tempoInizioMotore = millis();

    // Abilita tasks del motore
    blinkLedTask.enable();
    updateTimeTask.enable();
  }
}

// === Callback: lampeggio LED ===
void blinkLedCallback() {
  ledState = !ledState;
  digitalWrite(LED, ledState ? HIGH : LOW);
}

// === Callback: aggiornamento tempo ===
void updateTimeCallback() {
  if (motoreAttivo) {
    tempoUtilizzo += (millis() - tempoInizioMotore);
    tempoInizioMotore = millis();

    String tempoFormattato = formatTime(tempoUtilizzo / 1000);
    lcd.setCursor(7, 1);
    lcd.print(tempoFormattato);
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

// === Setup iniziale ===
void setup() {
  Serial.begin(9600);

  pinMode(SENSORE, INPUT);
  pinMode(MOTOR, OUTPUT);
  pinMode(LED, OUTPUT);

  digitalWrite(LED, HIGH);

  lcd.begin(16, 2);
  lcd.print("Matite: 0");
  lcd.setCursor(0, 1);
  lcd.print("Tempo: 00:00");

  // Inizializza lo scheduler
  runner.init();

  // Aggiungi i task
  runner.addTask(checkSensorTask);
  runner.addTask(blinkLedTask);
  runner.addTask(updateTimeTask);

  // Abilita solo il task del sensore all'avvio
  checkSensorTask.enable();
}

// === Loop principale ===
void loop() {
  runner.execute();
}
