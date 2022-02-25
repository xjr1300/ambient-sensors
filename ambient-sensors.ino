#define __SERIAL_MONITOR__
// #define __SD_CARD_LOGGER__
#include <SPI.h>
#include <Wire.h>

#include "illuminance_sensor.h"
#include "lcd_display.h"
#include "led.h"
#ifdef __SD_CARD_LOGGER__
#include "sd_card_logger.h"
#endif
#ifdef __SERIAL_MONITOR__
#include "serial_monitor.h"
#endif
#include "soil_moisture_sensor.h"
#include "temp_hum_sensor.h"

#ifdef __SERIAL_MONITOR__
// シリアルモニター
#define SERIAL_BOARATE 9600
SerialMonitor serial_monitor(SERIAL_BOARATE);
#endif

// LCDディスプレイ
LcdDisplay lcd_display;

// LED
#define LED_PIN 3
#define MAX_LED_VALUE 80
#define LED_DELAY 200
#define LED_STEP 8
SwitchingLed led(LED_PIN, MAX_LED_VALUE, LED_STEP, LED_DELAY);

// 温湿度センサー
#define SI7021_ADDRESS 0x40
TempHumSensor temp_hum_sensor(SI7021_ADDRESS);

// 照度センサー
IlluminanceSensor ill_sensor;

// 土壌水分センサー
SoilMoistureSensor moist_sensor(10, 10);

#ifdef __SD_CARD_LOGGER__
// SDカードロガー
// 100万データ出力したらファイルをローテーション
SDCardLogger logger(SD_CARD_CHIP_UNO, 1000000, 10);
#endif

void setup(void) {
    // I2Cの利用を開始
    Wire.begin();
    // SPIの利用を開始
    SPI.begin();

#ifdef __SERIAL_MONITOR__
    // シリアルモニタを初期化
    serial_monitor.init();
#endif
    // lcd_displayディスプレイを初期化
    // lcd_display.init(7, 8, 9, 10, 11, 12);
    lcd_display.init(7, 8, 9, 4, 5, 6);
    // LEDをの初期化
    led.init();
    // 温湿度センサーを初期化
    temp_hum_sensor.init();
    // 環境光照度センサーを初期化
    if (!ill_sensor.init()) {
        lcd_display.print_row(0, "Ill err.");
        return;
    }
    // 土壌水分センサーを初期化
    if (!moist_sensor.init()) {
        lcd_display.print_row(0, "Moist err.");
        return;
    }
#ifdef __SD_CARD_LOGGER__
    // SDカードロガーを初期化
    if (!logger.init()) {
        lcd_display.print_row(0, "Card err.");
        return;
    }
#endif

    // 測定を開始
    lcd_display.print_row(0, "Start...");
    delay(1000);
}

void loop(void) {
    // 温湿度値
    float temp, hum;
    // 環境光照度値
    float als;
    // 土壌水分量
    int16_t moist;

    // 温度と湿度を測定
    temp = temp_hum_sensor.measure_temp();
    hum = temp_hum_sensor.measure_hum();
    // 環境光照度を測定
    ill_sensor.measure(&als);
    // 土壌水分量を測定
    moist = moist_sensor.measure();

    // 測定値を出力
#ifdef __SERIAL_MONITOR__
    serial_monitor.print_measured_values(temp, hum, als, moist);
#endif
    lcd_display.print_measured_values(temp, hum, als, moist);
#ifdef __SD_CARD_LOGGER__
    logger.write_measured_values(temp, hum, als, moist);
#endif

    // LEDを点灯または消灯
    led.on_off();
}
