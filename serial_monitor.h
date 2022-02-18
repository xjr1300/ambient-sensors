class SerialMonitor {
   private:
    // ボーレート
    int _boarate;
    // シリアルモニタに測定値を表示するときに使用するバッファ
    char _serial[128], _temp[16], _hum[16], _ill[32], _moist[32];

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
        Serial.println("[INFO] Serial monitor was initialized.");
    }

    // 温度、湿度及び照度を出力する。
    //
    // Arguments:
    //  temp: 温度。
    //  hum: 湿度。
    //  ill: 照度。
    //  moist: 土壌水分量。
    void print_measured_values(float temp, float hum, float ill,
                               int32_t moist) {
        dtostrf(temp, -1, 1, this->_temp);
        dtostrf(hum, -1, 1, this->_hum);
        dtostrf(ill, -1, 0, this->_ill);
        if (0 <= moist) {
            sprintf(this->_moist, "%d", moist);
        } else {
            strcpy(this->_moist, "Err.");
        }
        sprintf(this->_serial,
                "Temp: %s[C], Hum: %s[%%], ALS: %s[lx], Moist: %d", this->_temp,
                this->_hum, this->_ill, this->_moist);
        Serial.println(_serial);
    }
};
