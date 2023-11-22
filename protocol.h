#ifndef IR_H
#define IR_H

#include "mbed.h"
#include <map>

namespace IR {
    #define GET_BIT(X, BIT) ((X & (1 << BIT)) >> BIT)

    class BaseDecoder {
    private:
        friend void on_edge();
        friend void decode_bit(BaseDecoder dec, uint8_t bit);

    protected:
        BaseDecoder(PinName pin, std::map<uint8_t, Callback<void()>> commands);

        InterruptIn signal;
        Timer clock;

        int prev_signal_val = 1;
        bool decoding = false;
        uint16_t command = 0;
        uint8_t cur_bit = 0;

        void decode_bit(uint8_t bit);

        virtual void decode_fall() = 0;
        virtual void decode_rise() = 0;
        virtual bool cmp_command(uint16_t cmd) = 0;
        virtual bool final_bit() = 0;
        virtual bool good_startcode(uint8_t) = 0;
        virtual void decode_reset() = 0;

        std::map<uint8_t, Callback<void()>> commands;
    };

    void add_decoder_to_list(BaseDecoder *dec);

    void on_edge();
}
#endif