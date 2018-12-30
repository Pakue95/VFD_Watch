#include <Arduino.h>
#include <Vfd_Display.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <WiFiUdp.h>
#include "OneButton.h"
#include "Password.h"

#define NTP_SERVER "de.pool.ntp.org"
#define TZ_INFO "WEST-1DWEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00" // Western European Time

WiFiUDP udp;


TaskHandle_t vfdTask;
vfdDisplay vfd;
#include "Animations.h"

OneButton button0(BTN0, true);
OneButton button1(BTN1, true);
OneButton button2(BTN2, true);

int seconds = 0;
int minutes = 0;
struct tm local;

Ticker updateTime;
Ticker shutdownTimer;
Ticker pullTime;
Ticker show35C3;


void initialBoot(){
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  for(int i = 0; WiFi.status() != WL_CONNECTED; (i = (++i % 4)) ) {
    wifiAnimation(i);
    Serial.print(".");
  }

  configTzTime(TZ_INFO, NTP_SERVER); // config system time via NTP server
  getLocalTime(&local, 10000);       // get time for 10s
  WiFi.mode( WIFI_MODE_NULL );
}


void setup() {
  Serial.begin(115200);
  vfd.begin(80, 1000, 10000);
  esp_sleep_wakeup_cause_t wakeup_cause;
  wakeup_cause = esp_sleep_get_wakeup_cause(); // check wakeup reason
  Serial.println(wakeup_cause);
  if (wakeup_cause != ESP_SLEEP_WAKEUP_EXT0) initialBoot();  // external reset -> iitialize

  setenv("TZ", TZ_INFO, 1); // reset timezone
  tzset();

  // initialize VFD again
  // filament duty cycle, multiplex frequency, filemant frequency
  //vfd.begin(110, 1000, 100000);
  getLocalTime(&local);
  vfd.setMinutes(local.tm_min);
  vfd.setHours(local.tm_hour);
  vfd.setDP(1,1);

  updateTime.attach(2, [](){
    // setup time
    vfd.setDP(1,1);
    getLocalTime(&local);
    Serial.println(&local, "Date: %d.%m.%y  Time: %H:%M:%S"); // print formated time
    vfd.setMinutes(local.tm_min);
    vfd.setHours(local.tm_hour);
  });

  /*
  // prepare shutdown in X seconds
  shutdownTimer.once(10, [](){
    vfd.deactivate();
    Serial.println("going into deep sleep...");
    delay(10);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_35,0);
    esp_deep_sleep_start();});
  */


  pullTime.attach(3600, [](){
    WiFi.mode(WIFI_MODE_STA);
    delay(1000);
    getLocalTime(&local);
    Serial.println("Pulled new time.");
    WiFi.mode(WIFI_MODE_NULL);});

  show35C3.attach(5, [](){
    vfd.print("35c3");
  });
}

void loop() {


  delay(2000);
}
