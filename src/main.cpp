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
#include "filter_ladder.h"
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
AudioSynthWaveformModulated fm1a;
AudioSynthWaveformModulated fm1b;
AudioEffectEnvelope adsr1;
AudioMixer4 fmSum1;
AudioSynthWaveform sine2;
AudioSynthWaveformModulated fm2a;
AudioSynthWaveformModulated fm2b;
AudioEffectEnvelope adsr2;
AudioMixer4 fmSum2;
AudioSynthWaveform sine3;
AudioSynthWaveformModulated fm3a;
AudioSynthWaveformModulated fm3b;
AudioEffectEnvelope adsr3;
AudioMixer4 fmSum3;
AudioSynthWaveform sine4;
AudioSynthWaveformModulated fm4a;
AudioSynthWaveformModulated fm4b;
AudioEffectEnvelope adsr4;
AudioMixer4 fmSum4;
AudioSynthWaveform sine5;
AudioSynthWaveformModulated fm5a;
AudioSynthWaveformModulated fm5b;
AudioEffectEnvelope adsr5;
AudioMixer4 fmSum5;
AudioSynthWaveform sine6;
AudioSynthWaveformModulated fm6a;
AudioSynthWaveformModulated fm6b;
AudioEffectEnvelope adsr6;
AudioMixer4 fmSum6;
AudioSynthWaveform sine7;
AudioSynthWaveformModulated fm7a;
AudioSynthWaveformModulated fm7b;
AudioEffectEnvelope adsr7;
AudioMixer4 fmSum7;
AudioSynthWaveform sine8;
AudioSynthWaveformModulated fm8a;
AudioSynthWaveformModulated fm8b;
AudioEffectEnvelope adsr8;
AudioMixer4 fmSum8;

// Mixer
AudioMixer4 mixer1;
AudioMixer4 mixer2;
AudioMixer4 mixer_master;

// Lowpass Ladder filter (meant to replicate the moog sound)
AudioFilterLadder lowpassLadder;

// Devices
AudioOutputI2S i2sOut;
AudioControlSGTL5000 sgtl5000;

// Patch cables
AudioConnection patchChord1a(sine1, 0, fm1a, 0);
AudioConnection patchChord1b(sine1, 0, fm1b, 0);
AudioConnection patchChord1mix_a(fm1a, 0, fmSum1, 0);
AudioConnection patchChord1mix_b(fm1b, 0, fmSum1, 1);
AudioConnection patchChord1env_a(fmSum1, 0, adsr1, 0);


AudioConnection patchChord2a(sine2, 0, fm2a, 0);
AudioConnection patchChord2b(sine2, 0, fm2b, 0);
AudioConnection patchChord2mix_a(fm2a, 0, fmSum2, 0);
AudioConnection patchChord2mix_b(fm2b, 0, fmSum2, 1);
AudioConnection patchChord2env_a(fmSum2, 0, adsr2, 0);

AudioConnection patchChord3a(sine3, 0, fm3a, 0);
AudioConnection patchChord3b(sine3, 0, fm3b, 0);
AudioConnection patchChord3mix_a(fm3a, 0, fmSum3, 0);
AudioConnection patchChord3mix_b(fm3b, 0, fmSum3, 1);
AudioConnection patchChord3env_a(fmSum3, 0, adsr3, 0);

AudioConnection patchChord4a(sine4, 0, fm4a, 0);
AudioConnection patchChord4b(sine4, 0, fm4b, 0);
AudioConnection patchChord4mix_a(fm4a, 0, fmSum4, 0);
AudioConnection patchChord4mix_b(fm4b, 0, fmSum4, 1);
AudioConnection patchChord4env_a(fmSum4, 0, adsr4, 0);

AudioConnection patchChord5a(sine5, 0, fm5a, 0);
AudioConnection patchChord5b(sine5, 0, fm5b, 0);
AudioConnection patchChord5mix_a(fm5a, 0, fmSum5, 0);
AudioConnection patchChord5mix_b(fm5b, 0, fmSum5, 1);
AudioConnection patchChord5env_a(fmSum5, 0, adsr5, 0);

AudioConnection patchChord6a(sine6, 0, fm6a, 0);
AudioConnection patchChord6b(sine6, 0, fm6b, 0);
AudioConnection patchChord6mix_a(fm6a, 0, fmSum6, 0);
AudioConnection patchChord6mix_b(fm6b, 0, fmSum6, 1);
AudioConnection patchChord6env_a(fmSum6, 0, adsr6, 0);

