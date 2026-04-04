/*! 
 *  \author Manoel McCadden
 *  \date   26-03-14
 *  \file   main.cpp
 *
 *  \brief
 *    Tests Teensy 4.1 by having the LED blind at variable tempos
 */

#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <algorithm>
#include <cmath>
#include "AudioControl.h"
#include "AudioStream.h"
#include "ILI9341_t3.h"
#include <XPT2046_Touchscreen.h>
#include "core_pins.h"
#include "pins_arduino.h"
#include "usb_serial.h"
#include <Encoder.h>
#include "USBHost_t36.h"
#include "midi.h"

/*
// TFT pins
const uint8_t TFT_DC = 9;
const uint8_t TFT_CS = 10;
const uint8_t XPT_CS = 8;
*/

// Potentiometer values
int pot_a0 = 0;

// Encoder 
Encoder encoder1(30,31);
const int encoder1Button = 32;
long encoder1Value = 0;
Encoder encoder2(34,35);
const int encoder2Button = 33;
long encoder2Value = 0;

uint16_t sineValue = 256;
uint16_t fmValue = 256;

// USB params
USBHost usbHost;
USBHub usbHub(usbHost);
MIDIDevice midiReader(usbHub);

// Patching
AudioSynthWaveform sine1;
AudioSynthWaveformSineModulated sine_fm1;
AudioOutputI2S i2sOut;
AudioControlSGTL5000 sgtl5000;
AudioConnection patchChord1(sine1, 0, sine_fm1, 0);
AudioConnection patchChord2(sine_fm1, 0, i2sOut, 0);
AudioConnection patchChord3(sine_fm1, 0, i2sOut, 1);

// Use hardware SPI (#13, #12, #11) and the above for CS/DC
//ILI9341_t3 tft = ILI9341_t3 (TFT_CS, TFT_DC);
//XPT2046_Touchscreen ts(XPT_CS);

void setup()
{
  // Turn on LED to signify testing
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);

  AudioMemory(12);

  sine1.amplitude(0);
  sine_fm1.amplitude(0);

  TIMS::notes[0].SetSineWaveform(&sine1);
  TIMS::notes[0].SetFMWaveform(&sine_fm1);
  TIMS::notes[0].sineFreq = 440;
  TIMS::notes[0].baseFMFreq = 440;

  usbHost.begin();
  midiReader.setHandleNoteOn(TIMS::NoteOn);
  midiReader.setHandleNoteOff(TIMS::NoteOff);

  sgtl5000.enable();
  sgtl5000.volume(0.0);

  // Setup encoder buttons
  pinMode(encoder1Button, INPUT_PULLUP);
  pinMode(encoder2Button, INPUT_PULLUP);

  // Setup usb sine out

  /*
  // Test if the screen can properly change color
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen (ILI9341_YELLOW);
  */
}

bool updateEncoderValue(Encoder &encoder, uint16_t &encoderValue)
{
  long value = encoder.readAndReset();

  if(value == 0) return false;

  if(abs(value) > encoderValue && value < 0) 
  {
    encoderValue = 0;
  }
  else 
  {
    encoderValue += value;
  }

  if(encoderValue > 1023) 
  {
    encoderValue = 1023;
  }

  return true;
}

bool isEncoderButtonDown(Encoder &encoder, const int &encoderButton)
{
  return digitalRead(encoderButton) == LOW;
}

void loop()
{
  // Poll for midi inputs
  usbHost.Task();
  midiReader.read();

  TIMS::TickAmplitudeLerp();

  // Update pot value and volume
  pot_a0 = analogRead(A0);
  sgtl5000.volume(static_cast<float>(pot_a0) / 1023.f);

  if(updateEncoderValue(encoder1, sineValue))
  {
    float value = 20.f * std::pow(20000.f / 20.f, (static_cast<float>(sineValue) / 512.f));
    TIMS::SetSine(value);
  }
  if(updateEncoderValue(encoder2, fmValue))
  {
    float value = 20.f * std::pow(20000.f / 20.f, (static_cast<float>(fmValue) / 512.f));
    TIMS::SetFMSine(value);
  }

  //tft.fillScreen (ILI9341_YELLOW);
}
