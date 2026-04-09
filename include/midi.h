/*!
 *  \author Manoel McCadden
 *  \date   26-04-03
 *  \file   midi.h
 *  \brief
 *    Handles MIDI data interpretting
 */

#include "effect_envelope.h"
#include "synth_sine.h"
#include "synth_waveform.h"
#include "synth_wavetable.h"
#include <Audio.h>

#pragma once

namespace TIMS
{
  struct Note
  {
    Note(AudioSynthWaveform *p_sine, AudioSynthWaveformModulated *p_fm
        , AudioEffectEnvelope *p_adsr, const float &sineBase
        , const float &fmBase);

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

    AudioSynthWaveform *sine;
    AudioSynthWaveformModulated *fm;
    AudioEffectEnvelope *adsr;

    float baseSineFreq = 0.f;
    float sineFreq = 0.f;
    float sineAmplitude = 0.f;
    float baseFMFreq = 0.f;
    float fmFreq = 0.f;
    float fmAmplitude = 0.f;

    static inline float pitchWheelModifer = 0.f;

    static inline uint8_t NoteIterator = 0;

    void TurnOn();
    void TurnOff();
    void SetSineFrequency();
    void SetFMFrequency();
  };

  // NOTE: Will need to update the FindNote function in midi.cpp if this is
  // changed
  inline std::vector<TIMS::Note> notes;

  // Sine functions
  void SetSineFrequency(const float &frequency);
  void SetFMSineFrequency(const float &frequency);
  void SetSineAmplitude(const float &amplitude);
  void SetFMSineAmplitude(const float &amplitude);

  // ADSR functions
  void SetAttack(const float &attack);
  void SetDecay(const float &decay);
  void SetSustain(const float &sustain);
  void SetRelease(const float &release);

  void NoteOn(byte channel, byte note, byte velocity);
  void NoteOff(byte channel, byte note, byte velocity);
  void OnPitchWheel(uint8_t channel, int pitchBend);
}
