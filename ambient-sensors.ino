#include <Wire.h>

#include "illuminance_sensor.h"
#include "lcd_display.h"
#include "led.h"
#include "serial_monitor.h"
#include "temp_hum_sensor.h"

// シリアルモニター
#define SERIAL_BOARATE 9600
SerialMonitor serial_monitor(SERIAL_BOARATE);

// LCDディスプレイ
LcdDisplay lcd_display;

// LED
const int LED_PIN = 5;
const int MAX_LED_VALUE = 100;
const int LED_DELAY = 200;
const int LED_STEP = 10;
SwitchingLed led(LED_PIN, MAX_LED_VALUE, LED_STEP, LED_DELAY);

// 温湿度センサー
#define SI7021_ADDRESS 0x40
TempHumSensor temp_hum_sensor(SI7021_ADDRESS);

// 照度センサー
IlluminanceSensor ill_sensor;

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
    // 照度センサーを初期化
    if (!ill_sensor.init()) {
        Serial.println("[ERROR] Illuminance sensor could not initialized.");
        lcd_display.print_row(0, "Ill sensor fail.");
        return;
    }

    // 測定を開始
    lcd_display.print_row(0, "Start measuring...");
    delay(1000);
}

void loop(void) {
    // 温湿度値
    float temp, hum;
    // 照度値
    bool r_ill;
    float ill;

    // 温度と湿度を測定
    temp = temp_hum_sensor.measure_temp();
    hum = temp_hum_sensor.measure_hum();
    // 照度を測定
    r_ill = ill_sensor.measure(&ill);
    // 測定値を出力
    serial_monitor.print_measured_values(temp, hum, ill);
    lcd_display.print_measured_values(temp, hum, ill);
    // LEDを点灯または消灯
    led.on_off();
}
