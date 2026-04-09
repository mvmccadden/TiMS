/*!
 *  \author Manoel McCadden
 *  \date   26-04-03
 *  \file   menu.h
 *  \brief
 *    Handles menu functionality
 */

#include <stdint.h>
#include <Encoder.h>
#include "Print.h"
#include "midi.h"

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

float sineFrequency= 148;
float fmFrequency = 148;
float sineAmplitude = 0;
float fmAmplitude = 255;

float attack = (100.f / 1000.f) * 255;
float decay = (200.f / 1000.f) * 255;
float sustain = (0.7f / 1.f) * 255;
float release = (200.f / 1000.f) * 255;

constexpr uint8_t MAXMENUS = 2;
constexpr uint8_t MAXSUBMENUS = 1;
uint8_t menuID = 0;
uint8_t submenuID = 0;

bool UpdateEncoderValue(Encoder &encoder, float &encoderValue
    , const float &subDividor = 12)
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

  if(encoderValue > 255) 
  {
    encoderValue = 255;
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

void IncrementSubMenu()
{
  menuID = (menuID + 1) % MAXSUBMENUS;
}

void DecrementSubMenu()
{
  menuID == 0 ? menuID = MAXSUBMENUS - 1 : --menuID;
}

void PrintMenu()
{
  switch(menuID)
  {
    case 0:
      Serial.println("=======================================================");
      Serial.println("FM Carrier/Modulator");
      Serial.println("=======================================================");
      Serial.print("FM Frequency Encoder Value: ");
      Serial.println(fmFrequency);
      Serial.print("FM Amplitude Encoder Value: ");
      Serial.println(fmAmplitude);
      Serial.print("Sine Frequency Encoder Value: ");
      Serial.println(sineFrequency);
      Serial.print("Sine Amplitude Encoder Value: ");
      Serial.println(sineAmplitude);
      break;
    case 1:
      Serial.println("=======================================================");
      Serial.println("ADSR");
      Serial.println("=======================================================");
      Serial.print("Attack");
      Serial.println(attack);
      Serial.print("Decay");
      Serial.println(decay);
      Serial.print("Sustain");
      Serial.println(sustain);
      Serial.print("Release");
      Serial.println(release);
      break;
  }
}

void UpdateEncoderButtons()
{
  if(IsEncoderButtonDown(encoder1Button))
  {
    IncrementMenu();
  }
  if(IsEncoderButtonDown(encoder2Button))
  {

  }
  if(IsEncoderButtonDown(encoder3Button))
  {
    PrintMenu();
  }
  if(IsEncoderButtonDown(encoder4Button))
  {
    DecrementMenu();
  }
}

void UpdateSineValues()
{
  if(UpdateEncoderValue(encoder1, fmFrequency, 4.f))
  {
    float value = 440.f * pow(2.f, static_cast<float>(fmFrequency - 148) / 12.f);
    TIMS::SetFMSineFrequency(value);
  }
  if(UpdateEncoderValue(encoder2, fmAmplitude))
  {
    float value = static_cast<float>(fmAmplitude) / 255.f;
    TIMS::SetFMSineAmplitude(value);
  }
  if(UpdateEncoderValue(encoder3, sineFrequency, 4.f))
  {
    float value = 440.f * pow(2.f, static_cast<float>(sineFrequency - 148) / 12.f);
    TIMS::SetSineFrequency(value);
  }
  if(UpdateEncoderValue(encoder4, sineAmplitude))
  {
    float value = static_cast<float>(sineAmplitude) / 255.f;
    TIMS::SetSineAmplitude(value);
  }
}

void UpdateADSRValues()
{
  if(UpdateEncoderValue(encoder1, attack))
  {
    float value = static_cast<float>(attack) / 255.f * 1000.f;
    TIMS::SetAttack(value);
  }
  if(UpdateEncoderValue(encoder2, decay))
  {
    float value = static_cast<float>(decay) / 255.f * 1000.f;
    TIMS::SetDecay(value);
  }
  if(UpdateEncoderValue(encoder3, sustain))
  {
    float value = static_cast<float>(sustain) / 255.f;
    TIMS::SetDecay(value);
  }
  if(UpdateEncoderValue(encoder4, release))
  {
    float value = static_cast<float>(release) / 255.f * 1000.f;
    TIMS::SetDecay(value);
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
      UpdateSineValues();
      break;
    case 1:
      UpdateADSRValues();
      break;
  }
}
