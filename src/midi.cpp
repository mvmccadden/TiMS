/*!
 *  \author Manoel McCadden
 *  \date   26-04-03
 *  \file   midi.cpp
 *  \brief
 *    Handles MIDI data interpretting
 */

#include "midi.h"
#include "synth_sine.h"
#include "synth_waveform.h"
#include "wiring.h"


namespace TIMS
{
  Note::Note(AudioSynthWaveform *p_sine, AudioSynthWaveformModulated *p_fm
      , AudioEffectEnvelope *p_adsr, const float &sineBase
      , const float &fmBase)
    : sine(p_sine), fm(p_fm), adsr(p_adsr), baseSineFreq(sineBase)
      , baseFMFreq(fmBase)
  {
    sine->begin(0, baseSineFreq, WAVEFORM_SQUARE);
    fm->begin(1, baseFMFreq, WAVEFORM_SAWTOOTH);

    SetSineFrequency();
    SetFMFrequency();

    adsr->delay(0);
    adsr->hold(0);
    adsr->attack(100);
    adsr->decay(200);
    adsr->sustain(0.7);
    adsr->release(200);
  }

  std::vector<TIMS::Note>::iterator FindNote(const byte &note)
  {
    for(auto it = notes.begin(); it != notes.end(); ++it)
    {
      if(it->value == note)
      {
        return it;
      }
    }

    return notes.end();
  }

  std::vector<TIMS::Note>::iterator FindNextOpenNote()
  {
    // Check all notes to see if any arn't playing. If there arn't any open then
    // select the note at the NoteIterator value and increment it so that it
    // cycles.
    for(auto it = notes.begin(); it != notes.end(); ++it)
    {
      if(it->isPlaying == false)
      {
        return it;
      }
    }

    // Get the next midi note in the cycle
    auto it = notes.begin() + Note::NoteIterator++;
    // If we go beyond the number of notes we have, cycle back to 0
    if(Note::NoteIterator >= notes.size()) 
    {
      Note::NoteIterator = 0;
    }

    return it;
  }

  void TIMS::Note::SetSineFrequency()
  {
    sineFreq = baseSineFreq * std::pow(2
        , static_cast<float>(value - 69 + pitchWheelModifer) / 12.f);
    sine->frequency(sineFreq);
  }

  void TIMS::Note::SetFMFrequency()
  {
    fmFreq = baseFMFreq * std::pow(2
        , static_cast<float>(value - 69 + pitchWheelModifer) / 12.f);
    fm->frequency(fmFreq);
  }

  void TIMS::Note::TurnOn()
  {
    isPlaying = true;
    adsr->noteOn();

    Serial.print("Playing note: ");
    Serial.print(value);
    Serial.print(" With ferquencies: ");
    Serial.print(baseSineFreq);
    Serial.print(", ");
    Serial.print(sineFreq);
    Serial.print(", ");
    Serial.print(baseFMFreq);
    Serial.print(", ");
    Serial.println(fmFreq);
  }

  void TIMS::Note::TurnOff()
  {
    isPlaying = false;
    adsr->noteOff();

    Serial.print("Stopping note: ");
    Serial.println(value);
  }

  void SetSineFrequency(const float &frequency)
  {
    for(TIMS::Note &note : notes)
    {
      note.baseSineFreq = frequency;
      note.SetSineFrequency();
    }
  }

  void SetFMSineFrequency(const float &frequency)
  {
    for(TIMS::Note &note : notes)
    {
      note.baseFMFreq = frequency;
      note.SetFMFrequency();
    }
  }

  void SetSineAmplitude(const float &amplitude)
  {
    for(TIMS::Note &note : notes)
    {
      note.sineAmplitude = amplitude;
      note.sine->amplitude(note.sineAmplitude);
    }
  }

  void SetFMSineAmplitude(const float &amplitude)
  {
    for(TIMS::Note &note : notes)
    {
      note.fmAmplitude = amplitude;
      note.fm->amplitude(note.fmAmplitude);
    }
  }

  void SetAttack(const float &attack)
  {
    for(TIMS::Note &note : notes)
    {
      note.adsr->attack(attack);
    }
  }

  void SetDecay(const float &decay)
  {
    for(TIMS::Note &note : notes)
    {
      note.adsr->decay(decay);
    }
  }

  void SetSustain(const float &sustain)
  {
    for(TIMS::Note &note : notes)
    {
      note.adsr->sustain(sustain);
    }
  }

  void SetRelease(const float &release)
  {
    for(TIMS::Note &note : notes)
    {
      note.adsr->release(release);
    }
  }

  void NoteOn(byte channel, byte note, byte velocity)
  { 
    // This is an iterator of the note vector, notes
    auto foundNote = FindNextOpenNote();

    foundNote->value = note;

    foundNote->SetFMFrequency();
    foundNote->SetSineFrequency();
    foundNote->TurnOn();
  }

  void NoteOff(byte channel, byte note, byte velocity)
  {
    auto foundNote = FindNote(note);

    if(foundNote == notes.end())
    {
      return;
    }

    foundNote->TurnOff();
  }

  void OnPitchWheel(uint8_t channel, int pitchBend)
  {
    Note::pitchWheelModifer = pitchBend / 8192.f;

    for(auto it = notes.begin(); it != notes.end(); ++it)
    {
      it->SetSineFrequency();
      it->SetFMFrequency();
    }
  }
}

