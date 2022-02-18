#include <Wire.h>

const byte COMMAND_LED_OFF = 0x00;
const byte COMMAND_LED_ON = 0x01;
const byte COMMAND_GET_VALUE = 0x05;
const byte COMMAND_NOTHING_NEW = 0x99;

// センサーのデフォルトのI2Cアドレス
const byte ADDRESS = 0x28;

/// Sparkfun Qwiic土壌水分センサー
class SoilMoistureSensor {
   private:
    // センサーからの応答を取得する試行回数。
    int _times;
    // センサーから応答の取得する間隔ミリ秒
    int _interval;

    // デフォルトコンストラクタ
    SoilMoistureSensor(void) {}

   public:
    // コンストラクタ
    // Arguments:
    //  times: センサーからの応答を取得する試行回数。
    //  interval: センサーから応答の取得する間隔ミリ秒
    SoilMoistureSensor(int times, int interval) {
        this->_times = times;
        this->_interval = interval;
    }

    // 初期化する。
    //
    // Returns:
    //  センサーが接続されていることを確認できた場合はtrue。確認できなかった場合はfalse。
    bool init(void) {
        for (int i = 0; i < this->_times; ++i) {
            Wire.beginTransmission(ADDRESS);
            if (Wire.endTransmission() == 0) {
                return true;
            }
            delay(this->_interval);
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
        Wire.requestFrom(ADDRESS, (uint8_t)0x02);

        // 送信された測定値を読み込み
        for (int i = 0; i < this->_times; ++i) {
            while (Wire.available()) {
                uint8_t adc_value_l = Wire.read();
                uint8_t adc_value_h = Wire.read();
                uint16_t adc_value = adc_value_h;
                adc_value <<= 8;
                adc_value |= adc_value_l;
                return (int32_t)adc_value;
            }
            delay(this->_interval);
        }
        return ~(int32_t)0;
    }
};
