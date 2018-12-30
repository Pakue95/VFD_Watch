#ifndef ANIMATIONS_H
#define ANIMATIONS_H

void wifiAnimation(int segment){

  //                gfedcba
  vfdDisplay::character c1 = {0,0,0,0,0,0,1};
  vfdDisplay::character c2 = {0,1,1,0,0,0,0};
  vfdDisplay::character c3 = {1,0,0,1,0,0,0};
  vfdDisplay::character c4 = {0,0,0,0,1,1,0};
  vfdDisplay::character c5 = {0,0,0,0,0,0,1};
  vfdDisplay::character c6 = {};

  vfdDisplay::screen s;
  s.digit[0] = c6;
  s.digit[1] = c6;
  s.digit[2] = c6;
  s.digit[3] = c6;

  s.digit[segment] = c1;
  vfd.setScreen(s);
  delay(150);
  s.digit[segment] = c2;
  vfd.setScreen(s);
  delay(150);
  s.digit[segment] = c3;
  vfd.setScreen(s);
  delay(150);
  s.digit[segment] = c4;
  vfd.setScreen(s);
  delay(150);
  s.digit[segment] = c5;
  vfd.setScreen(s);
  delay(150);
  s.digit[segment] = c6;
  vfd.setScreen(s);
  delay(80);
}

#endif
