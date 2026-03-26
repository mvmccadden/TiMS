/*! 
 *  \author Manoel McCadden
 *  \date   26-03-14
 *  \file   main.cpp
 *
 *  \brief
 *    Tests Teensy 4.1 by having the LED blind at variable tempos
 */

/* Testing LED
// Ignore the header errors as they are hallucinations from arduino and
// teensy... there might be a way to solve them but it isn't worth the time or
// effect when things compile


constexpr int DELAYVALUE = 200;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("LED ON");
  delay(DELAYVALUE);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("LED OFF");
  delay(DELAYVALUE);
}
*/

#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <cmath>
#include "AudioControl.h"
#include "AudioStream.h"
#include "ILI9341_t3.h"
#include "core_pins.h"
#include "usb_serial.h"

// TFT pins
const uint8_t TFT_DC = 9;
const uint8_t TFT_CS = 10;

// Potentiometer values
int pot_a0 = 0;
int pot_a1 = 0;
int pot_a2 = 0;

AudioSynthWaveform sine1;
AudioSynthWaveformSineModulated sine_fm1;
AudioOutputUSB usb1;
AudioConnection patchChord1(sine1, 0, sine_fm1, 0);
AudioConnection patchChord2(sine_fm1, 0, usb1, 0);
AudioConnection patchChord3(sine_fm1, 0, usb1, 1);

// Use hardware SPI (#13, #12, #11) and the above for CS/DC
//ILI9341_t3 tft = ILI9341_t3 (TFT_CS, TFT_DC);

void setup()
{
  // Turn on LED to signify testing
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(9600);

  delay(250);
  AudioMemory(12);
  sine1.frequency(440);
  sine1.amplitude(0.2);
  sine_fm1.frequency(440);
  sine_fm1.amplitude(0.2);
  delay(250);

  // Setup usb sine out

  /*
  // Test if the screen can properly change color
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen (ILI9341_YELLOW);
  */
}

bool updatePot(const uint8_t &potNumber, int &potValue)
{
  int value = analogRead(potNumber);
  if(potValue != value)
  {
    potValue = value;
    Serial.print("Analog ");
    Serial.print(potNumber);
    Serial.print(" : ");
    Serial.println(potValue);

    return true;
  }

  return false;
}

void loop()
{
  // Update pot values and their corresponding properties
  if(updatePot(A0, pot_a0))
  {
    // Map pot value to 0Hz-20kHz
    float value = 20.f * std::pow(20000.f / 20.f, (static_cast<float>(pot_a0) / 1023.f));
    sine1.frequency(value);

    Serial.print("Sine frequency: ");
    Serial.println(value);
  }
  if(updatePot(A1, pot_a1))
  {
    // Map pot value to 0Hz-20kHz
    float value = 20.f * std::pow(20000.f / 20.f, (static_cast<float>(pot_a1) / 1023.f));
    sine_fm1.frequency(value);

    Serial.print("FM Sine frequency: ");
    Serial.println(value);
  }
  if(updatePot(A2, pot_a2))
  {
    float value = static_cast<float>(pot_a2) / 1023.f;
    sine1.amplitude(value);
    sine_fm1.amplitude(value);

    Serial.print("Sine amplitudes: ");
    Serial.println(value);
  }
}

