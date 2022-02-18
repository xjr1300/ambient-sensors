#include "SparkFun_Si7021_Breakout_Library.h"

class TempHumSensor {
   private:
    // Si7021スレーブアドレス。
    char _address;
    // Si7021センサーインスタンス
    Weather _sensor;

    // デフォルトコンストラクタ
    TempHumSensor(void) {}

    // 華氏の温度を摂氏に変換する。
    //
    // Arguments:
    //  f: 華氏。
    //
    // Returns:
    //  摂氏。
    float f_to_c(float f) { return (f - 32.0) / 1.8; }

   public:
    // コンストラクタ
    //
    // Arguments:
    //  address: Si7021スレーブアドレス。
    TempHumSensor(char address) { this->_address = address; }

    // 初期化する。
    //
    // Returns:
    //  初期化に成功した場合はtrue。初期化に失敗した場合はfalse。
    bool init() {
        // Si7021をリセット
        Wire.beginTransmission(this->_address);
        Wire.write(0xFE);  // リセットコマンドアドレス
        Wire.endTransmission();
        delay(15);
        // Si7021センサーの存在を確認
        return this->_sensor.begin();
    }

    // 温度を測定する。
    //
    // Returns:
    //  温度。単位は度。
    float measure_temp(void) { return this->f_to_c(this->_sensor.getTempF()); }

    // 湿度を測定する。
    //
    // Returns:
    //  湿度。単位は%。
    float measure_hum(void) { return this->_sensor.getRH(); }
};
