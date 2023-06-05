#include "mbed.h"
#include "rc5.h"
#include "protocol.h"
#include <chrono>
#include <map>
#include <list>

namespace IR {
namespace RC5 {
    static constexpr size_t SIGNAL_LEN = 14;
    static constexpr size_t COMMAND_LEN = 6;
    static const std::chrono::microseconds med_pulse = 1500us;
    static const std::chrono::microseconds max_long_pulse = 3000us;
    
    Decoder::Decoder(PinName pin, std::map<uint8_t, Callback<void()>> commands) 
        : BaseDecoder(pin, commands) 
    {
        add_decoder_to_list(this);
    };
    
    bool Decoder::good_startcode() {
        return ((command & (1 << 0)) && (command & (1 << 1)));
    }

    void Decoder::decode_reset() {
        command = 0;
        decoding = true;
        clock.reset();
        cur_bit = 0;
    }

    void Decoder::decode_fall() {
        if (!decoding || clock.elapsed_time() > max_long_pulse) {
            decode_reset();
            BaseDecoder::decode_bit(1);
            return;    
        }

        if (!GET_BIT(command, cur_bit) && clock.elapsed_time() < med_pulse) {
            clock.reset();
            return;
        }

        BaseDecoder::decode_bit(1);
    }

    bool Decoder::final_bit() {
        return cur_bit >= SIGNAL_LEN;
    }

    static unsigned int swap_bits(unsigned int val, int bits_num) {
        for (int i = 0; i < bits_num / 2; i++) {
            val ^= ((val & (1 << i)) >> i) << (bits_num - i - 1);
            val ^= ((val & (1 << (bits_num - i - 1))) >> (bits_num - i - 1)) << i;
            val ^= ((val & (1 << i)) >> i) << (bits_num - i - 1);
        }
        return val;
    }

    bool Decoder::cmp_command(uint16_t cmd) {
        return cmd == swap_bits((command >> 8) & 0x3F, COMMAND_LEN);
    }

    void Decoder::decode_rise() {
        if (!decoding) return;

        if (cur_bit && GET_BIT(command, cur_bit) && clock.elapsed_time() < med_pulse) {
            clock.reset();
            return;
        }

        BaseDecoder::decode_bit(0);
    }
}
}