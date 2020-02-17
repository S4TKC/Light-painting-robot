#include <boarddefs.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <ir_Lego_PF_BitStreamEncoder.h>

#include <FastLED.h>

const int FLDataPin = A0;
const int LEDCount = 7;
CRGB leds[LEDCount];

const int m11 = 4;
const int m12 = 5;
const int m21 = 7;
const int m22 = 6;

const int encoder1 = 2;
const int encoder2 = 3;

int debouncetime = 1; //in ms
volatile unsigned long last_interrupt1 = 0;
volatile unsigned long last_interrupt2 = 0;

const int IR_reciever_pin = 8;
IRrecv IR_receiver(IR_reciever_pin);
decode_results IR_results;

volatile int count1 = 0;
volatile int count2 = 0;


void pinSetup()
{
  Serial.begin(9600);
  IR_receiver.enableIRIn();

  pinMode(m11, OUTPUT); //m1 pin 1
  pinMode(m12, OUTPUT); //m1 pin 2
  pinMode(m21, OUTPUT); //m2 pin 1
  pinMode(m22, OUTPUT); //m2 pin 2
  pinMode(FLDataPin, OUTPUT); //FastLED data
  FastLED.addLeds<NEOPIXEL, FLDataPin>(leds, LEDCount);
  pinMode(encoder1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoder1), ISR_encoder1, RISING);
  pinMode(encoder2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoder2), ISR_encoder2, RISING);
}
//--------------------------------------------------------------
const int allBrightFactor = 3; //The amount to dim all LEDs for, since the PSU cannot handle driving at full brightness

void blue()
{
  resetLEDs();
  leds[0] = CRGB::Blue;
  leds[2] = CRGB::Blue;
  leds[4] = CRGB::Blue;
  leds[6] = CRGB::Blue;
  FastLED.show();
}
void red()
{
  resetLEDs();
  leds[1] = CRGB::Red;
  leds[3] = CRGB::Red;
  leds[5] = CRGB::Red;
  FastLED.show();
}

void black()
{
  resetLEDs();
  FastLED.show();
}

void prepareBlink()
{
  resetLEDs();
  leds[3] = CRGB::Crimson;
  FastLED.show();
  delay(250);
  black();
  delay(1000);
}

void HSVCycle(int progress, int len) //All LEDs will get a hue based on progress
{
  resetLEDs();
  long progress_mapped = progress * 255L / len;
  for (int i = 0; i < LEDCount; i++)
  {
    leds[i] = CHSV(progress_mapped, 255, 255/ allBrightFactor);
  }
  FastLED.show();
}

void HSVSidewaysCycle(int progress, int len) //LEDs will get a hue based on progress, with an offset based on position.
{
  resetLEDs();
  long progress_mapped = progress * 255L / len;
  for (int i = 0; i < LEDCount; i++)
  {
    int newHValue = progress_mapped + i * (255 / LEDCount);
    newHValue = newHValue > 255 ? newHValue - 255 : newHValue;
    leds[i] = CHSV(newHValue, 255, 255 / allBrightFactor);
  }
  FastLED.show();
}

void RedGreenCycle(int progress, int len)
{
  resetLEDs();
  int progress_mapped = (int) progress * 255L / len;
  for (int i = 0; i < LEDCount; i += 1) //Only 4 of 7 LEDs will be lit, the outside ones, and the two next to the middle.
  {
    leds[i].g = progress_mapped / allBrightFactor;
    leds[i].r = (255 - progress_mapped) / allBrightFactor;
    leds[i].b = 0;
  }
  FastLED.show();
}

void resetLEDs()
{
  for (int i = 0; i < LEDCount; i++)
  {
    leds[i] = CRGB::Black;
  }
}
//--------------------------------------------------------------
void printIRData()
{
  if (IR_receiver.decode(&IR_results))
  {
    Serial.println(IR_results.value);
    IR_receiver.resume(); // Receive the next value
  }
}

int updateIRResults()
{
  if (IR_receiver.decode(&IR_results))
  {
    setIRData(IR_results.value);
    IR_receiver.resume();

  }
}

/* CH- FFA25F 16753247
   CH  FF629D 16736925
   CH+ FFE21D 16769565
   |<< FF22DD 16720605
   >>| FF02FD 16712445
   >|| FFC23D 16761405
   -   FFE01F 16769055
   +   FFA857 16754775
   EQ  FF906F 16748655
   0   FF6897 16738455
   100+FF9867 16750695
   200+FFB04F 16756815
   1   FF30CF 16724175
   2   FF18E7 16718055
   3   FF7A85 16743045
   4   FF10EF 16716015
   5   FF38C7 16726215
   6   FF5AA5 16734885
   7   FF42BD 16728765
   8   FF4AB5 16730805
   9   FF52AD 16732845
*/
//--------------------------------------------------------------

//01 = backward
//10 = forward
//11 = brake

//1P = forward PWM
//P1 = backward PWM

//--------------------------------------------------------------
void motor1(int spd)
{
  if (spd == 0) //Brake
  {
    digitalWrite(m11, HIGH);
    digitalWrite(m12, HIGH);
    return;
  }
  if (spd == 255) //Forward full
  {
    digitalWrite(m11, HIGH);
    digitalWrite(m12, LOW);
    return;
  }
  if (spd == -255) //Backward full
  {
    digitalWrite(m11, LOW);
    digitalWrite(m12, HIGH);
    return;
  }
  if (spd > 0 && spd != 255) //Forward PWM
  {
    digitalWrite(m11, HIGH);
    analogWrite(m12, 255 - spd);
  }
  if (spd < 0 && spd != -255) //Backward PWM
  {
    analogWrite(m11, 255 + spd);
    digitalWrite(m12, HIGH);
  }
}

//---------------------------------------------------------------
void motor2(int spd)
{
  if (spd == 0) //Brake
  {
    digitalWrite(m21, HIGH);
    digitalWrite(m22, HIGH);
    return;
  }
  if (spd == 255) //Forward full
  {
    digitalWrite(m21, HIGH);
    digitalWrite(m22, LOW);
    return;
  }
  if (spd == -255) //Backward full
  {
    digitalWrite(m21, LOW);
    digitalWrite(m22, HIGH);
    return;
  }
  if (spd > 0 && spd != 255) //Forward PWM
  {
    digitalWrite(m21, HIGH);
    analogWrite(m22, 255 - spd);
  }
  if (spd < 0 && spd != -255) //Backward PWM
  {
    analogWrite(m21, 255 + spd);
    digitalWrite(m22, HIGH);
  }
}
//------------------------------------------------
void resetCount()
{
  count1 = 0;
  count2 = 0;
}

int getCount1()
{
  return count1;
}

int getCount2()
{
  return count2;
}

void ISR_encoder1()
{
  if ((long)(micros() - last_interrupt1) >= debouncetime * 1000)
  {
    count1++;
    last_interrupt1 = micros();
  }
}

void ISR_encoder2()
{
  if ((long)(micros() - last_interrupt2) >= debouncetime * 1000)
  {
    count2++;
    last_interrupt2 = micros();
  }
}
