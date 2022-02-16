#include "hardware/Config.h"
#include "hardware/Display.h"
#include <Arduino.h>
#include <DS1307RTC.h>
#include <Streaming.h>

Display *display;

void setup()
{
    pinMode(SPEAKER_PIN, OUTPUT);
    pinMode(HC_SR04_TRIG_PIN, OUTPUT);
    pinMode(HC_SR04_ECHO_PIN, INPUT);

    Serial.begin(115200);
    while (!Serial) {
    }

    Serial << "Setup...";

    display = new Display(LCD_ADDR, LCD_COLS, LCD_ROWS);

    // Проверяем RTC
    Serial << "Checking RTC..." << endl;
    tmElements_t tm;

    // tm.Year = 52;
    // tm.Month = 2;
    // tm.Day = 16;
    // tm.Hour = 17;
    // tm.Minute = 11;

    // RTC.write(tm);


    if (RTC.read(tm)) {
        Serial << "Current time: " << ((tm.Hour < 10) ? "0" : "") << tm.Hour << ":" << ((tm.Minute < 10) ? "0" : "")
               << tm.Minute << ":" << ((tm.Second < 10) ? "0" : "") << tm.Second << " " << tmYearToCalendar(tm.Year)
               << "-" << ((tm.Month < 10) ? "0" : "") << tm.Month << "-" << ((tm.Day < 10) ? "0" : "") << tm.Day << endl;
    } else {
        Serial << "RTC fail!" << endl;
    }

    Serial << " OK" << endl;
}

void loop()
{
    static uint32_t lastDateTimeUpdate = 0;
    static uint32_t lastDistanceUpdate = 0;
    static uint32_t lastNear = 0;
    static uint32_t lastAddCount = 0;
    static uint32_t lastEmpty = 0;

    static uint32_t headLiftingTime = 0;
    static bool addToCount = false;

    static uint32_t allCount = 0;
    static uint32_t dayCount = 0;

    static uint8_t lastClearDayCountDay = 0;

    uint32_t now = millis();

    if (lastDateTimeUpdate > now) {
        lastDateTimeUpdate = now;
    }

    if (lastDistanceUpdate > now) {
        lastDistanceUpdate = now;
    }

    if (lastNear > now) {
        lastNear = now;
    }

    if (lastAddCount > now) {
        lastAddCount = now;
    }

    if (lastEmpty > now) {
        lastEmpty = now;
    }

    if (now > (lastDateTimeUpdate + DATE_TIME_UPDATE_INTERVAL)) {
        tmElements_t tm;
        RTC.read(tm);
        String date, time;
        date = ((tm.Day < 10) ? "0" : "") + String(tm.Day) + "-" + ((tm.Month < 10) ? "0" : "") + String(tm.Month) + "-" + (tmYearToCalendar(tm.Year) - 2000);
        display->showRightText(date, 0);
        time = String("   ") + ((tm.Hour < 10) ? "0" : "") + String(tm.Hour) + ":" + ((tm.Minute < 10) ? "0" : "") + String(tm.Minute);
        display->showRightText(time, 1);

        if ((tm.Hour == 0) && (tm.Minute == 0) && (lastClearDayCountDay != tm.Day)) {
            dayCount = 0;
            lastClearDayCountDay = tm.Day;
        }

        display->showLeftText(String(dayCount), 0);
        display->showLeftText(String(allCount), 1);

        lastDateTimeUpdate = now;
    }

    if (now > (lastDistanceUpdate + DISTANCE_UPDATE_INTERVAL)) {

        int duration;
        int distance;

        digitalWrite(HC_SR04_TRIG_PIN, LOW);
        delayMicroseconds(2);
        digitalWrite(HC_SR04_TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(HC_SR04_TRIG_PIN, LOW);

        duration = pulseIn(HC_SR04_ECHO_PIN, HIGH);
        distance = duration / 58;
        Serial << "distance = " <<  distance << " cm, " << "now = " << now << ", headLiftingTime = " << headLiftingTime << ", addToCount = " << (addToCount ? "true" : "false") << ", allCount = " << allCount << ", dayCount = " << dayCount << ", lastDateTimeUpdate = " << lastDateTimeUpdate << endl;

        if (distance < EMPTY_DISTANCE) {
            // todo Не включать сразу, подождать, чтобы расстояние еще раз пришло такое же (защит от паразитных значений)
            if (((now - lastEmpty) > BACKLIGHT_ON_INTERVAL) && !display->getBacklight()) {
                display->backlightOn();
            }
            lastNear = now;
        }



        if (distance > EMPTY_DISTANCE) {
            if (((now - lastNear) > BACKLIGHT_OFF_INTERVAL) && display->getBacklight()) {
                display->backlightOff();
            }
            lastEmpty = now;
        }

        if ((distance < LIFTING_DISTANCE)) {
            if (headLiftingTime == 0) {
                headLiftingTime = now;
            }

            if (((now - headLiftingTime) > HEAD_STAGE_INTERVAL) && !addToCount && ((now - lastAddCount) > ADD_COUNT_INTERVAL)) {
                allCount++;
                dayCount++;
                addToCount = true;
                tone(SPEAKER_PIN, 700, 100);
                lastAddCount = now;
                display->showLeftText(String(dayCount), 0);
                display->showLeftText(String(allCount), 1);
            }
        }

        if (distance > LIFTING_DISTANCE) {
            addToCount = false;
            headLiftingTime = 0;
        }




        lastDistanceUpdate = now;
    }
}
