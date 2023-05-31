#include "mbed.h"
#include "rc5.h"
#include "protocol.h"
#include <map>
#include <list>

namespace IR {
namespace RC5 {
    static constexpr size_t COMMAND_LEN = 14;
    static const std::chrono::microseconds long_pulse = 1778us;
    static const std::chrono::microseconds short_pulse = 889us;
    static const std::chrono::microseconds med_pulse = (long_pulse + short_pulse) / 2;
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
            return;    
        }

        if (GET_BIT(command, cur_bit) && clock.elapsed_time() < med_pulse) {
            clock.reset();
            return;
        }

        BaseDecoder::decode_bit(0);
    }

    bool Decoder::final_bit() {
        return cur_bit >= COMMAND_LEN;
    }

    bool Decoder::cmp_command(uint16_t cmd) {
        return cmd == ((command >> 8) & 0x3F);
    }

    void Decoder::decode_rise() {
        if (!decoding) return;

        if (cur_bit && !GET_BIT(command, cur_bit) && clock.elapsed_time() < med_pulse) {
            clock.reset();
            return;
        }

        BaseDecoder::decode_bit(1);
    }
}
}