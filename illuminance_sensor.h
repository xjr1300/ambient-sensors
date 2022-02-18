#include "RPR-0521RS.h"

class IlluminanceSensor {
   private:
    // RPR0521RS
    RPR0521RS _sensor;
    unsigned short _ps;

   public:
    // デフォルトコンストラクタ
    IlluminanceSensor(void) {}

    // 初期化する。
    //
    // Returns:
    //  初期化に成功した場合はtrue。初期化に失敗した場合はfalse。
    bool init() { return this->_sensor.init(); }

    // 照度を測定する。
    //
    // Arguments:
    //  ill: 測定した照度。単位はlux。
    //
    // Returns:
    //  測定に成功した場合はtrue。測定に失敗した場合はfalse。
    bool measure(float* ill) {
        return this->_sensor.get_psalsval(&this->_ps, ill) == 0;
    }
};
