/* started on 07SSep2024 - by Michael

* Reset garage control

Pulse from Garage Mini (Pin 9)
* decoding with ATTiny and Reset Entry Mini with RESET-PIN or power off with relais
* pulse length low:  0,5 sec
* pulse length high: 0,5 sec WDT is runnig
*                   RESET:
*                   > 5 sec --> Relais
*
* last changes on 07.09.2024 by Michael Muehl
* changed: start with programming
*/
#define Version "1.0.5"

#include <Arduino.h>

// Pin Assinements ATTiny 25 -------
#define LEDPuls 0   // [0 | PB0 | Pin 5] {Nano 3} output - puls shown with LED
#define spezPin 1   // [1 | PB1 | Pin 6] {Nano 4} input  - [not used]
#define RES_REL 2   // [2 | PB2 | Pin 7] {Nano 5} output - reset over relais
#define WTDPuls 3   // [3 | PB3 | Pin 2] {Nano 6} input  - watch dog pulse
#define showEvt 4   // [4 | PB4 | Pin 3] {Nano 7} output - show event
//   [not used] 5   // [5 | PB5 | Pin 1] RESET 

// counter set values
#define PulsScan   1 // [ 1] Pulsscan * 16,5ms
#define PulsHigh  30 // [30] * Pulsscan =495ms High
#define PulsLow   30 // [30] * Pulsscan =495ms Low
#define PulsTol   32 // [32] * Pulsscan =528ms Tolerance

#define LedFlash      6 // [  6] * 8ms Flash
#define PulsReset   125 // [125] * 8ms Reset
#define NoReStart  2425 // [250] * 8ms + [50](first sec) wait for Restart

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
    digitalWrite(RES_REL, LOW);
    delay(PulsReset);
    digitalWrite(RES_REL, HIGH);
    // wait for next reset if no pulses arrived
    digitalWrite(LEDPuls, LOW);
    delay(LedFlash);
    digitalWrite(LEDPuls, HIGH);
    delay(NoReStart);
    digitalWrite(LEDPuls, LOW);
    wdTimeL = 0;
    wdTimeH = 0;
  }
}
