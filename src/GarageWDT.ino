/* started on 07SSep2024 - by Michael

 Check if garage controller is checking position

 Pulse from Garage controller (Pin 9)
  decoding with ATTiny and Reset Garage Controller over power relais
  - check WDT pulse length:
    low: <=0,5 sec and high: <=0,5 sec
  - relais switching occurse if pulse lenght:
    low: >1 sec or high >1 sec

 last changes on 23.09.2024 by Michael Muehl
 changed: add wait time before reset puls
 !!! Reset on the external red button genarates over WDT after 25s a second reset!!!
*/
#define Version "1.0.7" // (Test = 1.0.7 ==> 1.0.8)
// wdt time to check (tp = time for part of the programm) [tp = 16,5ms]
#define PulsScan     1 // [ 1] * tp ( 16,5ms) Flash
#define PulsHigh    30 // [30] * tp (495  ms) High
#define PulsLow     30 // [30] * tp (495  ms) Low
#define PulsTol     32 // [32] * tp (528  ms) Tolerance
// wait time reset signal [tp = 8ms]
#define LedFlash     6 // [   6] * tp (0,5s) Flash
#define Be4PReset 3125 // [3125] * tp ( 25s) Before PulsReset
#define PulsReset  125 // [ 125] * tp (  1s)Reset
#define NoReStart   35 // [  35] (35s) max.wait for Restart

#include <Arduino.h>

// Pin Assinements ATTiny 25 -------
#define LEDPuls 0   // [0 | PB0 | Pin 5] {Nano 3} output - puls shown with LED
#define spezPin 1   // [1 | PB1 | Pin 6] {Nano 4} input  - [not used]
#define RES_REL 2   // [2 | PB2 | Pin 7] {Nano 5} output - reset over relais
#define WTDPuls 3   // [3 | PB3 | Pin 2] {Nano 6} input  - watch dog pulse
#define showEvt 4   // [4 | PB4 | Pin 3] {Nano 7} output - show event
//   [not used] 5   // [5 | PB5 | Pin 1] RESET 

// VARIABLES
unsigned int wdTimeL =  0;  // count puls for high
unsigned int wdTimeH =  0;  // count puls for low
// time Control
unsigned long prev;         // ms previus time
boolean valN;               // new input value
boolean valO;               // old input value

int ledStatus = LOW;        // Status f�r die Blink-LED

void setup() {
  pinMode(RES_REL, OUTPUT);
  pinMode(LEDPuls, OUTPUT);
  pinMode(showEvt, OUTPUT);

  digitalWrite(RES_REL, HIGH);
  digitalWrite(showEvt, LOW);
  // wait for first puls
  digitalWrite(LEDPuls, HIGH); // [HIGH]
  while (!digitalRead(WTDPuls)) // [!]
  {}
  digitalWrite(LEDPuls, LOW);  // [LOW]
  prev = millis();

}

// PROGRAM --------------------------------------
void loop()
{
  if ((millis() - prev) >= PulsScan)
  {
    prev = millis();
    valN = digitalRead(WTDPuls);
    if (valN == LOW) {
      ++wdTimeL;
    } else {
      ++wdTimeH;
    }
    if (!valN && valO)
    {
      digitalWrite(LEDPuls, HIGH);
      delay(LedFlash);
      digitalWrite(LEDPuls, LOW);
      wdTimeL = 0;
      wdTimeH = 0;
    }
    valO = valN;
  }
  if (wdTimeL > (PulsLow + PulsTol) || wdTimeH > (PulsHigh + PulsTol))
  {
    digitalWrite(LEDPuls, HIGH);
    delay(Be4PReset);
    digitalWrite(LEDPuls, LOW);
    digitalWrite(RES_REL, LOW);
    delay(PulsReset);
    digitalWrite(RES_REL, HIGH);
    // wait for next reset if no pulses arrived
    digitalWrite(LEDPuls, HIGH);
    prev = millis();  // start
    do
    {
      if (digitalRead(WTDPuls)) break;
    }
    while ((millis() - prev) < NoReStart * 100);
    prev = millis();  // finished
    digitalWrite(LEDPuls, LOW);
    wdTimeL = 0;
    wdTimeH = 0;
  }
}
