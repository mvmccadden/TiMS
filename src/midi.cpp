/ !
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

//=================
//  Note Methods 
//=================
 
  Note::Note(AudioSynthWaveform *p_sine, AudioSynthWaveformModulated *p_fmA
      , AudioSynthWaveformModulated *p_fmB, AudioEffectEnvelope *p_adsr)
    : modulator(p_sine), carrierA(p_fmA), carrierB(p_fmB), adsr(p_adsr) { }

  void Note::SetNote(const byte &note)
  {
    noteValue = note;
  }

  byte Note::GetNote()
  {
    return noteValue;
  }

  bool Note::GetPlaying()
  {
    return isPlaying;
  }

  const elapsedMillis &Note::GetElapsedTime()
  {
    return timeSincePlayed;
  }

  void Note::TurnOn()
  {
    isPlaying = true;
    timeSincePlayed = 0;
    adsr->noteOn();
    
    // NOTE: Removing midi debug data for screen output
    /*
    Serial.print("Playing note: ");
    Serial.println(noteValue);
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
    */
  }

  void Note::TurnOff()
  {
    isPlaying = false;
    adsr->noteOff();
        
    // Removing this to clear clutter :)
    //Serial.print("Stopping note: ");
    //Serial.println(value);
  }

  void Note::UpdateCarrierAFrequnecy()
  {
    carrierAFreq = baseCarrierAFreq * 
      std::pow(2, static_cast<float>(noteValue - 69 + pitchWheelModifer) 
          / 12.f);

    carrierA->frequency(carrierAFreq);
  }

  void Note::UpdateCarrierBFrequnecy()
  {
    carrierBFreq = baseCarrierBFreq * 
      std::pow(2, static_cast<float>(noteValue - 69 + pitchWheelModifer) 
          / 12.f);

    carrierB->frequency(carrierBFreq);
  }

  void Note::UpdateModulatorFrequnecy()
  {
    modulatorFreq = baseModulatorFreq;

    if(modulatorFollowsKeyboard == true)
    {
      modulatorFreq *= std::pow(2
          , static_cast<float>(noteValue - 69 + pitchWheelModifer) / 12.f);
    }

    modulator->frequency(modulatorFreq);
  }

  void Note::UpdateCarrierAAmplitude()
  {
    carrierA->amplitude(carrierAAmplitude);
  }

  void Note::UpdateCarrierBAmplitude()
  {
    carrierB->amplitude(carrierBAmplitude);
  }

  void Note::UpdateModulatorAmplitude()
  {
    modulator->amplitude(modulatorAmplitude);
  }

  void Note::UpdateAttack()
  {
    adsr->attack(attack);
  }
  
  void Note::UpdateDecay()
  {
    adsr->decay(decay);
  }

  void Note::UpdateSustain()
  {
    adsr->sustain(sustain);
  }
  
  void Note::UpdateRelease()
  {
    adsr->release(release);
  }

  void Note::ResetCarrierA()
  {
    carrierA->begin(carrierAAmplitude, carrierAFreq, carrierAWaveform);
  }

  void Note::ResetCarrierB()
  {
    carrierB->begin(carrierBAmplitude, carrierBFreq, carrierBWaveform);
  }

  void Note::ResetModulator()
  {
    modulator->begin(modulatorAmplitude, modulatorFreq, modulatorWaveform);
  }

  void Note::InitalizeNote()
  {
    ResetCarrierA();
    ResetCarrierB();
    ResetModulator();

    UpdateCarrierAFrequnecy();
    UpdateCarrierBFrequnecy();
    UpdateModulatorFrequnecy();

    UpdateCarrierAAmplitude();
    UpdateCarrierBAmplitude();
    UpdateModulatorAmplitude();

    adsr->delay(0);
    adsr->hold(0);
    UpdateAttack();
    UpdateDecay();
    UpdateSustain();
    UpdateRelease();
  }

