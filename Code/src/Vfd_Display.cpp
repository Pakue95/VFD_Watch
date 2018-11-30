#include "Vfd_Display.h"
#include <Arduino.h>
#include <Ticker.h>
#include <Wire.h>
#include "Adafruit_MCP23017.h"


vfdDisplay::vfdDisplay(){}

vfdDisplay::~vfdDisplay(){
  deactivate();
  //TODO disable PWM
}

Adafruit_MCP23017 mcp;

void vfdDisplay::begin(
  uint8_t dutyCycle,
  uint32_t freqMultiplex,
  uint32_t freqHeat){
  Serial.begin(115200);
  pinMode(EN_24V, OUTPUT);
  pinMode(HEAT_EN, OUTPUT);
  pinMode(HEAT_INT1, OUTPUT);
  pinMode(HEAT_INT2, OUTPUT);
  pinMode(IO_RST, OUTPUT);

  digitalWrite(EN_24V, HIGH);
  digitalWrite(HEAT_EN, HIGH);
  digitalWrite(HEAT_INT1, LOW);
  digitalWrite(HEAT_INT2, LOW);
  digitalWrite(IO_RST, HIGH);

  _freqHeat = freqHeat;
  _freqMultiplex = freqMultiplex;
  _dutyCycle = dutyCycle; //invert value

  mcp.begin();
  for(int i = 0; i < 16; i++){
    mcp.pinMode(i,OUTPUT);
  }
  mcp.writeGPIOAB(0x0000); // all pins low

  // PWM setup
  ledcSetup(0, _freqHeat, 8);
  ledcAttachPin(HEAT_INT1, 0);
  ledcWrite(0, _dutyCycle);


  // FUCK THIS IN PARTICULAR
  tickerMultiplex.attach_ms(1000.0/_freqMultiplex, _setFlagMultiplex);
}

void vfdDisplay::_nextMultiplex() {
  mcp.writeGPIOAB(_dataMultiplex[_posMultiplex]);
  _posMultiplex = (_posMultiplex + 1) % 5;
}

bool vfdDisplay::_flagMultiplex = 0;

void vfdDisplay::_setFlagMultiplex(){
  _flagMultiplex = 1;
}

void vfdDisplay::handler(){
    if(_flagMultiplex) {_nextMultiplex(); _flagMultiplex = 0;}
}

uint16_t vfdDisplay::_getMultiplex(uint8_t pos, bool *digit){
  uint16_t out = 0;

  switch (pos) {
    case 0: out |= (1 << CHAR_1); break;
    case 1: out |= (1 << CHAR_2); break;
    case 2: out |= (1 << CHAR_3); break;
    case 3: out |= (1 << CHAR_4); break;
    case 4: out |= (1 << CHAR_5); break;
  }

  if (digit[0]) {out |= (1 << SEG_A);}
  if (digit[1]) {out |= (1 << SEG_B);}
  if (digit[2]) {out |= (1 << SEG_C);}
  if (digit[3]) {out |= (1 << SEG_D);}
  if (digit[4]) {out |= (1 << SEG_E);}
  if (digit[5]) {out |= (1 << SEG_F);}
  if (digit[6]) {out |= (1 << SEG_G);}
  if (digit[7]) {out |= (1 << SEG_DP1);}
  if (digit[8]) {out |= (1 << SEG_DP2);}

  return out;
}

void vfdDisplay::deactivate(){
  digitalWrite(EN_24V, LOW);
  digitalWrite(HEAT_EN, LOW);
  tickerMultiplex.detach();
}

void vfdDisplay::activate(){
  digitalWrite(EN_24V, HIGH);
  digitalWrite(HEAT_EN, HIGH);
  tickerMultiplex.attach_ms(1000.0/_freqMultiplex, _setFlagMultiplex);
}

void vfdDisplay::setDutyCycle(uint8_t duty){
  _dutyCycle = duty;
  ledcWrite(0, _dutyCycle);
}

