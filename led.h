class SwitchingLed {
   private:
    // ピン番号。
    byte _pin;
    // LEDを点灯するときの最大値。
    byte _max_value;
    // LEDを点灯または消灯するときのステップ値。
    byte _step;
    // LEDを点灯または消灯するときの遅延時間(ミリ秒)。
    uint16_t _delay;
    // LED電圧増加フラグ。
    bool _increase;

    // デフォルトコンストラクタ
    SwitchingLed(void) {}

    // LEDを点灯または消灯するときの値を取得する。
    //
    // Arguments:
    //  start_value: 初期値。
    //  end_value: 終了値。
    //  step: ステップ値。
    void switching_values(byte* start_value, byte* end_value, byte* step) {
        if (this->_increase) {
            *start_value = 0;
            *end_value = this->_max_value;
            *step = this->_step;
        } else {
            *start_value = this->_max_value;
            *end_value = 0;
            *step = -this->_step;
        }
    }

    bool is_break_loop(byte current_value, byte end_value) {
        if (this->_increase && end_value < current_value) {
            return true;
        } else if (!this->_increase && current_value < end_value) {
            return true;
        }
        return false;
    }

   public:
    // コンストラクタ
    //
    // Arguments:
    //  pin: ピン番号。
    //  max_value: LEDを点灯するときの最大値。
    //  step: LEDを点灯または消灯するときのステップ数。
    //  delay: LEDを点灯または消灯するときの遅延時間(ミリ秒)。
    SwitchingLed(byte pin, byte max_value, byte step, uint16_t delay) {
        this->_pin = pin;
        this->_max_value = max_value;
        this->_step = step;
        this->_delay = delay;
        this->_increase = true;
    }

    /// 初期化する。
    void init(void) {
        pinMode(this->_pin, OUTPUT);
        analogWrite(this->_pin, 0);
    }

    /// 点灯または消灯する。
    void on_off(void) {
        byte current_value, end_value, step;
        this->switching_values(&current_value, &end_value, &step);
        while (!this->is_break_loop(current_value, end_value)) {
            analogWrite(this->_pin, current_value);
            current_value += step;
            delay(this->_delay);
        }
        analogWrite(this->_pin, end_value);
        this->_increase = !this->_increase;
    }
};
