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
#include "effect_envelope.h"
#include "mixer.h"
#include "pins_arduino.h"
#include "usb_serial.h"
#include "USBHost_t36.h"
#include "midi.h"
#include "menu.h"

/*
// TFT pins
const uint8_t TFT_DC = 9;
const uint8_t TFT_CS = 10;
const uint8_t XPT_CS = 8;
*/

// Potentiometer values
int pot_a0 = 0;

// USB params
USBHost usbHost;
USBHub usbHub(usbHost);
MIDIDevice midiReader(usbHub);

// Patching

// Notes
AudioSynthWaveform sine1;
AudioSynthWaveformModulated fm1;
AudioEffectEnvelope adsr1;
AudioSynthWaveform sine2;
AudioSynthWaveformModulated fm2;
AudioEffectEnvelope adsr2;
AudioSynthWaveform sine3;
AudioSynthWaveformModulated fm3;
AudioEffectEnvelope adsr3;
AudioSynthWaveform sine4;
AudioSynthWaveformModulated fm4;
AudioEffectEnvelope adsr4;
AudioSynthWaveform sine5;
AudioSynthWaveformModulated fm5;
AudioEffectEnvelope adsr5;
AudioSynthWaveform sine6;
AudioSynthWaveformModulated fm6;
AudioEffectEnvelope adsr6;
AudioSynthWaveform sine7;
AudioSynthWaveformModulated fm7;
AudioEffectEnvelope adsr7;
AudioSynthWaveform sine8;
AudioSynthWaveformModulated fm8;
AudioEffectEnvelope adsr8;

// Mixer
AudioMixer4 mixer1;
AudioMixer4 mixer2;
AudioMixer4 mixer_master;

// Devices
AudioOutputI2S i2sOut;
AudioControlSGTL5000 sgtl5000;

// Patch cables
AudioConnection patchChord1(sine1, 0, fm1, 0);
AudioConnection patchChord2(fm1, 0, adsr1, 0);
AudioConnection patchChord3(sine2, 0, fm2, 0);
AudioConnection patchChord4(fm2, 0, adsr2, 0);
AudioConnection patchChord5(sine3, 0, fm3, 0);
AudioConnection patchChord6(fm3, 0, adsr3, 0);
AudioConnection patchChord7(sine4, 0, fm4, 0);
AudioConnection patchChord8(fm4, 0, adsr4, 0);
AudioConnection patchChord9(sine5, 0, fm5, 0);
AudioConnection patchChord10(fm5, 0, adsr5, 0);
AudioConnection patchChord11(sine6, 0, fm6, 0);
AudioConnection patchChord12(fm6, 0, adsr6, 0);
AudioConnection patchChord13(sine7, 0, fm7, 0);
AudioConnection patchChord14(fm7, 0, adsr7, 0);
AudioConnection patchChord15(sine8, 0, fm8, 0);
AudioConnection patchChord16(fm8, 0, adsr8, 0);
AudioConnection patchChord17(adsr1, 0, mixer1, 0);
AudioConnection patchChord18(adsr2, 0, mixer1, 1);
AudioConnection patchChord19(adsr3, 0, mixer1, 2);
AudioConnection patchChord20(adsr4, 0, mixer1, 3);
AudioConnection patchChord21(adsr5, 0, mixer2, 0);
AudioConnection patchChord22(adsr6, 0, mixer2, 1);
AudioConnection patchChord23(adsr7, 0, mixer2, 2);
AudioConnection patchChord24(adsr8, 0, mixer2, 3);
AudioConnection patchChord25(mixer1, 0, mixer_master, 0);
AudioConnection patchChord26(mixer2, 0, mixer_master, 1);
AudioConnection patchChord27(mixer_master, 0, i2sOut, 0);
AudioConnection patchChord28(mixer_master, 0, i2sOut, 1);

// Use hardware SPI (#13, #12, #11) and the above for CS/DC
//ILI9341_t3 tft = ILI9341_t3 (TFT_CS, TFT_DC);
//XPT2046_Touchscreen ts(XPT_CS);

void setup()
{
  // Turn on LED to signify testing
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);

  AudioMemory(12);

  for(int i = 0; i < 4; ++i)
  {
    mixer1.gain(i, 0.25);
    mixer2.gain(i, 0.25);

    if(i % 2 == 0) mixer_master.gain(i / 2, 0.5);
  }

  TIMS::notes.push_back(TIMS::Note(&sine1, &fm1, &adsr1, 440, 440));
  TIMS::notes.push_back(TIMS::Note(&sine2, &fm2, &adsr2, 440, 440));
  TIMS::notes.push_back(TIMS::Note(&sine3, &fm3, &adsr3, 440, 440));
  TIMS::notes.push_back(TIMS::Note(&sine4, &fm4, &adsr4, 440, 440));
  TIMS::notes.push_back(TIMS::Note(&sine5, &fm5, &adsr5, 440, 440));
  TIMS::notes.push_back(TIMS::Note(&sine6, &fm6, &adsr6, 440, 440));
  TIMS::notes.push_back(TIMS::Note(&sine7, &fm7, &adsr7, 440, 440));
  TIMS::notes.push_back(TIMS::Note(&sine8, &fm8, &adsr8, 440, 440));

  usbHost.begin();
  midiReader.setHandleNoteOn(TIMS::NoteOn);
  midiReader.setHandleNoteOff(TIMS::NoteOff);
  midiReader.setHandlePitchChange(TIMS::OnPitchWheel);
  // TODO: Add pitch and mod wheel

  sgtl5000.enable();
  sgtl5000.volume(0.0);

  // Setup usb sine out

  /*
  // Test if the screen can properly change color
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen (ILI9341_YELLOW);
  */
}

void loop()
{
  // Poll for midi inputs
  usbHost.Task();
  midiReader.read();

  // Update pot value and volume
  pot_a0 = analogRead(A0);
  sgtl5000.volume(static_cast<float>(pot_a0) / 1023.f);

  // Update input/menu logic
  UpdateEncoderButtons();
  LoopMenu();

  //tft.fillScreen (ILI9341_YELLOW);

}
