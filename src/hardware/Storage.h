#ifndef Storage_h_
#define Storage_h_

#include <Arduino.h>

#define OFFSET_DATE 0
#define OFFSET_ALL_COUNT 3
#define OFFSET_DAY_COUNT 7

struct Date
{
    uint8_t year; // offset from 1970;
    uint8_t month;
    uint8_t day;
};

class Storage
{
    public:
        Date getDate();
        void putDate(Date date);

        uint32_t getAllCount();
        void putAllCount(uint32_t allCount);

        uint32_t getDayCount();
        void putDayCount(uint32_t dayCount);
};

#endif