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

// declare static member variables
// as they can't be declared in the
// header file
int vfdDisplay::_posMultiplex = 0;
uint16_t vfdDisplay::_dataMultiplex[5] = {
  0x0000,
  0x0000,
  0x0000,
  0x0000,
  0x0000
};

Adafruit_MCP23017 vfdDisplay::mcp;

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

  Wire.setClock(400000L);
  mcp.begin();
  for(int i = 0; i < 16; i++){
    mcp.pinMode(i,OUTPUT);
  }
  mcp.writeGPIOAB(0x0000); // all pins low

  // PWM setup
  ledcSetup(0, _freqHeat, 8);
  ledcAttachPin(HEAT_INT1, 0);
  ledcWrite(0, _dutyCycle);

  tickerMultiplex.attach_ms(
    1000.0/_freqMultiplex,
    _nextMultiplex);
}

void vfdDisplay::_nextMultiplex() {
  mcp.writeGPIOAB(0x0000); // turn off to reduce crosstalk
  _posMultiplex = (_posMultiplex + 1) % 5;
  mcp.writeGPIOAB(_dataMultiplex[_posMultiplex]);
}


uint16_t vfdDisplay::_setMultiplex(uint8_t pos, bool *digit){
  uint16_t out = 0;

  switch (pos) {
    case 0: out |= (1 << CHAR_1); break;
    case 1: out |= (1 << CHAR_2); break;
    case 2: out |= (1 << CHAR_3); break;
    case 3: out |= (1 << CHAR_4); break;
    case 4: out |= (1 << CHAR_5); break;
    default: return 0x0000;
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

  // set multiplex character in memory
  _dataMultiplex[pos] = out;
  return out;
}

uint16_t vfdDisplay::_updateMultiplex(){

  uint16_t tempStore = 0;
  for (int i = 0; i < 7; i++){
    if (_screen.digit[0].seg[i]) {tempStore |= (1 << _SEG_ARRAY[i]);}
  }
  tempStore |= (1 << CHAR_1);
  _dataMultiplex[0] = tempStore;

  tempStore = 0;
  for (int i = 0; i < 7; i++){
    if (_screen.digit[1].seg[i]) {tempStore |= (1 << _SEG_ARRAY[i]);}
  }
  tempStore |= (1 << CHAR_2);
  _dataMultiplex[1] = tempStore;

  tempStore = 0;
  for (int i = 0; i < 7; i++){
    if (_screen.digit[2].seg[i]) {tempStore |= (1 << _SEG_ARRAY[i]);}
  }
  tempStore |= (1 << CHAR_4);
  _dataMultiplex[3] = tempStore;

  tempStore = 0;
  for (int i = 0; i < 7; i++){
    if (_screen.digit[3].seg[i]) {tempStore |= (1 << _SEG_ARRAY[i]);}
  }
  tempStore |= (1 << CHAR_5);
  _dataMultiplex[4] = tempStore;
}

void vfdDisplay::deactivate(){
  digitalWrite(EN_24V, LOW);
  digitalWrite(HEAT_EN, LOW);
  tickerMultiplex.detach();
}

void vfdDisplay::activate(){
  digitalWrite(EN_24V, HIGH);
  digitalWrite(HEAT_EN, HIGH);
  // PWM setup
  ledcSetup(0, _freqHeat, 8);
  ledcAttachPin(HEAT_INT1, 0);
  ledcWrite(0, _dutyCycle);
  tickerMultiplex.attach_ms(1000.0/_freqMultiplex, _nextMultiplex);
}

void vfdDisplay::setDutyCycle(uint8_t duty){
  _dutyCycle = duty;
  ledcWrite(0, _dutyCycle);
}

void vfdDisplay::setHours(uint8_t hours){
  struct vfdDisplay::character c1;
  struct vfdDisplay::character c2;

  switch ((hours % 100)/10) {
    case 0: {c1.seg[0]=1; // a
             c1.seg[1]=1; // b
             c1.seg[2]=1; // c
             c1.seg[3]=1; // d
             c1.seg[4]=1; // e
             c1.seg[5]=1; // f
             c1.seg[6]=0; // g
             break;}
    case 1: {c1.seg[0]=0; // a
             c1.seg[1]=1; // b
             c1.seg[2]=1; // c
             c1.seg[3]=0; // d
             c1.seg[4]=0; // e
             c1.seg[5]=0; // f
             c1.seg[6]=0; // g
             break;}
    case 2: {c1.seg[0]=1; // a
             c1.seg[1]=1; // b
             c1.seg[2]=0; // c
             c1.seg[3]=1; // d
             c1.seg[4]=1; // e
             c1.seg[5]=0; // f
             c1.seg[6]=1; // g
             break;}
    case 3: {c1.seg[0]=1; // a
             c1.seg[1]=1; // b
             c1.seg[2]=1; // c
             c1.seg[3]=1; // d
             c1.seg[4]=0; // e
             c1.seg[5]=0; // f
             c1.seg[6]=1; // g
             break;}
    case 4: {c1.seg[0]=0; // a
             c1.seg[1]=1; // b
             c1.seg[2]=1; // c
             c1.seg[3]=0; // d
             c1.seg[4]=0; // e
             c1.seg[5]=1; // f
             c1.seg[6]=1; // g
             break;}
    case 5: {c1.seg[0]=1; // a
             c1.seg[1]=0; // b
             c1.seg[2]=1; // c
             c1.seg[3]=1; // d
             c1.seg[4]=0; // e
             c1.seg[5]=1; // f
             c1.seg[6]=1; // g
             break;}
    case 6: {c1.seg[0]=1; // a
             c1.seg[1]=0; // b
             c1.seg[2]=1; // c
             c1.seg[3]=1; // d
             c1.seg[4]=1; // e
             c1.seg[5]=1; // f
             c1.seg[6]=1; // g
             break;}
    case 7: {c1.seg[0]=1; // a
             c1.seg[1]=1; // b
             c1.seg[2]=1; // c
             c1.seg[3]=0; // d
             c1.seg[4]=0; // e
             c1.seg[5]=0; // f
             c1.seg[6]=0; // g
             break;}
    case 8: {c1.seg[0]=1; // a
             c1.seg[1]=1; // b
             c1.seg[2]=1; // c
             c1.seg[3]=1; // d
             c1.seg[4]=1; // e
             c1.seg[5]=1; // f
             c1.seg[6]=1; // g
             break;}
    case 9: {c1.seg[0]=1; // a
             c1.seg[1]=1; // b
             c1.seg[2]=1; // c
             c1.seg[3]=1; // d
             c1.seg[4]=0; // e
             c1.seg[5]=1; // f
             c1.seg[6]=1; // g
             break;}

  }
  switch (hours % 10) {
    case 0: {c2.seg[0]=1; // a
             c2.seg[1]=1; // b
             c2.seg[2]=1; // c
             c2.seg[3]=1; // d
             c2.seg[4]=1; // e
             c2.seg[5]=1; // f
             c2.seg[6]=0; // g
             break;}
    case 1: {c2.seg[0]=0; // a
             c2.seg[1]=1; // b
             c2.seg[2]=1; // c
             c2.seg[3]=0; // d
             c2.seg[4]=0; // e
             c2.seg[5]=0; // f
             c2.seg[6]=0; // g
             break;}
    case 2: {c2.seg[0]=1; // a
             c2.seg[1]=1; // b
             c2.seg[2]=0; // c
             c2.seg[3]=1; // d
             c2.seg[4]=1; // e
             c2.seg[5]=0; // f
             c2.seg[6]=1; // g
             break;}
    case 3: {c2.seg[0]=1; // a
             c2.seg[1]=1; // b
             c2.seg[2]=1; // c
             c2.seg[3]=1; // d
             c2.seg[4]=0; // e
             c2.seg[5]=0; // f
             c2.seg[6]=1; // g
             break;}
    case 4: {c2.seg[0]=0; // a
             c2.seg[1]=1; // b
             c2.seg[2]=1; // c
             c2.seg[3]=0; // d
             c2.seg[4]=0; // e
             c2.seg[5]=1; // f
             c2.seg[6]=1; // g
             break;}
    case 5: {c2.seg[0]=1; // a
             c2.seg[1]=0; // b
             c2.seg[2]=1; // c
             c2.seg[3]=1; // d
             c2.seg[4]=0; // e
             c2.seg[5]=1; // f
             c2.seg[6]=1; // g
             break;}
    case 6: {c2.seg[0]=1; // a
             c2.seg[1]=0; // b
             c2.seg[2]=1; // c
             c2.seg[3]=1; // d
             c2.seg[4]=1; // e
             c2.seg[5]=1; // f
             c2.seg[6]=1; // g
             break;}
    case 7: {c2.seg[0]=1; // a
             c2.seg[1]=1; // b
             c2.seg[2]=1; // c
             c2.seg[3]=0; // d
             c2.seg[4]=0; // e
             c2.seg[5]=0; // f
             c2.seg[6]=0; // g
             break;}
    case 8: {c2.seg[0]=1; // a
             c2.seg[1]=1; // b
             c2.seg[2]=1; // c
             c2.seg[3]=1; // d
             c2.seg[4]=1; // e
             c2.seg[5]=1; // f
             c2.seg[6]=1; // g
             break;}
    case 9: {c2.seg[0]=1; // a
             c2.seg[1]=1; // b
             c2.seg[2]=1; // c
             c2.seg[3]=1; // d
             c2.seg[4]=0; // e
             c2.seg[5]=1; // f
             c2.seg[6]=1; // g
             break;}
  }

  _screen.digit[0] = c1;
  _screen.digit[1] = c2;
  _updateMultiplex();
}

void vfdDisplay::setMinutes(uint8_t minutes){
  struct vfdDisplay::character c1;
  struct vfdDisplay::character c2;

  switch ((minutes % 100)/10) {
    case 0: {c1.seg[0]=1; // a
             c1.seg[1]=1; // b
             c1.seg[2]=1; // c
             c1.seg[3]=1; // d
             c1.seg[4]=1; // e
             c1.seg[5]=1; // f
             c1.seg[6]=0; // g
             break;}
    case 1: {c1.seg[0]=0; // a
             c1.seg[1]=1; // b
             c1.seg[2]=1; // c
             c1.seg[3]=0; // d
             c1.seg[4]=0; // e
             c1.seg[5]=0; // f
             c1.seg[6]=0; // g
             break;}
    case 2: {c1.seg[0]=1; // a
             c1.seg[1]=1; // b
             c1.seg[2]=0; // c
             c1.seg[3]=1; // d
             c1.seg[4]=1; // e
             c1.seg[5]=0; // f
             c1.seg[6]=1; // g

             break;}
    case 3: {c1.seg[0]=1; // a
             c1.seg[1]=1; // b
             c1.seg[2]=1; // c
             c1.seg[3]=1; // d
             c1.seg[4]=0; // e
             c1.seg[5]=0; // f
             c1.seg[6]=1; // g
             break;}
    case 4: {c1.seg[0]=0; // a
             c1.seg[1]=1; // b
             c1.seg[2]=1; // c
             c1.seg[3]=0; // d
             c1.seg[4]=0; // e
             c1.seg[5]=1; // f
             c1.seg[6]=1; // g
             break;}
    case 5: {c1.seg[0]=1; // a
             c1.seg[1]=0; // b
             c1.seg[2]=1; // c
             c1.seg[3]=1; // d
             c1.seg[4]=0; // e
             c1.seg[5]=1; // f
             c1.seg[6]=1; // g
             break;}
    case 6: {c1.seg[0]=1; // a
             c1.seg[1]=0; // b
             c1.seg[2]=1; // c
             c1.seg[3]=1; // d
             c1.seg[4]=1; // e
             c1.seg[5]=1; // f
             c1.seg[6]=1; // g
             break;}
    case 7: {c1.seg[0]=1; // a
             c1.seg[1]=1; // b
             c1.seg[2]=1; // c
             c1.seg[3]=0; // d
             c1.seg[4]=0; // e
             c1.seg[5]=0; // f
             c1.seg[6]=0; // g
             break;}
    case 8: {c1.seg[0]=1; // a
             c1.seg[1]=1; // b
             c1.seg[2]=1; // c
             c1.seg[3]=1; // d
             c1.seg[4]=1; // e
             c1.seg[5]=1; // f
             c1.seg[6]=1; // g
             break;}
    case 9: {c1.seg[0]=1; // a
             c1.seg[1]=1; // b
             c1.seg[2]=1; // c
             c1.seg[3]=1; // d
             c1.seg[4]=0; // e
             c1.seg[5]=1; // f
             c1.seg[6]=1; // g
             break;}
    /*
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
    */

  }
  switch (minutes % 10) {
    case 0: {c2.seg[0]=1; // a
             c2.seg[1]=1; // b
             c2.seg[2]=1; // c
             c2.seg[3]=1; // d
             c2.seg[4]=1; // e
             c2.seg[5]=1; // f
             c2.seg[6]=0; // g
             break;}
    case 1: {c2.seg[0]=0; // a
             c2.seg[1]=1; // b
             c2.seg[2]=1; // c
             c2.seg[3]=0; // d
             c2.seg[4]=0; // e
             c2.seg[5]=0; // f
             c2.seg[6]=0; // g
             break;}
    case 2: {c2.seg[0]=1; // a
             c2.seg[1]=1; // b
             c2.seg[2]=0; // c
             c2.seg[3]=1; // d
             c2.seg[4]=1; // e
             c2.seg[5]=0; // f
             c2.seg[6]=1; // g
             break;}
    case 3: {c2.seg[0]=1; // a
             c2.seg[1]=1; // b
             c2.seg[2]=1; // c
             c2.seg[3]=1; // d
             c2.seg[4]=0; // e
             c2.seg[5]=0; // f
             c2.seg[6]=1; // g
             break;}
    case 4: {c2.seg[0]=0; // a
             c2.seg[1]=1; // b
             c2.seg[2]=1; // c
             c2.seg[3]=0; // d
             c2.seg[4]=0; // e
             c2.seg[5]=1; // f
             c2.seg[6]=1; // g
             break;}
    case 5: {c2.seg[0]=1; // a
             c2.seg[1]=0; // b
             c2.seg[2]=1; // c
             c2.seg[3]=1; // d
             c2.seg[4]=0; // e
             c2.seg[5]=1; // f
             c2.seg[6]=1; // g
             break;}
    case 6: {c2.seg[0]=1; // a
             c2.seg[1]=0; // b
             c2.seg[2]=1; // c
             c2.seg[3]=1; // d
             c2.seg[4]=1; // e
             c2.seg[5]=1; // f
             c2.seg[6]=1; // g
             break;}
    case 7: {c2.seg[0]=1; // a
             c2.seg[1]=1; // b
             c2.seg[2]=1; // c
             c2.seg[3]=0; // d
             c2.seg[4]=0; // e
             c2.seg[5]=0; // f
             c2.seg[6]=0; // g
             break;}
    case 8: {c2.seg[0]=1; // a
             c2.seg[1]=1; // b
             c2.seg[2]=1; // c
             c2.seg[3]=1; // d
             c2.seg[4]=1; // e
             c2.seg[5]=1; // f
             c2.seg[6]=1; // g
             break;}
    case 9: {c2.seg[0]=1; // a
             c2.seg[1]=1; // b
             c2.seg[2]=1; // c
             c2.seg[3]=1; // d
             c2.seg[4]=0; // e
             c2.seg[5]=1; // f
             c2.seg[6]=1; // g
             break;}
  }

  _screen.digit[2] = c1;
  _screen.digit[3] = c2;
  _updateMultiplex();
}

void vfdDisplay::setDP(bool dp1, bool dp2){
  //                            a  b  c  d  e  f  g dp1 dp2
  if (dp2 && dp1)   {bool digit[9] = {0, 0, 0, 0, 0, 0, 0, 1, 1};memcpy(_digit3, digit, sizeof(digit[0])*9);}
  if (dp2 && !dp1)  {bool digit[9] = {0, 0, 0, 0, 0, 0, 0, 1, 0};memcpy(_digit3, digit, sizeof(digit[0])*9);}
  if (!dp2 && dp1)  {bool digit[9] = {0, 0, 0, 0, 0, 0, 0, 0, 1};memcpy(_digit3, digit, sizeof(digit[0])*9);}
  if (!dp2 && !dp1) {bool digit[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};memcpy(_digit3, digit, sizeof(digit[0])*9);}

  _setMultiplex(2, _digit3);
}

void vfdDisplay::setCharacter(char character, int pos){
  bool onSegments[9];

  switch (character) {
    //                       a  b  c  d  e  f  g dp1 dp2
    case '0': {bool temp[9] = {1, 1, 1, 1, 1, 1, 0, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case '1': {bool temp[9] = {0, 1, 1, 0, 0, 0, 0, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case '2': {bool temp[9] = {1, 1, 0, 1, 1, 0, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case '3': {bool temp[9] = {1, 1, 1, 1, 0, 0, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case '4': {bool temp[9] = {0, 1, 1, 0, 0, 1, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case '5': {bool temp[9] = {1, 0, 1, 1, 0, 1, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case '6': {bool temp[9] = {1, 0, 1, 1, 1, 1, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case '7': {bool temp[9] = {1, 1, 1, 0, 0, 0, 0, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case '8': {bool temp[9] = {1, 1, 1, 1, 1, 1, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case '9': {bool temp[9] = {1, 1, 1, 1, 0, 1, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}

    case 'a': {bool temp[9] = {1, 1, 1, 0, 1, 1, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 'b': {bool temp[9] = {0, 0, 1, 1, 1, 1, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 'c': {bool temp[9] = {1, 0, 0, 1, 1, 1, 0, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 'd': {bool temp[9] = {0, 1, 1, 1, 1, 0, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 'e': {bool temp[9] = {1, 0, 0, 1, 1, 1, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 'f': {bool temp[9] = {1, 0, 0, 0, 1, 1, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 'g': {bool temp[9] = {1, 1, 1, 1, 0, 1, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 'h': {bool temp[9] = {0, 1, 1, 0, 1, 1, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 'i': {bool temp[9] = {0, 0, 0, 0, 1, 1, 0, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 'j': {bool temp[9] = {0, 1, 1, 1, 1, 0, 0, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 'k': {bool temp[9] = {0, 0, 0, 0, 1, 1, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 'l': {bool temp[9] = {0, 0, 0, 1, 1, 1, 0, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 'm': {bool temp[9] = {1, 0, 1, 0, 1, 0, 0, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 'n': {bool temp[9] = {0, 0, 1, 0, 1, 0, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 'o': {bool temp[9] = {1, 1, 1, 1, 1, 1, 0, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 'p': {bool temp[9] = {1, 1, 0, 0, 1, 1, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 'q': {bool temp[9] = {1, 1, 1, 0, 0, 1, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 'r': {bool temp[9] = {0, 0, 0, 0, 1, 0, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 's': {bool temp[9] = {1, 0, 1, 1, 0, 1, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 't': {bool temp[9] = {0, 0, 0, 1, 1, 1, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 'u': {bool temp[9] = {0, 1, 1, 1, 1, 1, 0, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 'v': {bool temp[9] = {0, 0, 1, 1, 1, 0, 0, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 'w': {bool temp[9] = {0, 1, 0, 1, 0, 1, 0, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 'x': {bool temp[9] = {0, 1, 1, 0, 0, 0, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 'y': {bool temp[9] = {0, 1, 1, 1, 0, 1, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case 'z': {bool temp[9] = {0, 1, 0, 0, 1, 0, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    case '-': {bool temp[9] = {0, 0, 0, 0, 0, 0, 1, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
    default:  {bool temp[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; memcpy(onSegments, temp, sizeof(temp[0])*9); break;}
  }

  _setMultiplex(pos, onSegments);
}

void vfdDisplay::print(char* text){
  setDP(0,0);
  setCharacter(text[0],0);
  setCharacter(text[1],1);
  setCharacter(text[2],3);
  setCharacter(text[3],4);
}

void vfdDisplay::setSegment(uint8_t segments, uint8_t pos){
  bool boolSegments[9] = {0};

  for (int i = 0; i < 8; ++i){
    boolSegments[i] = ((segments) & (1<<(i)));
  }

  // the 4 7-segments are on positions
  // 0, 1, 3, 4
  // the third 7-segment (2) should therefore be
  // at multiplex position 3
  if(pos >= 2) ++pos;

  _setMultiplex(pos, boolSegments);
}

void vfdDisplay::testNewStruct(vfdDisplay::screen dodo){
  _screen = dodo;
  _updateMultiplex();
}
