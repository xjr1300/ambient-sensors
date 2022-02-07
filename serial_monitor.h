class SerialMonitor {
private:
    // ボーレート
    int _boarate;
    // シリアルモニタに測定値を表示するときに使用するバッファ
    char serial[128], temp[16], hum[16], ill[32];

    // デフォルトコンストラクタ。
    SerialMonitor() { }

public:
    // コンストラクタ
    //
    // Arguments:
    //  boarate: ボーレート。
    SerialMonitor(int boarate) {
        this->_boarate = boarate;
    }
    
    // 初期化する。
    void init() {
        Serial.begin(9600);
        // シリアルモニタが準備できるまでループ
        while (!Serial) {}
        Serial.println("[INFO] Serial monitor was initialized.");
    }

    // 温度、湿度及び照度を出力する。
    //
    // Arguments:
    //  temp: 温度。
    //  hum: 湿度。
    //  ill: 照度。
    void print(float temp, float hum, float ill) {
        dtostrf(temp, -1, 1, this->temp);
        dtostrf(hum, -1, 1, this->hum);
        dtostrf(ill, -1, 0, this->ill);
        sprintf(serial, "Temp: %s[C], Hum: %s[%%], ALS: %s[lx]", this->temp, this->hum, this->ill);
        Serial.println(serial);
    }
};
