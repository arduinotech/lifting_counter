#ifndef Config_h_
#define Config_h_

// I2C device found at address 0x27  !
// I2C device found at address 0x50  !
// I2C device found at address 0x68  !

// =================================
// ============= PINS ==============
// =================================

#define SPEAKER_PIN 5
#define HC_SR04_ECHO_PIN 6
#define HC_SR04_TRIG_PIN 7

// =================================
// ============= OTHERS ============
// =================================

// Дисплей
#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

// интервалы обновления (миллисекунды)
#define DATE_TIME_UPDATE_INTERVAL 1000
#define DISTANCE_UPDATE_INTERVAL 100
#define BACKLIGHT_OFF_INTERVAL 30000

// расстояния
#define EMPTY_DISTANCE 100
#define LIFTING_DISTANCE 32

#define HEAD_STAGE_INTERVAL 800
#define ADD_COUNT_INTERVAL 800

#define BACKLIGHT_ON_INTERVAL 300


#endif