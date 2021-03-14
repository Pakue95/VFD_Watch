#ifndef MANUAL_TIME_SET
#define MANUAL_TIME_SET

#include "OneButton.h"
#include <Vfd_Display.h>
#include <sys/time.h>

OneButton button_left(BTN0, true,true);
OneButton button_middle(BTN1, true,true);
OneButton button_right(BTN2, true,true);

uint manual_hours = 0;
uint manual_minutes = 0;
bool middlePressed = false;


static void hoursUp(){
    manual_hours = (manual_hours +1) % 24;
}

static void hoursDown(){
    manual_hours = (manual_hours + (24-1)) % 24;
}

static void minutesUp(){
    manual_minutes = (manual_minutes + 1) % 60;
}

static void minutesDown(){
    manual_minutes = (manual_minutes + (60-1)) % 60;
}

static void middleButtonWasPressed(){
    middlePressed = true;
}



void startManualTimeSet(vfdDisplay &vfd){
    button_left.attachClick(hoursDown);
    button_right.attachClick(hoursUp);
    button_middle.attachClick(middleButtonWasPressed);


    while(!middlePressed){
        button_left.tick();
        button_middle.tick();
        button_right.tick();
        vfd.setHours(manual_hours);
    }

    middlePressed = false;

    button_left.attachClick(minutesDown);
    button_right.attachClick(minutesUp);

    while(!middlePressed){
        button_left.tick();
        button_middle.tick();
        button_right.tick();
        vfd.setMinutes(manual_minutes);
    }

    middlePressed = false;

    // quick and dirty time set...
    timeval tv;
    tv.tv_sec = time_t(manual_hours*3600+manual_minutes*60);

    settimeofday(&tv, NULL);

}


#endif