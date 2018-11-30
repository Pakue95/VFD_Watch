#include <Arduino.h>
#include <Vfd_Display.h>



TaskHandle_t vfdTask;
/*
Ticker jitter;
bool dir = 1;
void PWMdir() {
  if(dir){
    ledcWrite(0, 0);
    ledcWrite(1, 90);
    dir = !dir;
  }
  else{
    ledcWrite(0, 90);
    ledcWrite(1, 0);
    dir = !dir;
  }
}

Ticker tickerMultiplex;
int charMultiplex = 3;
void multiplex(){
  charMultiplex++;
  if (charMultiplex > 4) charMultiplex = 0;
  switch (charMultiplex) {
    case 0: mcp.writeGPIOAB(0xAD5D); break;
    case 1: mcp.writeGPIOAB(0xBC5D); break;
    case 2: mcp.writeGPIOAB(0xC200); break;
    case 3: mcp.writeGPIOAB(0xAC5F); break;//A457
    case 4: mcp.writeGPIOAB(0xAC7D); break;
    default: break;
  }

}
*/

vfdDisplay vfd;

int seconds = 0;
int minutes = 0;

Ticker secondsTick;
Ticker minutesTick;

void secondUp(){
  seconds = (seconds + 1) % 60;
}

void minuteUp(){
  minutes = (minutes + 1) % 100;
}

void callHandler(void * pvParameters){
  while(1){
    vfd.handler();
  }
}

void setup() {
  Serial.begin(115200);


  xTaskCreatePinnedToCore(
     callHandler,                  /* pvTaskCode */
     "VFD_Handler",         /* pcName */
     1000,                  /* usStackDepth */
     NULL,                  /* pvParameters */
     1,                     /* uxPriority */
     &vfdTask,              /* pxCreatedTask */
     1);                    /* xCoreID */

  vfd.begin(120, 500, 10000);
  vfd.setHours(00);
  vfd.setMinutes(00);
  vfd.setDP(1,1);

  secondsTick.attach_ms(10, secondUp);
  minutesTick.attach_ms(600, minuteUp);
}

void loop() {
  /*
  digitalWrite(HEAT_INT1, HIGH);
  digitalWrite(HEAT_INT2, LOW);

  delayMicroseconds(0.5*dutyCycle*1000000.0/frequency);

  digitalWrite(HEAT_INT1, LOW);
  digitalWrite(HEAT_INT2, HIGH);

  delayMicroseconds(0.5*dutyCycle*1000000.0/frequency);

  digitalWrite(HEAT_INT1, LOW);
  digitalWrite(HEAT_INT2, LOW);
  */
  //delayMicroseconds((1.0-dutyCycle)*1000000.0/frequency);
  vfd.setHours(minutes);
  vfd.setMinutes(seconds);
}
