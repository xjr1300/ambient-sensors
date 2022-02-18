#include <Wire.h>

#define COMMAND_LED_OFF 0x00
#define COMMAND_LED_ON 0x01
#define COMMAND_GET_VALUE 0x05
#define COMMAND_NOTHING_NEW 0x99

// センサーのデフォルトのI2Cアドレス
const byte ADDRESS = 0x28;

/// Sparkfun Qwiic土壌水分センサー
class SoilMoistureSensor {
   public:
    // デフォルトコンストラクタ
    void SoilMoistureSensor(void) {}

    // 初期化する。
    // Arguments:
    //  times: センサーが接続されているか確認する回数。
    //  interval: センサーが接続されているか確認する間隔秒数。
    // Returns:
    //  センサーが接続されていることを確認できた場合はtrue。確認できなかった場合はfalse。
    bool init(int times, int interval) {
        for (int i = 0; i < times; ++i) {
            Wire.beginTransmission(ADDRESS);
            if (Wire.endTransmission() == 0) {
                return true;
            }
            delay(interval);
        }
        return false;
    }

    // 土壌水分量を測定する。
    //
    // Returns:
    //  土壌水分量。マイナスの値が返却された場合は測定エラー。
    int32_t measure(void) {
        // 測定する命令を発効
        Wire.beginTransmission(ADDRESS);
        Wire.write(COMMAND_GET_VALUE);
        Wire.endTransmission();

        // 測定値を送信する命令を発効
        Wire.requestFrom(ADDRESS, 2);

        // 送信された測定値を読み込み
        for (int i = 0; i < times; ++i) {
            while (Wire.available()) {
                uint8_t adc_value_l = Wire.read();
                uint8_t adc_value_h = Wire.read();
                uint16_t adc_value = adc_value_h;
                adc_value <<= 8;
                adc_value |= adc_value_l;
                return (int32_t)adc_value;
            }
        }
        return ~(int32_t)0;
    }
}
