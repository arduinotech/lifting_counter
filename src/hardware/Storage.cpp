#include "Storage.h"
#include <EEPROM.h>

Date Storage::getDate()
{
    Date date;
    EEPROM.get(OFFSET_DATE, date);
    return date;
}

void Storage::putDate(Date date)
{
    EEPROM.put(OFFSET_DATE, date);
}

uint32_t Storage::getAllCount()
{
    uint32_t allCount;
    EEPROM.get(OFFSET_ALL_COUNT, allCount);
    return allCount;
}

void Storage::putAllCount(uint32_t allCount)
{
    EEPROM.put(OFFSET_ALL_COUNT, allCount);
}

uint32_t Storage::getDayCount()
{
    uint32_t dayCount;
    EEPROM.get(OFFSET_DAY_COUNT, dayCount);
    return dayCount;
}

void Storage::putDayCount(uint32_t dayCount)
{
    EEPROM.put(OFFSET_DAY_COUNT, dayCount);
}
