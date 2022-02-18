#include <LiquidCrystal.h>

class LcdDisplay {
   private:
    // LCDディスプレイポインタ
    LiquidCrystal* _lcd;
    // LCDに描画する文字列を作成するバッファ
    char _buf[24], _temp[16], _hum[16], _als[16], _moist[16];
    // 環境光照度表示フラグ
    bool _is_als;

   public:
    // デフォルトコンストラクタ
    //
    LcdDisplay(void) { this->_is_als = true; }

    // デストラクタ
    ~LcdDisplay(void) { delete this->_lcd; }

    // 初期化する。
    // Arguments:
    //  rs: LCDのRSピンに接続するArduino側のピン番号。
    //  enable: LCDのenableピンに接続するArduino側のピン番号。
    //  d4: LCDのD4ピンに接続するArduino側のピン番号
    //  d5: LCDのD5ピンに接続するArduino側のピン番号
    //  d6: LCDのD6ピンに接続するArduino側のピン番号
    //  d7: LCDのD7ピンに接続するArduino側のピン番号
    void init(int rs, int enable, int d4, int d5, int d6, int d7) {
        this->_lcd = new LiquidCrystal(rs, enable, d4, d5, d6, d7);
        this->_lcd->begin(16, 2);
        this->_lcd->clear();
    }

    // LCDディスプレイの指定した行に文字列を表示する。
    //
    // Arguments:
    //  row: 文字列を表示する行番号。
    //  pBuf: 表示する文字列の先頭ポインタ。
    void print_row(int row, const char* buf) {
        this->_lcd->setCursor(0, row);
        this->_lcd->print(buf);
    }

    // 測定した値をLCDディスプレイに表示する。
    //
    // Arguments:
    //  temp: 温度。
    //  hum: 湿度。
    //  als: 照度。
    //  moist: 土壌水分量。
    void print_measured_values(float temp, float hum, float als,
                               int32_t moist) {
        this->_lcd->clear();
        // 温度と湿度
        dtostrf(temp, -1, 1, this->_temp);
        dtostrf(hum, -1, 1, this->_hum);
        sprintf(this->_buf, "%s[C] %s[%%]", this->_temp, this->_hum);
        this->print_row(0, this->_buf);
        // 照度または土壌水分量
        if (this->_is_als) {
            dtostrf(als, -1, 0, this->_als);
            sprintf(this->_buf, "ALS: %s[lx]", this->_als);
        } else {
            if (0 <= moist) {
                sprintf(this->_moist, "%d", moist);
            } else {
                strcpy(this->_moist, "Err.");
            }
            sprintf(this->_buf, "Soil moist: %s", this->_moist);
        }
        this->print_row(1, this->_buf);
        // 切り替え
        this->_is_als = !this->_is_als;
    }
};
