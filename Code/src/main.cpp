#include <Arduino.h>
#include <Vfd_Display.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <WiFiUdp.h>
#include "Password.h"
#include "ManualTimeSet.h"

#define NTP_SERVER "de.pool.ntp.org"
#define TZ_INFO "WEST-1DWEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00" // Western European Time

WiFiUDP udp;


TaskHandle_t vfdTask;
vfdDisplay vfd;
#include "Animations.h"



int seconds = 0;
int minutes = 0;
struct tm local;

Ticker updateTime;


void initialBoot(){
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  for(int i = 0, time = 0; WiFi.status() != WL_CONNECTED; ++i, ++time) {
    i = i % 4;
    wifiAnimation(i);
    Serial.print(".");
    if(time > 10){
      startManualTimeSet(vfd); //
      WiFi.mode( WIFI_OFF );
      return;
    }
  }

  configTzTime(TZ_INFO, NTP_SERVER); // config system time via NTP server
  getLocalTime(&local, 10000);       // get time for 10s
  setenv("TZ", TZ_INFO, 1); // reset timezone
  tzset();

  WiFi.mode( WIFI_OFF );
}


void setup() {
  Serial.begin(115200);
  vfd.begin(80, 1000, 10000);
  esp_sleep_wakeup_cause_t wakeup_cause = esp_sleep_get_wakeup_cause(); // check wakeup reason
  Serial.println(wakeup_cause);
  if (wakeup_cause != ESP_SLEEP_WAKEUP_EXT0) initialBoot();  // external reset -> initialize



  updateTime.attach(2, [](){
    // setup time
    vfd.setDP(1,1);
    time_t now;
    time(&now);
    tm *time = localtime(&now);
    
    // Serial.println(&local, "Date: %d.%m.%y  Time: %H:%M:%S"); // print formated time
    vfd.setMinutes(time->tm_min);
    vfd.setHours(time->tm_hour);
  });

}

void loop() {


}
