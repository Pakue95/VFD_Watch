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
#define CHAR_3     1
#define CHAR_4     5
#define CHAR_5    14 //DP

class vfdDisplay{
  static Adafruit_MCP23017 mcp;
  Ticker tickerMultiplex;
public:
  struct character{ // data type of a single 7-segment
    bool a;
    bool b;
    bool c;
    bool d;
    bool e;
    bool f;
    bool g;
    // initalize bool variables as false to avoid unwanted side effects
    character() : a(0), b(0), c(0), d(0), e(0), f(0), g(0) {}
    character(bool a_, bool b_, bool c_, bool d_, bool e_, bool f_, bool g_) :
             a(a_), b(b_), c(c_), d(d_), e(e_), f(f_), g(g_) {}
  };
  struct screen{   // data type of a whole screen + decimal points
    character digit[4];
    bool dp[2] = {0};
  };
  vfdDisplay(void);
  ~vfdDisplay(void);
  void begin(
    uint8_t dutyCycle,
    uint32_t freq_multiplex,
    uint32_t freq_heat);
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
  void setScreen(screen);


private:
  screen _screen; // exchange varable
  static int _posMultiplex;
  static uint16_t _dataMultiplex[5];
  static void _nextMultiplex();
  uint8_t _dutyCycle;
  uint32_t _freqMultiplex;
  uint32_t _freqHeat;
  void _updateMultiplex();
};


#endif
