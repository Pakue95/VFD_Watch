#include <Arduino.h>
#include <Vfd_Display.h>
#include <NTP.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "OneButton.h"
#include "Password.h"

WiFiUDP udp;

NTP ntp(udp);



TaskHandle_t vfdTask;
vfdDisplay vfd;

OneButton button0(BTN0, false);
OneButton button1(BTN1, false);
OneButton button2(BTN2, false);

int seconds = 0;
int minutes = 0;

Ticker secondsTick;
Ticker minutesTick;
Ticker blinker;

void secondUp(){
  seconds = (seconds + 1) % 60;
}

void minuteUp(){
  minutes = (minutes + 1) % 100;
}

void manualSetTime();

void setup() {
  Serial.begin(115200);

  vfd.begin(120, 1000, 10000);
  vfd.setHours(00);
  vfd.setMinutes(50);
  vfd.setDP(1,1);

  secondsTick.attach_ms(10, secondUp);
  minutesTick.attach_ms(600, minuteUp);

  //manualSetTime();

  WiFi.begin(ssid, password);


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }


  ntp.begin();
  ntp.offset(0, 1, 0, 1); //days,hours,minutes,seconds
  WiFi.mode( WIFI_MODE_NULL ); //turn off Wifi

  //button.attachDoubleClick(manualSetTime);
}

//char* text = "here could be your message    ";
//int length = 26;

void loop() {
  vfd.setHours(ntp.hours());
  vfd.setMinutes(ntp.minutes());

  delay(200); // wait for 20 seconds before refreshing.
  //button0.tick();
  /*
  delay(3000);

  for (int i = 0; i < length+1; i++){
    vfd.setCharacter(text[i], 0);
    vfd.setCharacter(text[i+1], 1);
    vfd.setCharacter(text[i+2], 3);
    vfd.setCharacter(text[i+3], 4);
    delay(500);
  }
  */
}

/*
void manualSetTime(){
  vfd.setCharacter('-', 0);
  vfd.setCharacter('-', 1);
  vfd.setCharacter('-', 3);
  vfd.setCharacter('-', 4);

  bool timeOK = false;
  int pos = 0;
  int tValue = 0;
  int tHours = 0;
  int tMinutes = 0;
  bool blink = false;


  bool flagButton0 = false;
  static auto setFlagButton0 = [&flagButton0](){flagButton0 = true;};
  auto callButton0 =
    [&pos,&flagButton0](){
       if(flagButton0){
         pos++;
         flagButton0 = false;}};


  button0.attachClick(setFlagButton0);
  // button1.attachClick([=](int tValue){tValue = (tValue + 1) % 10; Serial.println(tValue);});
  // button2.attachClick([=](int tValue){tValue = (tValue + 1) % 10; Serial.println(tValue);});

  while(!timeOK){
    button0.tick();
    button1.tick();
    button2.tick();
  }
}*/
