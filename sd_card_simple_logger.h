#include <SD.h>

class SDCardSimpleLogger {
   private:
    // CSラインピン番号。
    byte _cs;
    // 1ファイルに出力するデータ数。
    uint32_t _number_per_file;
    // ファイルに出力したデータ数。
    uint32_t long _number_of_output;
    // カレントファイル番号。
    uint16_t _current_file_number;
    // カレントファイル名。
    char _current_file_name[40];
    // SDカードに測定値を書き込みするときに使用するバッファ。
    char _buf[32], _temp[8], _hum[8], _als[8], _moist[8];

    // デフォルトコンストラクタ。
    SDCardSimpleLogger(void) {}

    // ファイル名を設定して、そのファイルが存在する場合は削除する。
    bool init_file() {
        sprintf_P(this->_current_file_name, PSTR("measured-%d.csv"),
                  this->_current_file_number);
        if (SD.exists(this->_current_file_name)) {
            SD.remove(this->_current_file_name);
        }
    }

   public:
    // コンストラクタ。
    //
    // Arguments:
    //  cs: SDカードのCS(チップ選択)ラインに接続したピンの番号。
    //  number_per_file: 1ファイルに記録するデータ数。
    SDCardSimpleLogger(byte cs, uint32_t number_per_file) {
        this->_cs = cs;
        this->_number_per_file = number_per_file;
        this->_number_of_output = 0;
        this->_current_file_number = 0;
    }

    // 初期化する。
    //
    // Returns:
    //  初期化に成功した場合はtrue。失敗した場合はfalse。
    bool init(void) {
        if (SD.begin(this->_cs)) {
            return this->init_file();
        }
        return false;
    }

    // 測定値を書き込む。
    //
    // Arguments:
    //  temp: 温度。
    //  hum: 湿度。
    //  als: 環境光照度。
    //  moist: 土壌水分量。
    //
    // Returns:
    //  測定値の書き込みに成功した場合はtrue。失敗した場合はfalse。
    bool write_measured_values(float temp, float hum, float als,
                               int16_t moist) {
        dtostrf(temp, -1, 1, this->_temp);
        dtostrf(hum, -1, 1, this->_hum);
        dtostrf(als, -1, 0, this->_als);
        if (0 <= moist) {
            sprintf_P(this->_moist, PSTR("%d"), moist);
        } else {
            strcpy_P(this->_moist, PSTR("Err."));
        }
        sprintf_P(this->_buf, PSTR("%s,%s,%s,%d"), this->_temp, this->_hum,
                  this->_als, this->_moist);

        File file = SD.open(this->_current_file_name, FILE_WRITE);
        if (file) {
            file.println(this->_buf);
            file.close();
        };
        ++this->_number_of_output;
        if (this->_number_per_file <= this->_number_of_output) {
            ++this->_current_file_number;
            this->init_file();
        }
    }
};
