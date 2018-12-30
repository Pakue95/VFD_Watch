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

void vfdDisplay::_updateMultiplex(){
  for (int i = 0; i < 5; i++){
    uint16_t tempStore = 0;
    if (_screen.digit[i].a) {tempStore |= (1 << SEG_A);}
    if (_screen.digit[i].b) {tempStore |= (1 << SEG_B);}
    if (_screen.digit[i].c) {tempStore |= (1 << SEG_C);}
    if (_screen.digit[i].d) {tempStore |= (1 << SEG_D);}
    if (_screen.digit[i].e) {tempStore |= (1 << SEG_E);}
    if (_screen.digit[i].f) {tempStore |= (1 << SEG_F);}
    if (_screen.digit[i].g) {tempStore |= (1 << SEG_G);}
    if (_screen.dp[0])      {tempStore |= (1 << SEG_DP1);}
    if (_screen.dp[1])      {tempStore |= (1 << SEG_DP2);}

    switch (i) {
      case 0: {tempStore |= (1 << CHAR_1); break;}
      case 1: {tempStore |= (1 << CHAR_2); break;}
      case 2: {tempStore |= (1 << CHAR_3); break;}
      case 3: {tempStore |= (1 << CHAR_4); break;}
      case 4: {tempStore |= (1 << CHAR_5); break;}
    }
    _dataMultiplex[i] = tempStore;
  }

/*
  tempStore = 0;
  for (int i = 0; i < 7; i++){
    if (_screen.digit[1].a) {tempStore |= (1 << SEG_A);}
    if (_screen.digit[1].b) {tempStore |= (1 << SEG_B);}
    if (_screen.digit[1].c) {tempStore |= (1 << SEG_C);}
    if (_screen.digit[1].d) {tempStore |= (1 << SEG_D);}
    if (_screen.digit[1].e) {tempStore |= (1 << SEG_E);}
    if (_screen.digit[1].f) {tempStore |= (1 << SEG_F);}
    if (_screen.digit[1].g) {tempStore |= (1 << SEG_G);}
  }
  tempStore |= (1 << CHAR_2);
  _dataMultiplex[1] = tempStore;

  tempStore = 0;
  for (int i = 0; i < 7; i++){
    if (_screen.digit[2].a) {tempStore |= (1 << SEG_A);}
    if (_screen.digit[2].b) {tempStore |= (1 << SEG_B);}
    if (_screen.digit[2].c) {tempStore |= (1 << SEG_C);}
    if (_screen.digit[2].d) {tempStore |= (1 << SEG_D);}
    if (_screen.digit[2].e) {tempStore |= (1 << SEG_E);}
    if (_screen.digit[2].f) {tempStore |= (1 << SEG_F);}
    if (_screen.digit[2].g) {tempStore |= (1 << SEG_G);}
  }
  tempStore |= (1 << CHAR_3);
  _dataMultiplex[3] = tempStore;

  tempStore = 0;
  for (int i = 0; i < 7; i++){
    if (_screen.digit[3].a) {tempStore |= (1 << SEG_A);}
    if (_screen.digit[3].b) {tempStore |= (1 << SEG_B);}
    if (_screen.digit[3].c) {tempStore |= (1 << SEG_C);}
    if (_screen.digit[3].d) {tempStore |= (1 << SEG_D);}
    if (_screen.digit[3].e) {tempStore |= (1 << SEG_E);}
    if (_screen.digit[3].f) {tempStore |= (1 << SEG_F);}
    if (_screen.digit[3].g) {tempStore |= (1 << SEG_G);}
  }
  tempStore |= (1 << CHAR_4);
  _dataMultiplex[4] = tempStore;
  */
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
  switch ((hours % 100)/10) { //        a  b  c  d  e  f  g
    case 0: {vfdDisplay::character c = {1, 1, 1, 1, 1, 1, 0}; _screen.digit[0] = c; break;}
    case 1: {vfdDisplay::character c = {0, 1, 1, 0, 0, 0, 0}; _screen.digit[0] = c; break;}
    case 2: {vfdDisplay::character c = {1, 1, 0, 1, 1, 0, 1}; _screen.digit[0] = c; break;}
    case 3: {vfdDisplay::character c = {1, 1, 1, 1, 0, 0, 1}; _screen.digit[0] = c; break;}
    case 4: {vfdDisplay::character c = {0, 1, 1, 0, 0, 1, 1}; _screen.digit[0] = c; break;}
    case 5: {vfdDisplay::character c = {1, 0, 1, 1, 0, 1, 1}; _screen.digit[0] = c; break;}
    case 6: {vfdDisplay::character c = {1, 0, 1, 1, 1, 1, 1}; _screen.digit[0] = c; break;}
    case 7: {vfdDisplay::character c = {1, 1, 1, 0, 0, 0, 0}; _screen.digit[0] = c; break;}
    case 8: {vfdDisplay::character c = {1, 1, 1, 1, 1, 1, 1}; _screen.digit[0] = c; break;}
    case 9: {vfdDisplay::character c = {1, 1, 1, 1, 0, 1, 1}; _screen.digit[0] = c; break;}
  }
  switch (hours % 10) { //              a  b  c  d  e  f  g
    case 0: {vfdDisplay::character c = {1, 1, 1, 1, 1, 1, 0}; _screen.digit[1] = c; break;}
    case 1: {vfdDisplay::character c = {0, 1, 1, 0, 0, 0, 0}; _screen.digit[1] = c; break;}
    case 2: {vfdDisplay::character c = {1, 1, 0, 1, 1, 0, 1}; _screen.digit[1] = c; break;}
    case 3: {vfdDisplay::character c = {1, 1, 1, 1, 0, 0, 1}; _screen.digit[1] = c; break;}
    case 4: {vfdDisplay::character c = {0, 1, 1, 0, 0, 1, 1}; _screen.digit[1] = c; break;}
    case 5: {vfdDisplay::character c = {1, 0, 1, 1, 0, 1, 1}; _screen.digit[1] = c; break;}
    case 6: {vfdDisplay::character c = {1, 0, 1, 1, 1, 1, 1}; _screen.digit[1] = c; break;}
    case 7: {vfdDisplay::character c = {1, 1, 1, 0, 0, 0, 0}; _screen.digit[1] = c; break;}
    case 8: {vfdDisplay::character c = {1, 1, 1, 1, 1, 1, 1}; _screen.digit[1] = c; break;}
    case 9: {vfdDisplay::character c = {1, 1, 1, 1, 0, 1, 1}; _screen.digit[1] = c; break;}
  }
  _updateMultiplex();
}

