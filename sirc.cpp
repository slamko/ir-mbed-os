#include "sirc.h"

namespace IR {
namespace SIRC {
    static constexpr int COMMAND_LEN = 7;
    static constexpr std::chrono::microseconds start_bit_pulse = 1720us;
    static constexpr std::chrono::microseconds high_pulse = 900us;
    static constexpr std::chrono::microseconds low_pulse = 400us;

    Decoder::Decoder(PinName pin, std::map<uint8_t, Callback<void()>> commands) 
        : BaseDecoder(pin, commands) 
    {
        add_decoder_to_list(this);
    };

    void Decoder::decode_reset() {
        command = 0;
        decoding = true;
        clock.reset();
        clock.start();
        cur_bit = 0;
    }

    bool Decoder::final_bit() {
        return cur_bit >= COMMAND_LEN;
    }

    bool Decoder::cmp_command(uint16_t cmd) {
        return cmd == ((command >> 1) & 0x7F);
    }

    bool Decoder::good_startcode(uint8_t com) {
        return com & (1 << 0);
    }

    void Decoder::decode_fall() {
        if (!decoding || clock.elapsed_time() > start_bit_pulse * 2) {
            decode_reset();
            return;    
        }

        clock.reset();
    }

    void Decoder::decode_rise() {
        if (!decoding) return;
        std::chrono::microseconds mc = clock.elapsed_time();

        if (mc > high_pulse) {
            BaseDecoder::decode_bit(1);
        } else if (mc > low_pulse) {
            BaseDecoder::decode_bit(0);
        }
    }
}
}