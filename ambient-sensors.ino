#include <Wire.h>
#include "SparkFun_Si7021_Breakout_Library.h"
#include "RPR-0521RS.h"

#include "led.h"
#include "serial_monitor.h"
#include "lcd_display.h"
#include "illuminance_sensor.h"

// Si7021スレーブアドレス
#define SI7021_ADDRESS 0x40

// LED
const int LED_PIN = 5;
const int MAX_LED_VALUE = 100;
const int LED_DELAY = 200;
const int LED_STEP = 10;
SwitchingLed led(LED_PIN, MAX_LED_VALUE, LED_STEP, LED_DELAY);

// シリアルモニター
#define SERIAL_BOARATE 9600
SerialMonitor monitor(SERIAL_BOARATE);

// LCDディスプレイ
LcdDisplay lcd;

// Si7021センサーインスタンス
Weather sensor;

// 照度センサー
IlluminanceSensor ill_sensor;

// 温度湿度構造体
struct TempHum {
  // 温度(℃)
  float temp;
  // 湿度(%)
  float hum;
};
    // 華氏の温度を摂氏に変換する。
    //
    // Arguments:
    //  f: 華氏。
    //
    // Returns:
    //  摂氏。
    float f_to_c(float f) {
      return (f - 32.0) / 1.8;
    }

// Si7021で温度と湿度を計測した値を返却する。
//
// Arguments:
//  pTempHum: 測定値した温度と湿度を記録する構造体のポインタ。
void getTempHum(TempHum *pTempHum) {
  pTempHum->temp = f_to_c(sensor.getTempF());
  pTempHum->hum = sensor.getRH();
}

void setup() {
  // LEDをの初期化
  led.init();
  // シリアルモニタを初期化
  monitor.init();
  // LCDディスプレイを初期化
  lcd.init(7, 8, 9, 10, 11, 12);

  // I2Cの利用を開始
  Wire.begin();
  lcd.print_row(0, "I2C init.");
  Serial.println("[INFO] I2C was initialized.");

  // Si7021をリセット
  Wire.beginTransmission(SI7021_ADDRESS);
  Wire.write(0xFE); // リセットコマンドアドレス
  Wire.endTransmission();
  lcd.print_row(0, "Si7021 init.");
  Serial.println("[INFO] Si7021 was initialized.");
  delay(15);

  // Si7021センサーの存在を確認
  if (sensor.begin()) {
    Serial.println("[INFO] Si7021 was detected.");
    lcd.print_row(0, "Si7021 detected.");
  } else {
    Serial.println("[ERROR] Si7021 was not detected.");
    lcd.print_row(0, "Si7021 fail.");
    return;
  }

  // 照度センサーを初期化
  if (ill_sensor.init()) {
    Serial.println("[INFO] Illuminance sensor was initialized.");
    lcd.print_row(0, "RPR-0521RS init.");
  } else {
    Serial.println("[ERROR] Illuminance sensor could not initialized.");
    lcd.print_row(0, "Ill sensor fail.");
    return;
  }

  // 測定を開始
  lcd.print_row(0, "Start measuring...");
  delay(500);
}

void loop() {
  // 測定値
  TempHum tempHum;
  // 照度。
  bool r_ill; float ill;

  // 温度と湿度を計測
  getTempHum(&tempHum);
  // 照度を計測
  r_ill = ill_sensor.measure(&ill); 
  // 測定値を出力
  monitor.print(tempHum.temp, tempHum.hum, ill);
  lcd.print_measured_values(tempHum.temp, tempHum.hum, ill);

  // LEDを点灯または消灯
  led.on_off();
}
