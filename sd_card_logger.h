#include "SdFat.h"

#define SPI_SPEED SD_SCK_MHZ(50)

const int SD_CARD_CHIP_UNO = 10;
const int SD_CARD_CHIP_MEGA = 53;
const char* FILE_NAME = "measured_values.log";
const char* ROTATED_NAME_FMT = "measured_values.log.%d";

class SDCardLogger {
   private:
    // SDカードのチップセレクト端子に接続するピン番号。
    SdCsPin_t _chip;
    // 1ファイルに出力するデータ数。
    unsigned long _number_per_file;
    // ファイルに出力したデータ数。
    unsigned long _number_of_output;
    // ローテーションファイル数。
    unsigned int _number_of_rotation;
    // カレントファイル番号。
    unsigned int _current_file_number;

    // SDカード。
    SdFs _sd;
    // 測定値を書き込むファイル。
    FsFile _file;

    // SDカードに測定値を書き込みするときに使用するバッファ。
    char _buf[32], _temp[8], _hum[8], _als[8], _moist[8];
    // 測定値を書き込むフィルをローテーションするときに使用するバッファ。
    char _rotated_fname[24];

    // デフォルトコンストラクタ
    SDCardLogger(void) {}

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
    SDCardLogger(SdCsPin_t chip, unsigned long number_per_file,
                 unsigned int number_of_rotation) {
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
                               int32_t moist) {
        dtostrf(temp, -1, 1, this->_temp);
        dtostrf(hum, -1, 1, this->_hum);
        dtostrf(als, -1, 0, this->_als);
        if (0 <= moist) {
            sprintf(this->_moist, "%d", moist);
        } else {
            strcpy(this->_moist, "Err.");
        }
        sprintf(this->_buf, "%s,%s,%s,%d", this->_temp, this->_hum, this->_als,
                this->_moist);
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