AudioConnection patchChord7a(sine7, 0, fm7a, 0);
AudioConnection patchChord7b(sine7, 0, fm7b, 0);
AudioConnection patchChord7mix_a(fm7a, 0, fmSum7, 0);
AudioConnection patchChord7mix_b(fm7b, 0, fmSum7, 1);
AudioConnection patchChord7env_a(fmSum7, 0, adsr7, 0);

AudioConnection patchChord8a(sine8, 0, fm8a, 0);
AudioConnection patchChord8b(sine8, 0, fm8b, 0);
AudioConnection patchChord8mix_a(fm8a, 0, fmSum8, 0);
AudioConnection patchChord8mix_b(fm8b, 0, fmSum8, 1);
AudioConnection patchChord8env_a(fmSum8, 0, adsr8, 0);

AudioConnection patchChordmix1_1(adsr1, 0, mixer1, 0);
AudioConnection patchChordmix1_2(adsr2, 0, mixer1, 1);
AudioConnection patchChordmix1_3(adsr3, 0, mixer1, 2);
AudioConnection patchChordmix1_4(adsr4, 0, mixer1, 3);
AudioConnection patchChordmix2_1(adsr5, 0, mixer2, 0);
AudioConnection patchChordmix2_2(adsr6, 0, mixer2, 1);
AudioConnection patchChordmix2_3(adsr7, 0, mixer2, 2);
AudioConnection patchChordmix2_4(adsr8, 0, mixer2, 3);

// Master input from 2 child mixers
AudioConnection patchChordMasterIn1(mixer1, 0, mixer_master, 0);
AudioConnection patchChordMasterIn2(mixer2, 0, mixer_master, 1);

// Moog-style low-pass filter (ladder)
AudioConnection patchChordLadder(mixer_master, 0, lowpassLadder, 0);

// Aux final output
AudioConnection patchChordAuxOut1(lowpassLadder, 0, i2sOut, 0);
AudioConnection patchChordAuxOut2(lowpassLadder, 0, i2sOut, 1);

// BUG: Note 7 will replay itself when 7 notes are pressed and note 8 wont play?

// Use hardware SPI (#13, #12, #11) and the above for CS/DC
//ILI9341_t3 tft = ILI9341_t3 (TFT_CS, TFT_DC);
//XPT2046_Touchscreen ts(XPT_CS);

void setup()
{
  // Turn on LED to signify testing
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);

  AudioMemory(12);

  // Set mixer volume based on number of inputs
  for(int i = 0; i < 4; ++i)
  {
    mixer1.gain(i, 0.25);
    mixer2.gain(i, 0.25);

    if(i % 2 == 0) mixer_master.gain(i / 2, 0.5);
  }

  // Set the ladder filter for the midi notes
  TIMS::Note::ladder = &lowpassLadder; 
  TIMS::SetFilterFreq(440.f * std::pow(2, (127 - 69) / 12.f));
  TIMS::SetFilterOctave(0);
  TIMS::SetFilterResonance(0.8f);

  // Assign notes
  TIMS::notes.push_back(TIMS::Note(&sine1, &fm1a, &fm1b, &adsr1
        , 440, 440, 440));
  TIMS::notes.push_back(TIMS::Note(&sine2, &fm2a, &fm2b, &adsr2
        , 440, 440, 440));
  TIMS::notes.push_back(TIMS::Note(&sine3, &fm3a, &fm3b, &adsr3
        , 440, 440, 440));
  TIMS::notes.push_back(TIMS::Note(&sine4, &fm4a, &fm4b, &adsr4
        , 440, 440, 440));
  TIMS::notes.push_back(TIMS::Note(&sine5, &fm5a, &fm5b, &adsr5
        , 440, 440, 440));
  TIMS::notes.push_back(TIMS::Note(&sine6, &fm6a, &fm6b, &adsr6
        , 440, 440, 440));
  TIMS::notes.push_back(TIMS::Note(&sine7, &fm7a, &fm7b, &adsr7
        , 440, 440, 440));
  TIMS::notes.push_back(TIMS::Note(&sine8, &fm8a, &fm8b, &adsr8
        , 440, 440, 440));

  // Send power to usb host
  usbHost.begin();

  // Set midi handlers
  midiReader.setHandleNoteOn(TIMS::NoteOn);
  midiReader.setHandleNoteOff(TIMS::NoteOff);
  midiReader.setHandlePitchChange(TIMS::OnPitchWheel);
  // TODO: Add pitch and mod wheel

  // Enable aux output
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
