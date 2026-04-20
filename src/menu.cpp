/*!
 *  \author Manoel McCadden
 *  \date   26-04-03
 *  \file   menu.h
 *  \brief
 *    Handles menu functionality
 */

#include <stdint.h>
#include <Encoder.h>

#include "midi.h"

elapsedMillis screenTimer;

struct EncoderButton
{
  EncoderButton(const int &buttonPin)
    : buttonID(buttonPin) 
  {
    pinMode(buttonID, INPUT_PULLUP);
  }

  const int buttonID;
  bool prevState = false;
  bool state = false;

  void UpdateState(const bool &newState)
  {
    prevState = state;
    state = newState;
  }

  bool Pressed()
  {
    return prevState != state && state == true;
  }
};

// Encoder 
Encoder encoder1(28,27);
EncoderButton encoder1Button(29);

Encoder encoder2(37,38);
EncoderButton encoder2Button(36);

Encoder encoder3(31,30);
EncoderButton encoder3Button(32);

Encoder encoder4(34,35);
EncoderButton encoder4Button(33);

// NOTE: How values are mapped here
// - Freq -> 0-35
// - Octave -> 0-11 
// - Amp  -> 0-63

// NOTE: Order of mappings for oscilators
// 1 -> CarrierA
// 2 -> CarrierB
// 3 -> Modulator

// Waveforms
enum WAVEFORMS
{
  TRIANGLE = 0 
  , SAWTOOTH
  , REVERSESAWTOOTH 
  , SQUARE 
  , PULSE 
  , SAMPLEHOLD 
  , SINE 
  , VARIABLETRIANGLE 
};

// Oscilators Encoder Values
float frequencies[3] = {18, 18, 18};
float octaves[3] = {6, 6, 0};
float amplitudes[3] = {63, 63, 0};
float waveforms[3] = {WAVEFORMS::TRIANGLE, WAVEFORMS::SAWTOOTH
  , WAVEFORMS::TRIANGLE};

// NOTE:
//  Modulator low frequency -> 0-35 (0Hz to ~6Hz)
float modulatorLowFrequency = 0;

// Modulator Encoder Buttons
bool modulatorFollowsKeyboard = false;
bool modualtorInLowMode = false;

void(*p_freqFuncs[3])(const float&) = {TIMS::SetCarrierAFrequency
  , TIMS::SetCarrierBFrequency, TIMS::SetModulatorFrequency};
void(*p_ampFuncs[3])(const float&) = {TIMS::SetCarrierAAmplitude
  , TIMS::SetCarrierBAmplitude, TIMS::SetModulatorAmplitude};
void(*p_waveFuncs[3])(const int&) = {TIMS::SetCarrierAWaveform
  , TIMS::SetCarrierBWaveform, TIMS::SetModulatorWaveform};

// ADSR Encoder Values
float attack = (75.f / 1000.f) * 127;
float decay = (75.f / 1000.f) * 127;
float sustain = (0.7f / 1.f) * 127;
float release = (75.f / 1000.f) * 127;

// Lowpass Encoder Values
// NOTE:
// Lowpass range
// freq -> 0-127
// fine -> 0-63
// octave -> 0-63
// resonane -> 0-31
float lowpassFreq = 127;
float lowpassFine = 31;
float lowpassOctave = 0;
float lowpassResonance = 0.8f / 1.8f * 31;

// Menu values
constexpr uint8_t MAXMENUS = 5;
uint8_t menuID = 0;

bool UpdateEncoderValue(Encoder &encoder, float &encoderValue
    , const float &subDividor = 1, const int &maxValue = 127)
{
  float value = static_cast<float>(encoder.readAndReset()) / subDividor;

  if(value == 0) return false;

  if(abs(value) > encoderValue && value < 0) 
  {
    encoderValue = 0;
  }
  else 
  {
    encoderValue += value;
  }

  if(encoderValue > maxValue) 
  {
    encoderValue = maxValue;
  }

  return true;
}

bool IsEncoderButtonDown(EncoderButton &button)
{
  button.UpdateState(digitalRead(button.buttonID) == LOW);
  return button.Pressed();
}

void IncrementMenu()
{
  menuID = (menuID + 1) % MAXMENUS;
}

void DecrementMenu()
{
  menuID == 0 ? menuID = MAXMENUS - 1 : --menuID;
}

void PrintWaveform()
{
  Serial.print("Waveform: ");
  switch(static_cast<WAVEFORMS>(waveforms[menuID]))
  {
    case TRIANGLE:
      Serial.println("Triangle");
      break;
    case SAWTOOTH:
      Serial.println("Sawtooth");
      break;
    case REVERSESAWTOOTH:
      Serial.println("Reverse Sawtooth");
      break;
    case SAMPLEHOLD:
      Serial.println("Sample and Hold");
      break;
    case PULSE:
      Serial.println("Pulse");
      break;
    case SINE:
      Serial.println("Sine");
      break;
    case SQUARE:
      Serial.println("Square");
      break;
    case VARIABLETRIANGLE:
      Serial.println("Variable Triangle");
      break;
    default:
      Serial.println("DEFAULT: Triangle");
  }
}