void vfdDisplay::setMinutes(uint8_t minutes){
  switch ((minutes % 100)/10) { //      a  b  c  d  e  f  g
    case 0: {vfdDisplay::character c = {1, 1, 1, 1, 1, 1, 0}; _screen.digit[2] = c; break;}
    case 1: {vfdDisplay::character c = {0, 1, 1, 0, 0, 0, 0}; _screen.digit[2] = c; break;}
    case 2: {vfdDisplay::character c = {1, 1, 0, 1, 1, 0, 1}; _screen.digit[2] = c; break;}
    case 3: {vfdDisplay::character c = {1, 1, 1, 1, 0, 0, 1}; _screen.digit[2] = c; break;}
    case 4: {vfdDisplay::character c = {0, 1, 1, 0, 0, 1, 1}; _screen.digit[2] = c; break;}
    case 5: {vfdDisplay::character c = {1, 0, 1, 1, 0, 1, 1}; _screen.digit[2] = c; break;}
    case 6: {vfdDisplay::character c = {1, 0, 1, 1, 1, 1, 1}; _screen.digit[2] = c; break;}
    case 7: {vfdDisplay::character c = {1, 1, 1, 0, 0, 0, 0}; _screen.digit[2] = c; break;}
    case 8: {vfdDisplay::character c = {1, 1, 1, 1, 1, 1, 1}; _screen.digit[2] = c; break;}
    case 9: {vfdDisplay::character c = {1, 1, 1, 1, 0, 1, 1}; _screen.digit[2] = c; break;}
  }
  switch (minutes % 10) { //            a  b  c  d  e  f  g
    case 0: {vfdDisplay::character c = {1, 1, 1, 1, 1, 1, 0}; _screen.digit[3] = c; break;}
    case 1: {vfdDisplay::character c = {0, 1, 1, 0, 0, 0, 0}; _screen.digit[3] = c; break;}
    case 2: {vfdDisplay::character c = {1, 1, 0, 1, 1, 0, 1}; _screen.digit[3] = c; break;}
    case 3: {vfdDisplay::character c = {1, 1, 1, 1, 0, 0, 1}; _screen.digit[3] = c; break;}
    case 4: {vfdDisplay::character c = {0, 1, 1, 0, 0, 1, 1}; _screen.digit[3] = c; break;}
    case 5: {vfdDisplay::character c = {1, 0, 1, 1, 0, 1, 1}; _screen.digit[3] = c; break;}
    case 6: {vfdDisplay::character c = {1, 0, 1, 1, 1, 1, 1}; _screen.digit[3] = c; break;}
    case 7: {vfdDisplay::character c = {1, 1, 1, 0, 0, 0, 0}; _screen.digit[3] = c; break;}
    case 8: {vfdDisplay::character c = {1, 1, 1, 1, 1, 1, 1}; _screen.digit[3] = c; break;}
    case 9: {vfdDisplay::character c = {1, 1, 1, 1, 0, 1, 1}; _screen.digit[3] = c; break;}
  }
  _updateMultiplex();
}

