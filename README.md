# 🔧 TemperaTu – Temperino Automatico Embedded

## 👤 Autore

**Francesco Emanuele Franguelli**

---

## 📖 Descrizione del progetto

**TemperaTu** è un progetto embedded che realizza un **temperino automatico intelligente**. Utilizza un sensore a infrarossi per rilevare l’inserimento di una matita e attiva automaticamente un motore per iniziare il processo di tempera. Quando la matita viene rimossa, il motore si arresta. Il sistema funziona in **loop chiuso** grazie al feedback continuo del sensore IR.

### 🟢 Interfaccia utente

Il sistema utilizza tre LED per fornire informazioni sullo stato del temperamatite:

- **Verde**: il sistema è acceso
- **Arancione**: il motore è in funzione (temperino attivo)
- **Rosso**: guasto rilevato (es. errore motore o sensore)

---

## 🧱 Componenti hardware

| Componente                    | Quantità | Descrizione                            |
| ----------------------------- | -------- | -------------------------------------- |
| ELEGOO Mega 2560 R3           | 1        | Microcontrollore basato su ATmega2560  |
| Sensore Infrarossi (IR)       | 1        | Rilevamento della matita               |
| Motore ()                     | 1        | Movimento della lama del temperamatite |
| LED (verde, arancione, rosso) | 3        | Indicatori di stato                    |
| Resistenze per LED            | 3        | Limitatori di corrente                 |
| Alimentazione 5V o 12V        | 1        | In base al motore scelto               |
| Breadboard                    | 1        | Per prototipazione o montaggio finale  |
| Cavi jumper                   | vari     | Collegamenti                           |

---

## 🧠 Organizzazione del codice
