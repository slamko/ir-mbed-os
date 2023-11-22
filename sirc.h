#ifndef SIRC_H
#define SIRC_H

#include "mbed.h"
#include <map>
#include <list>
#include <vector>
#include "protocol.h"

namespace IR {
namespace SIRC {
    class Decoder : protected IR::BaseDecoder {
    public:
        Decoder(PinName pin, std::map<uint8_t, Callback<void()>> commands);

    protected:
        void decode_bit(uint8_t bit);

        bool final_bit() override;
        bool good_startcode(uint8_t) override;
        void decode_reset() override;
        void decode_fall() override;
        void decode_rise() override;
        bool cmp_command(uint16_t cmd) override;
    };
}
}
#endif