void vfdDisplay::setDP(bool dp1, bool dp2){
  if (dp2 && dp1)   {_screen.dp[0] = 1; _screen.dp[1] = 1;}
  if (dp2 && !dp1)  {_screen.dp[0] = 1; _screen.dp[1] = 0;}
  if (!dp2 && dp1)  {_screen.dp[0] = 0; _screen.dp[1] = 1;}
  if (!dp2 && !dp1) {_screen.dp[0] = 0; _screen.dp[1] = 0;}

  _updateMultiplex();
}

void vfdDisplay::setCharacter(char character, int pos){
  switch (character) {
    //                                    a  b  c  d  e  f  g
    case '0': {vfdDisplay::character c = {1, 1, 1, 1, 1, 1, 0}; _screen.digit[pos] = c; break;}
    case '1': {vfdDisplay::character c = {0, 1, 1, 0, 0, 0, 0}; _screen.digit[pos] = c; break;}
    case '2': {vfdDisplay::character c = {1, 1, 0, 1, 1, 0, 1}; _screen.digit[pos] = c; break;}
    case '3': {vfdDisplay::character c = {1, 1, 1, 1, 0, 0, 1}; _screen.digit[pos] = c; break;}
    case '4': {vfdDisplay::character c = {0, 1, 1, 0, 0, 1, 1}; _screen.digit[pos] = c; break;}
    case '5': {vfdDisplay::character c = {1, 0, 1, 1, 0, 1, 1}; _screen.digit[pos] = c; break;}
    case '6': {vfdDisplay::character c = {1, 0, 1, 1, 1, 1, 1}; _screen.digit[pos] = c; break;}
    case '7': {vfdDisplay::character c = {1, 1, 1, 0, 0, 0, 0}; _screen.digit[pos] = c; break;}
    case '8': {vfdDisplay::character c = {1, 1, 1, 1, 1, 1, 1}; _screen.digit[pos] = c; break;}
    case '9': {vfdDisplay::character c = {1, 1, 1, 1, 0, 1, 1}; _screen.digit[pos] = c; break;}
    case 'a': {vfdDisplay::character c = {1, 1, 1, 0, 1, 1, 1}; _screen.digit[pos] = c; break;}
    case 'b': {vfdDisplay::character c = {0, 0, 1, 1, 1, 1, 1}; _screen.digit[pos] = c; break;}
    case 'c': {vfdDisplay::character c = {1, 0, 0, 1, 1, 1, 0}; _screen.digit[pos] = c; break;}
    case 'd': {vfdDisplay::character c = {0, 1, 1, 1, 1, 0, 1}; _screen.digit[pos] = c; break;}
    case 'e': {vfdDisplay::character c = {1, 0, 0, 1, 1, 1, 1}; _screen.digit[pos] = c; break;}
    case 'f': {vfdDisplay::character c = {1, 0, 0, 0, 1, 1, 1}; _screen.digit[pos] = c; break;}
    case 'g': {vfdDisplay::character c = {1, 1, 1, 1, 0, 1, 1}; _screen.digit[pos] = c; break;}
    case 'h': {vfdDisplay::character c = {0, 1, 1, 0, 1, 1, 1}; _screen.digit[pos] = c; break;}
    case 'i': {vfdDisplay::character c = {0, 0, 0, 0, 1, 1, 0}; _screen.digit[pos] = c; break;}
    case 'j': {vfdDisplay::character c = {0, 1, 1, 1, 1, 0, 0}; _screen.digit[pos] = c; break;}
    case 'k': {vfdDisplay::character c = {0, 0, 0, 0, 1, 1, 1}; _screen.digit[pos] = c; break;}
    case 'l': {vfdDisplay::character c = {0, 0, 0, 1, 1, 1, 0}; _screen.digit[pos] = c; break;}
    case 'm': {vfdDisplay::character c = {1, 0, 1, 0, 1, 0, 0}; _screen.digit[pos] = c; break;}
    case 'n': {vfdDisplay::character c = {0, 0, 1, 0, 1, 0, 1}; _screen.digit[pos] = c; break;}
    case 'o': {vfdDisplay::character c = {1, 1, 1, 1, 1, 1, 0}; _screen.digit[pos] = c; break;}
    case 'p': {vfdDisplay::character c = {1, 1, 0, 0, 1, 1, 1}; _screen.digit[pos] = c; break;}
    case 'q': {vfdDisplay::character c = {1, 1, 1, 0, 0, 1, 1}; _screen.digit[pos] = c; break;}
    case 'r': {vfdDisplay::character c = {0, 0, 0, 0, 1, 0, 1}; _screen.digit[pos] = c; break;}
    case 's': {vfdDisplay::character c = {1, 0, 1, 1, 0, 1, 1}; _screen.digit[pos] = c; break;}
    case 't': {vfdDisplay::character c = {0, 0, 0, 1, 1, 1, 1}; _screen.digit[pos] = c; break;}
    case 'u': {vfdDisplay::character c = {0, 1, 1, 1, 1, 1, 0}; _screen.digit[pos] = c; break;}
    case 'v': {vfdDisplay::character c = {0, 0, 1, 1, 1, 0, 0}; _screen.digit[pos] = c; break;}
    case 'w': {vfdDisplay::character c = {0, 1, 0, 1, 0, 1, 0}; _screen.digit[pos] = c; break;}
    case 'x': {vfdDisplay::character c = {0, 1, 1, 0, 0, 0, 1}; _screen.digit[pos] = c; break;}
    case 'y': {vfdDisplay::character c = {0, 1, 1, 1, 0, 1, 1}; _screen.digit[pos] = c; break;}
    case 'z': {vfdDisplay::character c = {0, 1, 0, 0, 1, 0, 1}; _screen.digit[pos] = c; break;}
    case '-': {vfdDisplay::character c = {0, 0, 0, 0, 0, 0, 1}; _screen.digit[pos] = c; break;}
    default:  {vfdDisplay::character c = {0, 0, 0, 0, 0, 0, 0}; _screen.digit[pos] = c; break;}
  }

  _updateMultiplex();
}

void vfdDisplay::print(char* text){
  setDP(0,0);
  setCharacter(text[0],0);
  setCharacter(text[1],1);
  setCharacter(text[2],2);
  setCharacter(text[3],3);
}

void vfdDisplay::setScreen(screen s){
  _screen = s;
  _updateMultiplex();
}
