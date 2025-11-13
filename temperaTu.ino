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
struct Motore {
  bool isActive;
  int matiteTemperate;
  unsigned long tempoInizio;
  unsigned long tempoUtilizzo;
};

Motore motore = {false, 0, 0, 0};
bool isLedActive = false;

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
  if (statoSensore == HIGH && motore.isActive) {
    // Ferma il motore
    digitalWrite(MOTOR, LOW);
    motore.isActive = false;

    // Mette il LED acceso fisso
    digitalWrite(LED, HIGH);
    isLedActive = true;

    // Aggiorna il tempo di utilizzo e il display con l'intervallo finale
    updateTimeCallback();

    // Aggiorna il conteggio delle matite temperate
    if(motore.matiteTemperate == 1000) {
      motore.matiteTemperate++;
      // Aggiorna il display
      lcd.setCursor(8, 0);
      lcd.print("+1K ");
    } else if (motore.matiteTemperate < 1000) {
      motore.matiteTemperate++;
      // Aggiorna il display
      lcd.setCursor(8, 0);
      lcd.print(motore.matiteTemperate);
    }

    // Disabilita tasks del motore
    blinkLedTask.disable();
    updateTimeTask.disable();
  }
  // --- Caso: matita presente ---
  else if (statoSensore == LOW && !motore.isActive) {
    // Avvia il motore
    digitalWrite(MOTOR, HIGH);
    motore.isActive = true;

    // Inizializza il tempo di inizio
    motore.tempoInizio = millis();

    // Abilita tasks del motore
    blinkLedTask.enable();
    updateTimeTask.enable();
  }
}

// === Callback: lampeggio LED ===
void blinkLedCallback() {
  isLedActive = !isLedActive;
  digitalWrite(LED, isLedActive ? HIGH : LOW);
}

// === Callback: aggiornamento tempo ===
void updateTimeCallback() {
  if (motore.isActive) {
    motore.tempoUtilizzo += (millis() - motore.tempoInizio);
    motore.tempoInizio = millis();
    char* tempoFormattato = formatTime(motore.tempoUtilizzo / 1000);
    lcd.setCursor(7, 1);
    lcd.print(tempoFormattato);
  }
}

// === Funzione di formattazione tempo (mm:ss) ===
char* formatTime(unsigned long secondiTotali) {
  static char buffer[7];
  unsigned int secondi = secondiTotali % 60;
  unsigned long minuti = secondiTotali / 60;

  // Se supera 1 ora (60 minuti) mostra +1h
  if (minuti >= 60) {
    strcpy(buffer, "+1h   ");
  } else {
    sprintf(buffer, "%02lu:%02u", minuti, secondi);
  }

  return buffer;
}

// === Setup iniziale ===
void setup() {
  Serial.begin(9600);

  pinMode(SENSORE, INPUT);
  pinMode(MOTOR, OUTPUT);
  pinMode(LED, OUTPUT);

  digitalWrite(LED, HIGH);
  isLedActive = true;

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
