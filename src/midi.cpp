/*!
 *  \author Manoel McCadden
 *  \date   26-04-03
 *  \file   midi.cpp
 *  \brief
 *    Handles MIDI data interpretting
 */

#include "midi.h"
#include "synth_waveform.h"

namespace TIMS
{
  Note::Note(AudioSynthWaveform *p_sine, AudioSynthWaveformModulated *p_fmA
      , AudioSynthWaveformModulated *p_fmB, AudioEffectEnvelope *p_adsr
      , const float &sineBase, const float &fmABase, const float &fmBBase)
    : modulator(p_sine), carrierA(p_fmA), carrierB(p_fmB), adsr(p_adsr)
      , baseModulatorFreq(sineBase)
      , baseCarrierAFreq(fmABase), baseCarrierBFreq(fmBBase)
      , modulatorAmplitude(0), carrierAAmplitude(1), carrierBAmplitude(1)
      , carrierAWaveform(WAVEFORM_TRIANGLE)
      , carrierBWaveform(WAVEFORM_SAWTOOTH)
      , modulatorWaveform(WAVEFORM_TRIANGLE)
  {
    ResetCarrierA();
    ResetCarrierB();
    ResetModulator();

    UpdateCarrierAFrequnecy();
    UpdateCarrierBFrequnecy();
    UpdateModulatorFrequnecy();

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

  void TIMS::Note::TurnOn()
  {
    isPlaying = true;
    adsr->noteOn();

    Serial.print("Playing note: ");
    Serial.println(value);
    Serial.print("Carrier A Frequency: ");
    Serial.print(carrierAFreq);
    Serial.print(", Amplitude: ");
    Serial.println(carrierAAmplitude);
    Serial.print("Carrier B Frequency: ");
    Serial.print(carrierBFreq);
    Serial.print(", Amplitude: ");
    Serial.println(carrierBAmplitude);
    Serial.print("Modulator Frequency: ");
    Serial.print(modulatorFreq);
    Serial.print(", Amplitude: ");
    Serial.println(modulatorAmplitude);
    Serial.println("---------------------------------------------------------");
  }

  void TIMS::Note::TurnOff()
  {
    isPlaying = false;
    adsr->noteOff();
    
    // Removing this to clear clutter :)
    //Serial.print("Stopping note: ");
    //Serial.println(value);
  }

  void TIMS::Note::UpdateCarrierAFrequnecy()
  {
    carrierAFreq = baseCarrierAFreq * 
      std::pow(2, static_cast<float>(value - 69 + pitchWheelModifer) 
          / 12.f);

    carrierA->frequency(carrierAFreq);
  }

  void TIMS::Note::UpdateCarrierBFrequnecy()
  {
    carrierBFreq = baseCarrierBFreq * 
      std::pow(2, static_cast<float>(value - 69 + pitchWheelModifer) 
          / 12.f);

    carrierB->frequency(carrierBFreq);
  }

  void TIMS::Note::UpdateModulatorFrequnecy()
  {
    modulatorFreq = baseModulatorFreq;

    if(modulatorFollowsKeyboard == true)
    {
      modulatorFreq *= std::pow(2
          , static_cast<float>(value - 69 + pitchWheelModifer) / 12.f);
    }

    modulator->frequency(modulatorFreq);
  }

  void TIMS::Note::ResetCarrierA()
  {
    carrierA->begin(carrierAAmplitude, carrierAFreq, carrierAWaveform);
  }

  void TIMS::Note::ResetCarrierB()
  {
    carrierB->begin(carrierBAmplitude, carrierBFreq, carrierBWaveform);
  }

  void TIMS::Note::ResetModulator()
  {
    modulator->begin(modulatorAmplitude, modulatorFreq, modulatorWaveform);
  }

  void SetCarrierAFrequency(const float &frequency)
  {
    for(TIMS::Note &note : notes)
    {
      note.baseCarrierAFreq = frequency;
      note.UpdateCarrierAFrequnecy();
    }
  }

  void SetCarrierBFrequency(const float &frequency)
  {
    for(TIMS::Note &note : notes)
    {
      note.baseCarrierBFreq = frequency;
      note.UpdateCarrierBFrequnecy();
    }
  }

  void SetModulatorFrequency(const float &frequency)
  {
    for(TIMS::Note &note : notes)
    {
      note.baseModulatorFreq = frequency;
      note.UpdateModulatorFrequnecy();
    }
  }

  void SetCarrierAAmplitude(const float &amplitude)
  {
    for(TIMS::Note &note : notes)
    {
      note.carrierAAmplitude = amplitude;
      note.carrierA->amplitude(note.carrierAAmplitude);
    }
  }

  void SetCarrierBAmplitude(const float &amplitude)
  {
    for(TIMS::Note &note : notes)
    {
      note.carrierBAmplitude = amplitude;
      note.carrierB->amplitude(note.carrierBAmplitude);
    }
  }

  void SetModulatorAmplitude(const float &amplitude)
  {
    for(TIMS::Note &note : notes)
    {
      note.modulatorAmplitude = amplitude;
      note.modulator->amplitude(note.modulatorAmplitude);
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

  void SetCarrierAWaveform(const int &waveForm)
  {
    for(Note &note : notes)
    {
      note.carrierAWaveform = waveForm;
      note.ResetCarrierA();
    }
  }

  void SetCarrierBWaveform(const int &waveForm)
  {
    for(Note &note : notes)
    {
      note.carrierBWaveform = waveForm;
      note.ResetCarrierB();
    }
  }

  void SetModulatorWaveform(const int &waveForm)
  {
    for(Note &note : notes)
    {
      note.modulatorWaveform = waveForm;
      note.ResetModulator();
    }
  }

  void SetModulatorKeyboardFollowing(const bool &follow)
  {
    for(Note &note: notes)
    {
      note.modulatorFollowsKeyboard = follow;
      note.UpdateModulatorFrequnecy();
    }
  }

  void SetFilterFreq(const float &freq)
  {
    Note::ladder->frequency(freq);
  }

  void SetFilterOctave(const float &octave)
  {
    Note::ladder->octaveControl(octave);
  }

  void SetFilterResonance(const float &resonance)
  {
    Note::ladder->resonance(resonance);
  }

// =====================
//  MIDI NOTE HANDLING
// =====================

  void NoteOn(byte channel, byte note, byte velocity)
  { 
    // This is an iterator of the note vector, notes
    auto foundNote = FindNextOpenNote();

    foundNote->value = note;

    foundNote->UpdateCarrierAFrequnecy();
    foundNote->UpdateCarrierBFrequnecy();
    foundNote->UpdateModulatorFrequnecy();
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
  
    for(Note &note : notes)
    {
      note.UpdateCarrierAFrequnecy();
      note.UpdateCarrierBFrequnecy();
      note.UpdateModulatorFrequnecy();
    }
  }
}

