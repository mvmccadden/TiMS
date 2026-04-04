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

typedef struct AmplitudeLerp AmplitudeLerp;

std::vector<AmplitudeLerp> amplitudeLerps;

struct AmplitudeLerp
{
  AmplitudeLerp(TIMS::Note *lerpNote, const float &lerpAmp, const float &lerpTime)
    : note(lerpNote), startingAmp(lerpNote->fmAmplitude), goalAmp(lerpAmp)
      , lerpTime(lerpTime)
  {
    amplitudeLerps.push_back(*this);
  }

  TIMS::Note *note;
  float startingAmp = 0.f;
  float goalAmp = 0.f;
  float lerpTime = 0.f;
  float timePassed = 0.f;
};


namespace TIMS
{
  std::array<TIMS::Note, NOTECOUNT>::iterator FindNote(const byte &note)
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

  std::array<TIMS::Note, 1>::iterator FindNextOpenNote()
  {
    for(auto it = notes.begin(); it != notes.end(); ++it)
    {
      if(it->isPlaying == false)
      {
        return it;
      }
    }

    return notes.begin();
  }

  void TIMS::Note::SetSineWaveform(AudioSynthWaveform *waveForm)
  {
    sine = waveForm;
  }

  void TIMS::Note::SetFMWaveform(AudioSynthWaveformSineModulated *waveForm)
  {
    fm = waveForm;
  }

  void TIMS::Note::SetSineFrequency()
  {
    sine->frequency(sineFreq);
  }

  void TIMS::Note::SetFMFrequency()
  {
    fmFreq = baseFMFreq * std::pow(2, static_cast<float>(value - 69) / 12.f);
    fm->frequency(fmFreq);
  }

  void TIMS::Note::TurnOn()
  {
    isPlaying = true;
    sine->amplitude(1);
    fm->amplitude(fmAmplitude);

    Serial.print("Playing note: ");
    Serial.print(value);
    Serial.print(" With ferquencies: ");
    Serial.print(sineFreq);
    Serial.print(", ");
    float testValue = std::pow(2, static_cast<float>(value - 69) / 12.f);
    Serial.print(testValue);
    Serial.print(", ");
    Serial.print(baseFMFreq);
    Serial.print(", ");
    Serial.println(fmFreq);
  }

  void TIMS::Note::TurnOff()
  {
    isPlaying = false;
    sine->amplitude(0);
    fmAmplitude = 0;
    fm->amplitude(fmAmplitude);
    Serial.print("Stopping note: ");
    Serial.println(value);
  }

  void SetSine(const float &frequency)
  {
    for(TIMS::Note &note : notes)
    {
      note.sineFreq = frequency;
      note.SetSineFrequency();
    }
  }

  void SetFMSine(const float &frequency)
  {
    for(TIMS::Note &note : notes)
    {
      note.baseFMFreq = frequency;
      note.SetFMFrequency();
    }
  }

  void NoteOn(byte channel, byte note, byte velocity)
  { 
    // This is an iterator of notes
    auto foundNote = FindNote(note);
   
    if(foundNote == notes.end())
    {
      foundNote = FindNextOpenNote();
    }

    foundNote->value = note;

    foundNote->SetFMFrequency();
    foundNote->TurnOn();

    // WARN: FORCING CLEAR RIGHT NOW THIS NEEDS TO BE FIXED TO REMOVE THE FOUND
    // NOTE
    amplitudeLerps.clear();
    AmplitudeLerp(foundNote, 1.f, 500);
  }

  void NoteOff(byte channel, byte note, byte velocity)
  {
    auto foundNote = FindNote(note);

    if(foundNote == notes.end())
    {
      return;
    }

    // WARN: FORCING CLEAR RIGHT NOW THIS NEEDS TO BE FIXED TO REMOVE THE FOUND
    // NOTE
    amplitudeLerps.clear();
    AmplitudeLerp(foundNote, 0.f, 500);
  }

  void TickAmplitudeLerp()
  {
    for(auto it = amplitudeLerps.begin(); it != amplitudeLerps.end(); ++it)
    {
      it->timePassed++;
      if(it->timePassed >= it->lerpTime)
      {
        // Turn off if we are lerping to 0
        if(it->goalAmp == 0)
        {
          it->note->TurnOff();
        }
        amplitudeLerps.erase(it);
        // TODO: replace this with proper handling of removal
        return;
      }
      else
      {
        it->note->fmAmplitude = it->startingAmp
          + (it->goalAmp - it->startingAmp) 
          * it->timePassed / it->lerpTime;

        it->note->fm->amplitude(it->note->fmAmplitude);
      }
    }
  }
}

