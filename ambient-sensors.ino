#include <Wire.h>

#include "illuminance_sensor.h"
#include "lcd_display.h"
#include "led.h"
#include "serial_monitor.h"
#include "soil_moisture_sensor.h"
#include "temp_hum_sensor.h"

// シリアルモニター
#define SERIAL_BOARATE 9600
SerialMonitor serial_monitor(SERIAL_BOARATE);

// LCDディスプレイ
LcdDisplay lcd_display;

// LED
const int LED_PIN = 5;
const int MAX_LED_VALUE = 80;
const int LED_DELAY = 200;
const int LED_STEP = 8;
SwitchingLed led(LED_PIN, MAX_LED_VALUE, LED_STEP, LED_DELAY);

// 温湿度センサー
#define SI7021_ADDRESS 0x40
TempHumSensor temp_hum_sensor(SI7021_ADDRESS);

// 照度センサー
IlluminanceSensor ill_sensor;

// 土壌水分センサー
SoilMoistureSensor moist_sensor(10, 10);

void setup(void) {
    // I2Cの利用を開始
    Wire.begin();

    // シリアルモニタを初期化
    serial_monitor.init();
    // lcd_displayディスプレイを初期化
    lcd_display.init(7, 8, 9, 10, 11, 12);
    // LEDをの初期化
    led.init();
    // 温湿度センサーを初期化
    temp_hum_sensor.init();
    Serial.println("[INFO] Temp & hum sensor was initialized.");
    lcd_display.print_row(0, "Temp-Hum init.");
    // 環境光照度センサーを初期化
    if (!ill_sensor.init()) {
        Serial.println("[ERROR] Illuminance sensor could not initialized.");
        lcd_display.print_row(0, "Ill sensor err.");
        return;
    }
    Serial.println("[INFO] Illuminance sensor was initialized.");
    lcd_display.print_row(0, "Ill sensor init.");
    // 土壌水分センサーを初期化
    if (!moist_sensor.init()) {
        Serial.println("[ERROR] Soil moisture sensor could not initialized.");
        lcd_display.print_row(0, "Soil sensor err.");
        return;
    }
    Serial.println("[INFO] Soil moisture sensor was initialized.");
    lcd_display.print_row(0, "Soil sensor init.");

    // 測定を開始
    lcd_display.print_row(0, "Start measuring...");
    delay(1000);
}

void loop(void) {
    // 温湿度値
    float temp, hum;
    // 環境光照度値
    bool r_als;
    float als;
    // 土壌水分量
    int32_t moist;

    // 温度と湿度を測定
    temp = temp_hum_sensor.measure_temp();
    hum = temp_hum_sensor.measure_hum();
    // 環境光照度を測定
    r_als = ill_sensor.measure(&als);
    // 土壌水分量を測定
    moist = moist_sensor.measure();

    // 測定値を出力
    serial_monitor.print_measured_values(temp, hum, als, moist);
    lcd_display.print_measured_values(temp, hum, als, moist);

    // LEDを点灯または消灯
    led.on_off();
}
