#ifndef VFD_DISPLAY_H
#define VFD_DISPLAY_H

#include <Arduino.h>
#include <Ticker.h>
#include <Wire.h>
#include "Adafruit_MCP23017.h"

#define EN_24V     2
#define HEAT_EN   18
#define HEAT_INT1  5
#define HEAT_INT2 17
#define IO_RST    23
#define BAT       34
#define BTN0       0
#define BTN1      25
#define BTN2      35

#define SEG_A      4
#define SEG_B      2
#define SEG_C     13
#define SEG_D      0
#define SEG_E     11
#define SEG_F      3
#define SEG_G     10
#define SEG_DP1    9
#define SEG_DP2   15

#define CHAR_1     8
#define CHAR_2    12
#define CHAR_3    14
#define CHAR_4     1
#define CHAR_5     5

class vfdDisplay{
  //Ticker tickerJitter;

  static Adafruit_MCP23017 mcp;
  Ticker tickerMultiplex;
public:
  vfdDisplay(void);
  ~vfdDisplay(void);
  void begin(
    uint8_t dutyCycle,
    uint32_t freq_multiplex,
    uint32_t freq_heat);
  void set(
    uint8_t digit1,
    uint8_t digit2,
    uint8_t digit3,
    uint8_t digit4,
    uint8_t dots);
  void activate();
  void deactivate();
  void setHours(uint8_t hours);
  void setMinutes(uint8_t minutes);
  void setDP(bool dp1, bool dp2);
  uint8_t getDutyCycle();
  void setDutyCycle(uint8_t);
  uint32_t getFreqMultiplex();
  uint32_t getFreqHeat();
  void setCharacter(char character, int pos);
  void print(char* text);

private:
  // a b c d e f g dp1 dp2
  bool _digit1[9] = {0};
  bool _digit2[9] = {0};
  bool _digit3[9] = {0}; //dots
  bool _digit4[9] = {0};
  bool _digit5[9] = {0};
  static int _posMultiplex;
  static uint16_t _dataMultiplex[5];
  static void _nextMultiplex();
  uint8_t _dutyCycle;
  uint32_t _freqMultiplex;
  uint32_t _freqHeat;
  uint16_t _getMultiplex(uint8_t pos, bool *digit);
};


#endif