void vfdDisplay::setHours(uint8_t hours){
  switch ((hours % 100)/10) {
    //                        a  b  c  d  e  f  g dp1 dp2
    case 0: {bool digit[9] = {1, 1, 1, 1, 1, 1, 0, 0, 0}; memcpy(_digit1, digit, sizeof(digit[0])*9); break;}
    case 1: {bool digit[9] = {0, 1, 1, 0, 0, 0, 0, 0, 0}; memcpy(_digit1, digit, sizeof(digit[0])*9); break;}
    case 2: {bool digit[9] = {1, 1, 0, 1, 1, 0, 1, 0, 0}; memcpy(_digit1, digit, sizeof(digit[0])*9); break;}
    case 3: {bool digit[9] = {1, 1, 1, 1, 0, 0, 1, 0, 0}; memcpy(_digit1, digit, sizeof(digit[0])*9); break;}
    case 4: {bool digit[9] = {0, 1, 1, 0, 0, 1, 1, 0, 0}; memcpy(_digit1, digit, sizeof(digit[0])*9); break;}
    case 5: {bool digit[9] = {1, 0, 1, 1, 0, 1, 1, 0, 0}; memcpy(_digit1, digit, sizeof(digit[0])*9); break;}
    case 6: {bool digit[9] = {1, 0, 1, 1, 1, 1, 1, 0, 0}; memcpy(_digit1, digit, sizeof(digit[0])*9); break;}
    case 7: {bool digit[9] = {1, 1, 1, 0, 0, 0, 0, 0, 0}; memcpy(_digit1, digit, sizeof(digit[0])*9); break;}
    case 8: {bool digit[9] = {1, 1, 1, 1, 1, 1, 1, 0, 0}; memcpy(_digit1, digit, sizeof(digit[0])*9); break;}
    case 9: {bool digit[9] = {1, 1, 1, 1, 0, 1, 1, 0, 0}; memcpy(_digit1, digit, sizeof(digit[0])*9); break;}
  }
  switch (hours % 10) {
    //                        a  b  c  d  e  f  g dp1 dp2
    case 0: {bool digit[9] = {1, 1, 1, 1, 1, 1, 0, 0, 0}; memcpy(_digit2, digit, sizeof(digit[0])*9); break;}
    case 1: {bool digit[9] = {0, 1, 1, 0, 0, 0, 0, 0, 0}; memcpy(_digit2, digit, sizeof(digit[0])*9); break;}
    case 2: {bool digit[9] = {1, 1, 0, 1, 1, 0, 1, 0, 0}; memcpy(_digit2, digit, sizeof(digit[0])*9); break;}
    case 3: {bool digit[9] = {1, 1, 1, 1, 0, 0, 1, 0, 0}; memcpy(_digit2, digit, sizeof(digit[0])*9); break;}
    case 4: {bool digit[9] = {0, 1, 1, 0, 0, 1, 1, 0, 0}; memcpy(_digit2, digit, sizeof(digit[0])*9); break;}
    case 5: {bool digit[9] = {1, 0, 1, 1, 0, 1, 1, 0, 0}; memcpy(_digit2, digit, sizeof(digit[0])*9); break;}
    case 6: {bool digit[9] = {1, 0, 1, 1, 1, 1, 1, 0, 0}; memcpy(_digit2, digit, sizeof(digit[0])*9); break;}
    case 7: {bool digit[9] = {1, 1, 1, 0, 0, 0, 0, 0, 0}; memcpy(_digit2, digit, sizeof(digit[0])*9); break;}
    case 8: {bool digit[9] = {1, 1, 1, 1, 1, 1, 1, 0, 0}; memcpy(_digit2, digit, sizeof(digit[0])*9); break;}
    case 9: {bool digit[9] = {1, 1, 1, 1, 0, 1, 1, 0, 0}; memcpy(_digit2, digit, sizeof(digit[0])*9); break;}
  }


  _dataMultiplex[0] = _getMultiplex(0, _digit1);
  _dataMultiplex[1] = _getMultiplex(1, _digit2);
}

