#include "mbed.h"
#include "rc5.h"
#include <map>
#include <list>

namespace RC5 {
    static constexpr size_t COMMAND_LEN = 14;
    static const microseconds long_pulse = 1778us;
    static const microseconds short_pulse = 889us;
    static const microseconds med_pulse = (long_pulse + short_pulse) / 2;
    static const microseconds max_long_pulse = 3000us;

    Decoder::Decoder(PinName pin, std::map<uint8_t, Callback<void()>> commands) 
        : signal{pin}, commands{commands} 
    {
        signal.mode(PullNone);
        signal.rise(&on_edge);
        signal.fall(&on_edge);
    };
    
    bool Decoder::good_startcode() {
        return ((command & (1 << 0)) && (command & (1 << 1)));
    }

    void Decoder::decode_bit(uint8_t bit) {
        if (bit) {
            command |= (bit << cur_bit);
        }
        cur_bit++;

        if (cur_bit >= COMMAND_LEN) {
            decoding = false;
            cur_bit = 0;
            clock.stop();
            clock.reset();

            if (good_startcode()) {
                
            }
            command = 0;
            return;
        }

        clock.reset();
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

        decode_bit(0);
    }

    void Decoder::decode_rise() {
        if (!decoding) return;

        if (cur_bit && !GET_BIT(command, cur_bit) && clock.elapsed_time() < med_pulse) {
            clock.reset();
            return;
        }

        decode_bit(1);
    }

    static std::list<Decoder> decoders_list;

    void init(std::list<Decoder> decoders) {
        decoders_list = decoders;    
    }

    void on_edge() {
        for (auto &dec : decoders_list) {
            if (dec.signal.read() != dec.prev_signal_val) {
                dec.prev_signal_val = !dec.prev_signal_val;
            }
            if (dec.signal.read()) {
                dec.decode_rise();
            } else {
                dec.decode_fall();
            }
        }
    }
}