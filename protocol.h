#ifndef IR_H
#define IR_H

#include "mbed.h"
#include <map>

namespace IR {
    class BaseDecoder {
    private:
        friend void on_edge();
        friend void decode_bit(BaseDecoder dec, uint8_t bit);

    protected:
        BaseDecoder(PinName pin, std::map<uint8_t, Callback<void()>> commands);

        virtual void decode_fall() = 0;
        virtual void decode_rise() = 0;

        InterruptIn signal;
        Timer clock;

        int prev_signal_val = 1;
        bool decoding = false;
        uint16_t command;
        uint8_t cur_bit;

        virtual bool final_bit() = 0;
        virtual bool good_startcode() = 0;
        virtual void decode_reset() = 0;
        void decode_bit(uint8_t bit);

        std::map<uint8_t, Callback<void()>> commands;
    };

    void add_decoder_to_list(BaseDecoder *dec);

    void on_edge();
}
#endif