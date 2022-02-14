/**
 *  @filename   :   epd2in7b-demo.ino
 *  @brief      :   2.7inch e-paper display (B) demo
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     July 17 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
/*
 *
 * Copyright (c) 2018 Seeed Technology Co., Ltd.
 * Website    : www.seeed.cc
 * Author     : downey
 * Create Time: Dec 2018
 * Change Log :
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "imagedata.h"
#include <SPI.h>
#include <Wire.h>
#include <epd2in7b.h>
#include <epdpaint.h>

// #ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
//   #define SERIAL SerialUSB
// #else
//   #define SERIAL Serial
// #endif

#define COLORED 1
#define UNCOLORED 0

#define key1 5
#define key2 6

unsigned char sw1_flag = 0;
unsigned char sw2_flag = 0;

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    //  Wire.begin();
    pinMode(key1, INPUT_PULLUP);
    pinMode(key2, INPUT_PULLUP);
    pinMode(18, OUTPUT);
    pinMode(19, OUTPUT);
    digitalWrite(18, HIGH);
    digitalWrite(19, LOW);
    Epd epd;

    Serial.println("Grove i2c port OK");
    if (epd.Init() != 0) {
        Serial.println("e-Paper init failed");
        return;
    } else
        Serial.println("e-Paper init OK");

    /* This clears the SRAM of the e-paper display */
    epd.ClearFrame();

    /**
     * Due to RAM not enough in Arduino UNO, a frame buffer is not allowed.
     * In this case, a smaller image buffer is allocated and you have to
     * update a partial display several times.
     * 1 byte = 8 pixels, therefore you have to set 8*N pixels at a time.
     */
    unsigned char image[1024];
    Paint paint(image, 176, 24); // width should be the multiple of 8

    paint.Clear(UNCOLORED);
    paint.DrawStringAt(0, 0, "e-Paper Demo", &Font16, COLORED);
    epd.TransmitPartialBlack(paint.GetImage(), 16, 32, paint.GetWidth(), paint.GetHeight());

    paint.Clear(COLORED);
    paint.DrawStringAt(2, 2, "Hello world!", &Font20, UNCOLORED);
    epd.TransmitPartialRed(paint.GetImage(), 0, 64, paint.GetWidth(), paint.GetHeight());

    paint.SetWidth(64);
    paint.SetHeight(64);

    paint.Clear(UNCOLORED);
    paint.DrawRectangle(0, 0, 40, 50, COLORED);
    paint.DrawLine(0, 0, 40, 50, COLORED);
    paint.DrawLine(40, 0, 0, 50, COLORED);
    epd.TransmitPartialBlack(paint.GetImage(), 10, 130, paint.GetWidth(), paint.GetHeight());

    paint.Clear(UNCOLORED);
    paint.DrawCircle(32, 32, 30, COLORED);
    epd.TransmitPartialBlack(paint.GetImage(), 90, 120, paint.GetWidth(), paint.GetHeight());

    paint.Clear(UNCOLORED);
    paint.DrawFilledRectangle(0, 0, 40, 50, COLORED);
    epd.TransmitPartialRed(paint.GetImage(), 10, 200, paint.GetWidth(), paint.GetHeight());

    paint.Clear(UNCOLORED);
    paint.DrawFilledCircle(32, 32, 30, COLORED);
    epd.TransmitPartialRed(paint.GetImage(), 90, 190, paint.GetWidth(), paint.GetHeight());

    /* This displays the data from the SRAM in e-Paper module */
    epd.DisplayFrame();

    /* This displays an image */
    epd.DisplayFrame(IMAGE_BLACK, IMAGE_RED);

    Serial.println("Setup will end soon");

    /* Deep sleep */
    epd.Sleep();

    Serial.println("Setup end");
}
/*
unsigned char Scann_i2c_device(void)
{
  byte error, address;
  int nDevices = 0;
  for(address=0; address < 127; address++ )
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknow error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }
  }
  return nDevices;
}
*/

void loop()
{
    // put your main code here, to run repeatedly:
    if (!digitalRead(key1)) {
        Serial.println("key1 pressed");
        sw1_flag = 1;
    }

    if (!digitalRead(key2)) {
        Serial.println("key2 pressed");
        sw2_flag = 1;
    }
    if (sw1_flag && sw2_flag) {
        digitalWrite(18, LOW);
        digitalWrite(19, HIGH);
        Serial.println("OK.");
    }
}
