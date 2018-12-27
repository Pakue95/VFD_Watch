#ifndef ANIMATIONS_H
#define ANIMATIONS_H

void wifiAnimation(int segment){
  //                gfedcba
  vfd.setSegment(0b01000000, segment);
  delay(150);
  vfd.setSegment(0b00000110, segment);
  delay(150);
  vfd.setSegment(0b00001001, segment);
  delay(150);
  vfd.setSegment(0b00110000, segment);
  delay(150);
  vfd.setSegment(0b01000000, segment);
  delay(80);
  vfd.setSegment(0b00000000, segment);
}

#endif
