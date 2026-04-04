/*!
 *  \author Manoel McCadden
 *  \date   26-04-03
 *  \file   midi.h
 *  \brief
 *    Handles MIDI data interpretting
 */

#include <Audio.h>

#pragma once

namespace TIMS
{
  struct Note
  {
    enum PLAYING_STATE
    {
      PS_OFF = 0
      , PS_ON
      , PS_STARTING
      , PS_STOPPING
    };
  
    // TODO: Update this with playing state instead of bool
    bool isPlaying = false;
    byte value = 69;
    float sineFreq = 0.f;
    float baseFMFreq = 0.f;
    float fmFreq = 0.f;
    float fmAmplitude = 0.f;
    AudioSynthWaveform *sine;
    AudioSynthWaveformSineModulated *fm;

    void SetSineWaveform(AudioSynthWaveform *waveform);
    void SetFMWaveform(AudioSynthWaveformSineModulated *waveform);
    void TurnOn();
    void TurnOff();
    void SetSineFrequency();
    void SetFMFrequency();
  };

  // NOTE: Will need to update the FindNote function in midi.cpp if this is
  // changed
  constexpr uint8_t NOTECOUNT = 1;
  inline std::array<TIMS::Note, NOTECOUNT> notes;

  void SetSine(const float &frequency);
  void SetFMSine(const float &frequency);

  void NoteOn(byte channel, byte note, byte velocity);
  void NoteOff(byte channel, byte note, byte velocity);

  void TickAmplitudeLerp();
}
