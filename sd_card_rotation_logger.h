#include "SdFat.h"

#define SD_FAT_TYPE 3
#define SPI_SPEED SD_SCK_MHZ(50)

#define SD_CARD_CHIP_UNO 10
#define SD_CARD_CHIP_MEGA 53
#define FILE_NAME "measured.csv"
#define ROTATED_NAME_FMT "measured.csv.%d"

class SDCardRotationLogger {
   private:
    // SDカードのチップセレクト端子に接続するピン番号。
    SdCsPin_t _chip;
    // 1ファイルに出力するデータ数。
    uint32_t _number_per_file;
    // ファイルに出力したデータ数。
    uint32_t long _number_of_output;
    // ローテーションファイル数。
    uint8_t _number_of_rotation;
    // カレントファイル番号。
    uint8_t _current_file_number;

    // SDカード。
    SdFs _sd;
    // 測定値を書き込むファイル。
    FsFile _file;

    // SDカードに測定値を書き込みするときに使用するバッファ。
    char _buf[32], _temp[8], _hum[8], _als[8], _moist[8];
    // 測定値を書き込むフィルをローテーションするときに使用するバッファ。
    char _rotated_fname[24];

    // デフォルトコンストラクタ
    SDCardRotationLogger(void) {}

    // 測定値を書き込むファイルをローテーションする。
    bool rotate_file(void) {
        // 退避先のファイル名を生成
        sprintf(this->_rotated_fname, ROTATED_NAME_FMT,
                this->_current_file_number);
        // 退避先のファイルが存在する場合は削除
        if (this->_sd.exists(this->_rotated_fname)) {
            if (!this->_sd.remove(this->_rotated_fname)) {
                return false;
            }
        }
        // 現在のファイル番号を設定
        ++this->_current_file_number;
        this->_current_file_number %= this->_number_of_rotation;
        // 測定値を記録したファイルを退避
        return this->_sd.rename(FILE_NAME, this->_rotated_fname);
    }

   public:
    // コンストラクタ
    //
    // Arguments:
    //  chip: SDカードのチップセレクト端子に接続するピン番号。
    SDCardRotationLogger(SdCsPin_t chip, uint32_t number_per_file,
                         uint16_t number_of_rotation) {
        this->_chip = chip;
        this->_number_per_file = number_per_file;
        this->_number_of_rotation = number_of_rotation;
        this->_current_file_number = 0;
    }

    // 初期化する。
    bool init(void) { this->_sd.begin(this->_chip, SPI_SPEED); }

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
        if (this->_file.open(FILE_NAME, O_WRITE | O_CREAT | O_APPEND)) {
            this->_file.println(this->_buf);
            this->_file.close();
        };
        ++this->_number_of_output;
        if (this->_number_per_file <= this->_number_of_output) {
            this->rotate_file();
        }
    }
};
