class SerialMonitor {
   private:
    // ボーレート
    int _boarate;
    // シリアルモニタに測定値を表示するときに使用するバッファ
    char _serial[128], _temp[16], _hum[16], _als[32], _moist[32];

    // デフォルトコンストラクタ。
    SerialMonitor(void) {}

   public:
    // コンストラクタ
    //
    // Arguments:
    //  boarate: ボーレート。
    SerialMonitor(int boarate) { this->_boarate = boarate; }

    // 初期化する。
    void init(void) {
        Serial.begin(9600);
        // シリアルモニタが準備できるまでループ
        while (!Serial) {
            // Do not anything.
        }
        Serial.println(F("[INFO] Serial was initialized."));
    }

    // 測定値をシリアルモニタに出力する。
    //
    // Arguments:
    //  temp: 温度。
    //  hum: 湿度。
    //  als: 環境光照度。
    //  moist: 土壌水分量。
    void print_measured_values(float temp, float hum, float als,
                               int16_t moist) {
        dtostrf(temp, -1, 1, this->_temp);
        dtostrf(hum, -1, 1, this->_hum);
        dtostrf(als, -1, 0, this->_als);
        if (0 <= moist) {
            sprintf_P(this->_moist, PSTR("%d"), moist);
        } else {
            strcpy_P(this->_moist, PSTR("Err."));
        }
        sprintf_P(this->_serial,
                  PSTR("Temp: %s[C], Hum: %s[%%], ALS: %s[lx], Moist: %d"),
                  this->_temp, this->_hum, this->_als, this->_moist);
        Serial.println(_serial);
    }
};
