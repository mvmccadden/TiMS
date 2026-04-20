/*!
 *  \author Manoel McCadden
 *  \date   26-04-03
 *  \file   midi.h
 *  \brief
 *    Handles MIDI data interpretting
 */

#pragma once

#include <Audio.h>

namespace TIMS
{
  struct Note
  {
    public:
      Note(AudioSynthWaveform *p_sine, AudioSynthWaveformModulated *p_fmA
          , AudioSynthWaveformModulated *p_fmB , AudioEffectEnvelope *p_adsr);

      enum PLAYING_STATE
      {
        PS_OFF = 0
        , PS_ON
        , PS_STARTING
        , PS_STOPPING
      };

      void SetNote(const byte &note);
      byte GetNote();

      bool GetPlaying();
      const elapsedMillis &GetElapsedTime();
    
      void TurnOn();
      void TurnOff();

      void UpdateCarrierAFrequnecy();
      void UpdateCarrierBFrequnecy();
      void UpdateModulatorFrequnecy();
      
      void UpdateCarrierAAmplitude();
      void UpdateCarrierBAmplitude();
      void UpdateModulatorAmplitude();

      void UpdateAttack();
      void UpdateDecay();
      void UpdateSustain();
      void UpdateRelease();

      void ResetCarrierA();
      void ResetCarrierB();
      void ResetModulator();

      void InitalizeNote();

      static inline float modulatorFreq = 0.f;
      static inline float carrierAFreq = 0.f;
      static inline float carrierBFreq = 0.f;

      static inline float baseModulatorFreq = 440.f * std::pow(2, -68.f / 12.f);
      static inline float baseCarrierAFreq = 440.f;
      static inline float baseCarrierBFreq = 440.f;

      static inline float modulatorAmplitude = 0.f;
      static inline float carrierAAmplitude = 1.f;
      static inline float carrierBAmplitude = 1.f;

      static inline int carrierAWaveform = WAVEFORM_TRIANGLE;
      static inline int carrierBWaveform = WAVEFORM_TRIANGLE;
      static inline int modulatorWaveform = WAVEFORM_TRIANGLE;

      static inline float attack = 75.f;
      static inline float decay = 75.f;
      static inline float sustain = 0.7f;
      static inline float release = 75.f;

      static inline bool modulatorFollowsKeyboard = false;

      static inline float lowpassFreq = 440.f * std::pow(2, 68 / 12.f);
      static inline float lowpassResonance = 0.8f;

      static inline float pitchWheelModifer = 0.f;
      static inline uint8_t NoteIterator = 0;
      static inline AudioFilterLadder *ladder = nullptr;
    
    private:
      bool isPlaying = false;
      byte noteValue = 69;
      elapsedMillis timeSincePlayed = 0;

      // Modulator based on keyboard
      AudioSynthWaveform *modulator;
      // Carrier based on keyboard
      AudioSynthWaveformModulated *carrierA;
      // Carrier based on keyboard with tuning
      AudioSynthWaveformModulated *carrierB;

      // The ADSR filter for each note
      AudioEffectEnvelope *adsr;
  };

  // Contains all notes that can be played
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
  void SetFilterResonance(const float &resonance);

  // Midi Handling
  void NoteOn(byte channel, byte note, byte velocity);
  void NoteOff(byte channel, byte note, byte velocity);
  void OnPitchWheel(uint8_t channel, int pitchBend);

  // Initalization functions for notes
  void InitalizeNotes(AudioFilterLadder *ladder);
}