//=========================
//  Public Set Functions
//=========================

  void SetCarrierAFrequency(const float &frequency)
  {
    Note::baseCarrierAFreq = frequency;

    for(Note &note : notes)
    {
      note.UpdateCarrierAFrequnecy();
    }
  }

  void SetCarrierBFrequency(const float &frequency)
  {
    Note::baseCarrierBFreq = frequency;

    for(Note &note : notes)
    {
      note.UpdateCarrierBFrequnecy();
    }
  }

  void SetModulatorFrequency(const float &frequency)
  {
    Note::baseModulatorFreq = frequency;

    for(Note &note : notes)
    {
      note.UpdateModulatorFrequnecy();
    }
  }

  void SetCarrierAAmplitude(const float &amplitude)
  {
    Note::carrierAAmplitude = amplitude;

    for(Note &note : notes)
    {
      note.UpdateCarrierAAmplitude();
    }
  }

  void SetCarrierBAmplitude(const float &amplitude)
  {
    for(TIMS::Note &note : notes)
    {
      note.carrierBAmplitude = amplitude;
      note.UpdateCarrierBAmplitude();
    }
  }

  void SetModulatorAmplitude(const float &amplitude)
  {
    for(Note &note : notes)
    {
      note.modulatorAmplitude = amplitude;
      note.UpdateModulatorAmplitude();
    }
  }

  void SetAttack(const float &attack)
  {
    Note::attack = attack;

    for(Note &note : notes)
    {
      note.UpdateAttack();
    }
  }

  void SetDecay(const float &decay)
  {
    Note::decay = decay;

    for(Note &note : notes)
    {
      note.UpdateDecay();
    }
  }

  void SetSustain(const float &sustain)
  {
    Note::sustain = sustain;

    for(Note &note : notes)
    {
      note.UpdateSustain();
    }
  }

  void SetRelease(const float &release)
  {
    Note::release = release;

    for(Note &note : notes)
    {
      note.UpdateRelease();
    }
  }

  void SetCarrierAWaveform(const int &waveForm)
  {
    Note::carrierAWaveform = waveForm;

    for(Note &note : notes)
    {
      note.ResetCarrierA();
    }
  }

  void SetCarrierBWaveform(const int &waveForm)
  {
    Note::carrierBWaveform = waveForm;

    for(Note &note : notes)
    {
      note.ResetCarrierB();
    }
  }

  void SetModulatorWaveform(const int &waveForm)
  {
    Note::modulatorWaveform = waveForm;

    for(Note &note : notes)
    {
      note.ResetModulator();
    }
  }

  void SetModulatorKeyboardFollowing(const bool &follow)
  {
    Note::modulatorFollowsKeyboard = follow;

    for(Note &note: notes)
    {
      note.UpdateModulatorFrequnecy();
    }
  }

  void SetFilterFreq(const float &freq)
  {
    Note::lowpassFreq = freq;
    Note::ladder->frequency(Note::lowpassFreq);
  }

  void SetFilterResonance(const float &resonance)
  {
    Note::lowpassResonance = resonance;
    Note::ladder->resonance(Note::lowpassResonance);
  }

//=======================
//  MIDI NOTE HANDLING
//=======================

  // TODO: Add velocity into account of amplitude of notes
  void NoteOn(byte channel, byte note, byte velocity)
  { 
    // Start by pointing to the note at the front of the array
    Note *foundNote = &notes.front();

    // Attempts to find a note that isn't playing
    for(auto it = notes.begin(); it != notes.end(); ++it)
    {
      if(it->GetPlaying() == false)
      {
        foundNote = &(*it);
        break;
      }
      // If this note has been playing for longer than the currently playing
      // note lets take it in case all notes are playing then we will override
      // it
      else if(it->GetElapsedTime() > foundNote->GetElapsedTime())
      {
        foundNote = &(*it);
      }
    }

    // TODO: Figure out way to stop clipping of notes when moving too fast
  
    // Update all values and turn on the note
    foundNote->SetNote(note);
    foundNote->UpdateCarrierAFrequnecy();
    foundNote->UpdateCarrierBFrequnecy();
    foundNote->UpdateModulatorFrequnecy();

    foundNote->TurnOn();
  }

  void NoteOff(byte channel, byte note, byte velocity)
  {
    // Find the currently playing note and turn it off
    for(auto it = notes.begin(); it != notes.end(); ++it)
    {
      if((*it).GetNote() == note)
      {
        it->TurnOff();
        break;
      }
    }
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

//==========================
// Initalization Function
//==========================

  void InitalizeNotes(AudioFilterLadder *ladder)
  {
    TIMS::Note::ladder = ladder;
    TIMS::SetFilterFreq(TIMS::Note::lowpassFreq);
    TIMS::SetFilterResonance(TIMS::Note::lowpassResonance);

    // Add each note in the note array to the 
    for(Note& note : notes)
    {
      note.InitalizeNote();
    }
  }
}

