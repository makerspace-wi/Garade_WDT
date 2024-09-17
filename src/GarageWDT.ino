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
#define Version "1.0.X"

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
#define PulsTol   32 // [10] * Pulsscan =165ms Tolerance

#define LedFlash      6 // [    6] ms for Flash
#define PulsReset   500 // [  500] ms for Reset
#define NoReStart 10000 // [10000] ms Mini not started

// VARIABLES
unsigned int wdTimeL =  0;  // count puls for high
unsigned int wdTimeH =  0;  // count puls for low
// time Control
unsigned long prev;         // ms previus time
boolean valN;               // new input value
boolean valO;               // old input value

int ledStatus = LOW;        // Status f�r die Blink-LED

void setup() {
 // Serial.begin(115200); // nano debug

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

  // Serial.println("nano;POR");  // Entry System restart MM 15.07.19
}

// PROGRAM --------------------------------------
void loop()
{
  if ((millis() - prev) >= PulsScan)
  {
    // digitalWrite(showEvt, !digitalRead(showEvt));
    // digitalWrite(showEvt, LOW);
    prev = millis();
    valN = digitalRead(WTDPuls);
    if (valN == LOW) {
      ++wdTimeL;
    } else {
      ++wdTimeH;
    }
    if (!valN && valO)
    {
      // Serial.println("WD L / H + C: " + String(wdTimeL) + " " + String(wdTimeH) + " " + String(wdtCount));  // Nano Debug only
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
    // Serial.println("Pin RES LOW / HIGH: " + String(wdTimeL) + " " + String(wdTimeH));  // Nano Debug only
    digitalWrite(LEDPuls, HIGH);
    digitalWrite(RES_REL, LOW);
    delay(PulsReset);
    digitalWrite(RES_REL, HIGH);

    prev = millis();
    while (!digitalRead(WTDPuls)) // [!]
    {
      // delay(2);
      // digitalWrite(showEvt, !digitalRead(showEvt));
      // if ((millis() - prev) >= NoReStart)
      // {
      //   digitalWrite(showEvt, HIGH);
      //   break;  
      // }
    }
    digitalWrite(LEDPuls, LOW);
    wdTimeL = 0;
    wdTimeH = 0;
    prev = millis();
  }
}