void PrintMenu()
{
  // Move to top left of terminal
  Serial.print("\x1b[H");
  // Clear the screen
  Serial.print("\x1b[2J");

  Serial.println("=======================================================");

  switch(menuID)
  {
    case 0:
      Serial.println("Carrier A");
      Serial.println("=======================================================");
      Serial.printf("Frequency: %.2f Hz", TIMS::Note::baseCarrierAFreq);
      Serial.println("");
      Serial.printf("--> Frequency Offset Value: %.2f", (frequencies[menuID]) / 3.f);
      Serial.println("");
      Serial.printf("--> Octave Offset Value: %.2f", octaves[menuID]);
      Serial.println("");
      Serial.printf("Amplitude: %.2f", TIMS::Note::carrierAAmplitude);
      Serial.println("");
      PrintWaveform();
      break;
    case 1:
      Serial.println("Carrier B");
      Serial.println("=======================================================");
      Serial.printf("Frequency: %.2f Hz", TIMS::Note::baseCarrierBFreq);
      Serial.println("");
      Serial.printf("--> Frequency Offset Value: %.2f", (frequencies[menuID]) / 3.f);
      Serial.println("");
      Serial.printf("--> Octave Offset Value: %.2f", octaves[menuID]);
      Serial.println("");
      Serial.printf("Amplitude: %.2f", TIMS::Note::carrierBAmplitude);
      Serial.println("");
      PrintWaveform();
      break;
    case 2:
      Serial.println("Modualtor");
      Serial.println("=======================================================");
      if(modualtorInLowMode)
      {
        Serial.printf("Frequency: %.2f Hz", modulatorLowFrequency / 6.f);
        Serial.println("");
      }
      else
      {
        Serial.printf("Frequency: %.2f Hz", TIMS::Note::baseModulatorFreq);
        Serial.println("");
        Serial.printf("--> Frequency Offset Value: %.2f", (frequencies[menuID]) / 3.f);
        Serial.println("");
        Serial.printf("--> Octave Offset Value: %.2f", octaves[menuID]);
        Serial.println("");
      }
      Serial.printf("Amplitude: %.2f", TIMS::Note::modulatorAmplitude);
      Serial.println("");
      Serial.printf("Frequency Follows Keyboard: %s"
          , modulatorFollowsKeyboard ? "true" : "false");
      Serial.println("");
      Serial.printf("Low Frequency Mode: %s"
          , modualtorInLowMode? "true" : "false");
      Serial.println("");
      PrintWaveform();
      break;
    case 3:
      Serial.println("ADSR");
      Serial.println("=======================================================");
      Serial.printf("Attack: %.2f ms", TIMS::Note::attack);
      Serial.println("");
      Serial.printf("Decay: %.2f ms", TIMS::Note::decay);
      Serial.println("");
      Serial.printf("Sustain: %.2f", TIMS::Note::sustain);
      Serial.println("");
      Serial.printf("Release: %.2f ms", TIMS::Note::release);
      Serial.println("");
      break;
    case 4:
      Serial.println("Lowpass Ladder Filter");
      Serial.println("=======================================================");
      Serial.printf("Frequency: %.2f Hz", TIMS::Note::lowpassFreq);
      Serial.println("");
      Serial.printf("Resonance: %.2f", TIMS::Note::lowpassResonance);
      Serial.println("");
      break;
  }
}

void HandleEncoder2Press()
{
  switch(menuID)
  {
    case 2:
      modualtorInLowMode = !modualtorInLowMode;
  }
}

void HandleEncoder3Press()
{
  switch(menuID)
  {
    case 2:
      modulatorFollowsKeyboard = !modulatorFollowsKeyboard;
      TIMS::SetModulatorKeyboardFollowing(modulatorFollowsKeyboard);
      //Serial.print("Modulator Keyboard Follow Status Update: ");
      //Serial.println(modulatorFollowsKeyboard);
      break;
  }
}

void UpdateEncoderButtons()
{
  if(IsEncoderButtonDown(encoder1Button))
  {
    DecrementMenu();
  }
  if(IsEncoderButtonDown(encoder2Button))
  {
    HandleEncoder2Press();
  }
  if(IsEncoderButtonDown(encoder3Button))
  {
    //PrintMenu();
    HandleEncoder3Press();
  }
  if(IsEncoderButtonDown(encoder4Button))
  {
    IncrementMenu();
  }
}

