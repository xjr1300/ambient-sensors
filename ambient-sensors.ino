#include <Wire.h>
#include <LiquidCrystal.h>
#include "SparkFun_Si7021_Breakout_Library.h"
#include "RPR-0521RS.h"

// Si7021スレーブアドレス
#define SI7021_ADDRESS 0x40

// Si7021センサーインスタンス
Weather sensor;

// RPR0521RSセンサーインスタンス
RPR0521RS rpr0521rs;

// シリアルモニタに測定値を表示するときに使用するバッファ
char serialBuff[128], tempBuf[16], humBuf[16], alsBuf[32];

// LCDディスプリインスタンスの構築とピン設定
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// LCDに出力する文字列を記憶するバッファ
char lcdBuf[24];

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

// 測定した値をシリアルモニタに表示する。
//
// Arguments:
//  pTempHum: 温度と湿度を記録した構造体のポインタ。
//  pAlsValue: 照度を記録した変数のポインタ。
void printMeasuredValuesToSerial(TempHum *pTempHum, float *pAlsValue) {
  dtostrf(pTempHum->temp, -1, 1, tempBuf);
  dtostrf(pTempHum->hum, -1, 1, humBuf);
  dtostrf(*pAlsValue, -1, 0, alsBuf);
  sprintf(serialBuff, "Temp: %s[C], Hum: %s[%%], ALS: %s[lx]", tempBuf, humBuf, alsBuf);
  Serial.println(serialBuff);
}

// LCDディスプレイに文字列を表示する。
//
// Arguments:
//  row: 文字列を表示する行番号。
//  pBuf: 表示する文字列の先頭ポインタ。
void printLcd(int row, const char *pBuf) {
  lcd.setCursor(0, row);
  lcd.print(pBuf);
}

// 測定した値をLCDディスプレイに表示する。
//
// Arguments:
//  pTempHum: 温度と湿度を記録した構造体のポインタ。
//  pAlsValue: 照度を記録した変数のポインタ。
void printMeasuredValuesToLcd(TempHum *pTempHum, float *pAlsValue) {
  lcd.clear();
  // 温度と湿度
  dtostrf(pTempHum->temp, -1, 1, tempBuf);
  dtostrf(pTempHum->hum, -1, 1, humBuf);
  sprintf(lcdBuf, "%s[C] %s[%%]", tempBuf, humBuf);
  printLcd(0, lcdBuf);
  // 照度
  dtostrf(*pAlsValue, -1, 0, alsBuf);
  sprintf(lcdBuf, "%s[lx]", alsBuf);
  printLcd(1, lcdBuf);
}

void setup() {
  // LCDディスプレイの準備
  lcd.begin(16, 2);
  lcd.clear();
  printLcd(0, "Initializing...");

  // シリアルモニタの利用を開始
  Serial.begin(9600);
  // シリアルモニタが準備できるまでループ
  while (!Serial) {}
  printLcd(0, "Serial init.");
  Serial.println("[INFO] Initializing.");

  // I2Cの利用を開始
  Wire.begin();
  printLcd(0, "I2C init.");
  Serial.println("[INFO] I2C was initialized.");

  // Si7021をリセット
  Wire.beginTransmission(SI7021_ADDRESS);
  Wire.write(0xFE); // リセットコマンドアドレス
  Wire.endTransmission();
  printLcd(0, "Si7021 init.");
  Serial.println("[INFO] Si7021 was initialized.");
  delay(15);

  // Si7021センサーの存在を確認
  if (sensor.begin()) {
    Serial.println("[INFO] Success - Si7021 was detected.");
  } else {
    Serial.println("[ERROR] Fail - Si7021 was not detected.");
    return;
  }
  printLcd(0, "Si7021 detected.");

  // RPR-0521RSを初期化
  if (rpr0521rs.init() == 0) {
    Serial.println("[INFO] RPR0521RS was initialized.");
  } else {
    Serial.println("[ERROR] Fail - RPR0521RS could not initialized.");
    return;
  }
  printLcd(0, "RPR-0521RS init.");
  printLcd(0, "Start measuring...");
  delay(1000);
}

void loop() {
  // 測定値
  TempHum tempHum;
  unsigned short psValue; // PS: RPR-0521RS近接センサ
  float alsValue;         // ALS: Ambient light sensor(環境光センサー)
  // 温度と湿度を計測
  getTempHum(&tempHum);
  // 照度を計測
  bool isPrintSerial = rpr0521rs.get_psalsval(&psValue, &alsValue) == 0;
  // 測定値を出力
  if (isPrintSerial) {
    printMeasuredValuesToLcd(&tempHum, &alsValue);
    printMeasuredValuesToSerial(&tempHum, &alsValue);
  }

  delay(1000);
}
