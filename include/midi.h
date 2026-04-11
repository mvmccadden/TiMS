/*!
 *  \author Manoel McCadden
 *  \date   26-04-03
 *  \file   midi.h
 *  \brief
 *    Handles MIDI data interpretting
 */

#include "synth_waveform.h"
#include <Audio.h>

#pragma once

namespace TIMS
{
  struct Note
  {
    Note(AudioSynthWaveform *p_sine, AudioSynthWaveformModulated *p_fmA
        , AudioSynthWaveformModulated *p_fmB , AudioEffectEnvelope *p_adsr
        , const float &sineBase, const float &fmABase, const float &fmBBase);

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

    // Modulator based on keyboard
    // NOTE: May want to not make based on keyboard or make a switch control?
    AudioSynthWaveform *modulator;
    // Carrier based on keyboard
    AudioSynthWaveformModulated *carrierA;
    // Carrier based on keyboard with tuning
    AudioSynthWaveformModulated *carrierB;


    AudioEffectEnvelope *adsr;

    float modulatorFreq = 0.f;
    float carrierAFreq = 0.f;
    float carrierBFreq = 0.f;

    float baseModulatorFreq = 0.f;
    float baseCarrierAFreq = 0.f;
    float baseCarrierBFreq = 0.f;

    float modulatorAmplitude = 0.f;
    float carrierAAmplitude = 0.f;
    float carrierBAmplitude = 0.f;

    int carrierAWaveform = WAVEFORM_TRIANGLE;
    int carrierBWaveform = WAVEFORM_SAWTOOTH;
    int modulatorWaveform = WAVEFORM_TRIANGLE;

    bool modulatorFollowsKeyboard = false;

    static inline float pitchWheelModifer = 0.f;
    static inline uint8_t NoteIterator = 0;
    static inline AudioFilterLadder *ladder = nullptr;

    void TurnOn();
    void TurnOff();

    void UpdateCarrierAFrequnecy();
    void UpdateCarrierBFrequnecy();
    void UpdateModulatorFrequnecy();

    void ResetCarrierA();
    void ResetCarrierB();
    void ResetModulator();
  };

  // NOTE: Will need to update the FindNote function in midi.cpp if this is
  // changed
  inline std::vector<TIMS::Note> notes;

  // Amplitude and Frequency functions
  void SetCarrierAFrequency(const float &frequency);
  void SetCarrierBFrequency(const float &frequency);
  void SetModulatorFrequency(const float &frequency);

  void SetCarrierAAmplitude(const float &amplitude);
  void SetCarrierBAmplitude(const float &amplitude);
  void SetModulatorAmplitude(const float &amplitude);

  // Waveform
  void SetCarrierAWaveform(const int &waveForm);
  void SetCarrierBWaveform(const int &waveForm);
  void SetModulatorWaveform(const int &waveForm);

  // EncoderPressed
  void SetModulatorKeyboardFollowing(const bool &follow);

  // ADSR functions
  void SetAttack(const float &attack);
  void SetDecay(const float &decay);
  void SetSustain(const float &sustain);
  void SetRelease(const float &release);

  // Filter functions
  void SetFilterFreq(const float &freq);
  void SetFilterOctave(const float &octave);
  void SetFilterResonance(const float &resonance);

  void NoteOn(byte channel, byte note, byte velocity);
  void NoteOff(byte channel, byte note, byte velocity);
  void OnPitchWheel(uint8_t channel, int pitchBend);
}