void UpdateWaveforms()
{
  int waveform;

  switch(static_cast<WAVEFORMS>(waveforms[menuID]))
  {
    case TRIANGLE:
      waveform = WAVEFORM_TRIANGLE;
      break;
    case SAWTOOTH:
      waveform = WAVEFORM_SAWTOOTH;
      break;
    case REVERSESAWTOOTH:
      waveform = WAVEFORM_SAWTOOTH_REVERSE;
      break;
    case SAMPLEHOLD:
      waveform = WAVEFORM_SAMPLE_HOLD;
      break;
    case PULSE:
      waveform = WAVEFORM_PULSE;
      break;
    case SINE:
      waveform = WAVEFORM_SINE;
      break;
    case SQUARE:
      waveform = WAVEFORM_SQUARE;
      break;
    case VARIABLETRIANGLE:
      waveform = WAVEFORM_TRIANGLE_VARIABLE;
      break;
    default:
      waveform = WAVEFORM_TRIANGLE;
  }

  p_waveFuncs[menuID](waveform);
}

/*!
 *  Calculates the oscilator frequency based on the frequency knob and octave
 *  knob
 *
 *  Default is A4 440Hz at 0,0 and the frequency and octave knobs adjust it 
 *  from there 
 */
void UpdateOscilatorFreq()
{
  float frequencyKnobValue = frequencies[menuID] / 3.f - 6.f;
  float octaveKnobValue = (octaves[menuID] - 6.f) * 12.f;

  float value = 440.f * pow(2.f
      , (frequencyKnobValue + octaveKnobValue) / 12.f);

  p_freqFuncs[menuID](value);
}

void UpdateModulatorLowFreq()
{
  float value = modulatorLowFrequency / 6.f;

  p_freqFuncs[menuID](value);
}

void UpdateOscilatorValues()
{
  if(menuID == 2 && modualtorInLowMode 
      && UpdateEncoderValue(encoder1, modulatorLowFrequency, 4.f, 35))
  {
    UpdateModulatorLowFreq(); 
  }
  else if(UpdateEncoderValue(encoder1, frequencies[menuID], 4.f, 35))
  {
    UpdateOscilatorFreq();
  }

  if(UpdateEncoderValue(encoder3, amplitudes[menuID], 4.f, 63))
  {
    float value = amplitudes[menuID] / 63.f;
    p_ampFuncs[menuID](value);
  }
  if(UpdateEncoderValue(encoder4, waveforms[menuID], 4.f
        , static_cast<int>(WAVEFORMS::VARIABLETRIANGLE)))
  {
    UpdateWaveforms();
  }

  if(menuID == 2 && modualtorInLowMode)
  {
    return;
  }

  if(UpdateEncoderValue(encoder2, octaves[menuID], 4.f, 11))
  {
    UpdateOscilatorFreq();
  }
}

void UpdateADSRValues()
{
  if(UpdateEncoderValue(encoder1, attack))
  {
    float value = static_cast<float>(attack) / 127.f * 1000.f;
    TIMS::SetAttack(value);
  }
  if(UpdateEncoderValue(encoder2, decay))
  {
    float value = static_cast<float>(decay) / 127.f * 1000.f;
    TIMS::SetDecay(value);
  }
  if(UpdateEncoderValue(encoder3, sustain))
  {
    float value = static_cast<float>(sustain) / 127.f;
    TIMS::SetSustain(value);
  }
  if(UpdateEncoderValue(encoder4, release))
  {
    float value = static_cast<float>(release) / 127.f * 1000.f;
    TIMS::SetRelease(value);
  }
}

void UpdateLowpassLaderFreq()
{
  float value = 440.f * std::pow(2.f
      , (((lowpassFreq - 69) + ((lowpassFine / 31.f) - 1)) / 12.f));
  TIMS::SetFilterFreq(value);
}

void UpdateLowpassLadderValues()
{
  if(UpdateEncoderValue(encoder1, lowpassFreq, 4.f))
  {
    UpdateLowpassLaderFreq();
  }
  if(UpdateEncoderValue(encoder2, lowpassFine, 4.f, 63))
  {
    UpdateLowpassLaderFreq();
  }
  if(UpdateEncoderValue(encoder3, lowpassOctave, 4.f, 63))
  {
  }
  if(UpdateEncoderValue(encoder4, lowpassResonance, 4., 31))
  {
    TIMS::SetFilterResonance(lowpassResonance / 31.f * 1.8f);
  }
}

// TODO: Make this increment and remove bounce of buttons by saving button state
void CheckButtons()
{

}

void LoopMenu()
{
  switch(menuID)
  {
    case 0:
    case 1:
    case 2:
      UpdateOscilatorValues();
      break;
    case 3:
      UpdateADSRValues();
      break;
    case 4:
      UpdateLowpassLadderValues();
  }

  if(screenTimer > 100)
  {
    screenTimer = 0;
    PrintMenu();
  }
}
