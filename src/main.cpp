#include "hardware/Config.h"
#include "hardware/Display.h"
#include "hardware/Storage.h"
#include <Arduino.h>
#include <DS1307RTC.h>

#include <Wire.h>
#include <TimeLib.h>


Display *display;
Storage *storage;

uint32_t allCount;
uint32_t dayCount;

Date date;

void resetDayCount()
{
    date = storage->getDate();
    tmElements_t tm;
    RTC.read(tm);
    if ((tm.Year != date.year) || (tm.Month != date.month) || (tm.Day != date.day)) {
        if (dayCount != 0) {
            dayCount = 0;
            if (dayCount != storage->getDayCount()) {
                storage->putDayCount(dayCount);
            }
        }
        date.year = tm.Year;
        date.month = tm.Month;
        date.day = tm.Day;
        storage->putDate(date);
    }
}

void showCurrentDateAndTime()
{
    tmElements_t tm;
    RTC.read(tm);
    String date, time;
    date = ((tm.Day < 10) ? "0" : "") + String(tm.Day) + "-" + ((tm.Month < 10) ? "0" : "") + String(tm.Month) + "-" + (tmYearToCalendar(tm.Year) - 2000);
    display->showRightText(date, 0);
    time = String("   ") + ((tm.Hour < 10) ? "0" : "") + String(tm.Hour) + ":" + ((tm.Minute < 10) ? "0" : "") + String(tm.Minute);
    display->showRightText(time, 1);
}

void showCounts()
{
    display->showLeftText("Day:" + String(dayCount), 0);
    display->showLeftText("All:" + String(allCount), 1);
}

void setup()
{
    pinMode(SPEAKER_PIN, OUTPUT);
    pinMode(HC_SR04_TRIG_PIN, OUTPUT);
    pinMode(HC_SR04_ECHO_PIN, INPUT);

    display = new Display(LCD_ADDR, LCD_COLS, LCD_ROWS);
    storage = new Storage();

    date = storage->getDate();
    allCount = storage->getAllCount();
    dayCount = storage->getDayCount();

    resetDayCount();
    showCurrentDateAndTime();
    showCounts();
}

void loop()
{
    static uint32_t lastDateTimeUpdate = 0;
    static uint32_t lastDistanceUpdate = 0;
    static uint32_t lastNear = 0;
    static uint32_t lastAddCount = 0;
    static uint32_t lastEmpty = 0;
    static uint32_t lastBackup = 0;

    static uint32_t headLiftingTime = 0;
    static bool addToCount = false;

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

    if (lastBackup > now) {
        lastBackup = now;
    }

    if (now > (lastDateTimeUpdate + DATE_TIME_UPDATE_INTERVAL)) {
        resetDayCount();
        showCurrentDateAndTime();
        showCounts();

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

        if (distance < EMPTY_DISTANCE) {
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
                showCounts();
            }
        }

        if (distance > LIFTING_DISTANCE) {
            addToCount = false;
            headLiftingTime = 0;
        }
        lastDistanceUpdate = now;
    }

    if (now > (lastBackup + BACKUP_COUNTS_INTERVAL)) {
        if (allCount != storage->getAllCount()) {
            storage->putAllCount(allCount);
        }

        if (dayCount != storage->getDayCount()) {
            storage->putDayCount(dayCount);
        }

        lastBackup = now;
    }
}



// uncomment for set datetime!
/*
const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

tmElements_t tm;

bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}


void setup() {
  bool parse=false;
  bool config=false;

  if (getDate(__DATE__) && getTime(__TIME__)) {
    parse = true;
    if (RTC.write(tm)) {
      config = true;
    }
  }

  Serial.begin(9600);
  while (!Serial) ; // wait for Arduino Serial Monitor
  delay(200);
  if (parse && config) {
    Serial.print("DS1307 configured Time=");
    Serial.print(__TIME__);
    Serial.print(", Date=");
    Serial.println(__DATE__);
  } else if (parse) {
    Serial.println("DS1307 Communication Error :-{");
    Serial.println("Please check your circuitry");
  } else {
    Serial.print("Could not parse info from the compiler, Time=\"");
    Serial.print(__TIME__);
    Serial.print("\", Date=\"");
    Serial.print(__DATE__);
    Serial.println("\"");
  }
}

void loop() {
}

*/
