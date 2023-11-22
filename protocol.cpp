#include "mbed.h"
#include "protocol.h"
#include <stdint.h>
#include <vector>

namespace IR {
    void BaseDecoder::decode_bit(uint8_t bit) {
        if (bit) {
            command |= (bit << cur_bit);
        }
        cur_bit++;

        if (final_bit()) {
            uint8_t com = command;
            decoding = false;
            cur_bit = 0;
            
            clock.stop();
            clock.reset();

            if (good_startcode(com)) {
                for (auto & cmd : commands) {
                    if (cmp_command(cmd.first)) {
                        cmd.second();
                    }
                }
            }

            command = 0;

            return;
        }

        clock.reset();
    }

    BaseDecoder::BaseDecoder(PinName pin, std::map<uint8_t, Callback<void()>> commands):
        signal{pin}, commands{commands} 
    {
        signal.mode(PullNone);
        signal.rise(&on_edge);
        signal.fall(&on_edge);
    }

    static std::vector<BaseDecoder *> decoders_list;

    void add_decoder_to_list(BaseDecoder *dec) {
        decoders_list.push_back(dec);
    }

    void on_edge() {
        for (auto dec : decoders_list) {
            if (dec->signal.read() != dec->prev_signal_val) {
                dec->prev_signal_val = !dec->prev_signal_val;
            }
            if (dec->signal.read()) {
                dec->decode_rise();
            } else {
                dec->decode_fall();
            }
        }
    }
}