void vfdDisplay::setMinutes(uint8_t minutes){
  switch ((minutes % 100)/10) {
    //                        a  b  c  d  e  f  g dp1 dp2
    case 0: {bool digit[9] = {1, 1, 1, 1, 1, 1, 0, 0, 0}; memcpy(_digit4, digit, sizeof(digit[0])*9); break;}
    case 1: {bool digit[9] = {0, 1, 1, 0, 0, 0, 0, 0, 0}; memcpy(_digit4, digit, sizeof(digit[0])*9); break;}
    case 2: {bool digit[9] = {1, 1, 0, 1, 1, 0, 1, 0, 0}; memcpy(_digit4, digit, sizeof(digit[0])*9); break;}
    case 3: {bool digit[9] = {1, 1, 1, 1, 0, 0, 1, 0, 0}; memcpy(_digit4, digit, sizeof(digit[0])*9); break;}
    case 4: {bool digit[9] = {0, 1, 1, 0, 0, 1, 1, 0, 0}; memcpy(_digit4, digit, sizeof(digit[0])*9); break;}
    case 5: {bool digit[9] = {1, 0, 1, 1, 0, 1, 1, 0, 0}; memcpy(_digit4, digit, sizeof(digit[0])*9); break;}
    case 6: {bool digit[9] = {1, 0, 1, 1, 1, 1, 1, 0, 0}; memcpy(_digit4, digit, sizeof(digit[0])*9); break;}
    case 7: {bool digit[9] = {1, 1, 1, 0, 0, 0, 0, 0, 0}; memcpy(_digit4, digit, sizeof(digit[0])*9); break;}
    case 8: {bool digit[9] = {1, 1, 1, 1, 1, 1, 1, 0, 0}; memcpy(_digit4, digit, sizeof(digit[0])*9); break;}
    case 9: {bool digit[9] = {1, 1, 1, 1, 0, 1, 1, 0, 0}; memcpy(_digit4, digit, sizeof(digit[0])*9); break;}
  }
  switch (minutes % 10) {
    //                        a  b  c  d  e  f  g dp1 dp2
    case 0: {bool digit[9] = {1, 1, 1, 1, 1, 1, 0, 0, 0}; memcpy(_digit5, digit, sizeof(digit[0])*9); break;}
    case 1: {bool digit[9] = {0, 1, 1, 0, 0, 0, 0, 0, 0}; memcpy(_digit5, digit, sizeof(digit[0])*9); break;}
    case 2: {bool digit[9] = {1, 1, 0, 1, 1, 0, 1, 0, 0}; memcpy(_digit5, digit, sizeof(digit[0])*9); break;}
    case 3: {bool digit[9] = {1, 1, 1, 1, 0, 0, 1, 0, 0}; memcpy(_digit5, digit, sizeof(digit[0])*9); break;}
    case 4: {bool digit[9] = {0, 1, 1, 0, 0, 1, 1, 0, 0}; memcpy(_digit5, digit, sizeof(digit[0])*9); break;}
    case 5: {bool digit[9] = {1, 0, 1, 1, 0, 1, 1, 0, 0}; memcpy(_digit5, digit, sizeof(digit[0])*9); break;}
    case 6: {bool digit[9] = {1, 0, 1, 1, 1, 1, 1, 0, 0}; memcpy(_digit5, digit, sizeof(digit[0])*9); break;}
    case 7: {bool digit[9] = {1, 1, 1, 0, 0, 0, 0, 0, 0}; memcpy(_digit5, digit, sizeof(digit[0])*9); break;}
    case 8: {bool digit[9] = {1, 1, 1, 1, 1, 1, 1, 0, 0}; memcpy(_digit5, digit, sizeof(digit[0])*9); break;}
    case 9: {bool digit[9] = {1, 1, 1, 1, 0, 1, 1, 0, 0}; memcpy(_digit5, digit, sizeof(digit[0])*9); break;}
  }

  _dataMultiplex[3] = _getMultiplex(3, _digit4);
  _dataMultiplex[4] = _getMultiplex(4, _digit5);
}

void vfdDisplay::setDP(bool dp1, bool dp2){
  //                            a  b  c  d  e  f  g dp1 dp2
  if (dp2 && dp1)   {bool digit[9] = {0, 0, 0, 0, 0, 0, 0, 1, 1};memcpy(_digit3, digit, sizeof(digit[0])*9);}
  if (dp2 && !dp1)  {bool digit[9] = {0, 0, 0, 0, 0, 0, 0, 1, 0};memcpy(_digit3, digit, sizeof(digit[0])*9);}
  if (!dp2 && dp1)  {bool digit[9] = {0, 0, 0, 0, 0, 0, 0, 0, 1};memcpy(_digit3, digit, sizeof(digit[0])*9);}
  if (!dp2 && !dp1) {bool digit[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};memcpy(_digit3, digit, sizeof(digit[0])*9);}

  _dataMultiplex[2] = _getMultiplex(2, _digit3);
}

void vfdDisplay::setCharacter(uint8_t pos, bool *digit){
  for(int i = 0; i < 5; i++){
    _dataMultiplex[i]=0x0000;
  }
  _dataMultiplex[pos] = _getMultiplex(pos, digit);
}
