#include "mbed.h"
#include <map>
#include <list>
#include <vector>

namespace RC5 {

    #define GET_BIT(X, BIT) ((X & (1 << BIT)) >> BIT)

    class Decoder {
    public:
        int prev_signal_val = 1;
        InterruptIn signal;
        Decoder(PinName pin, std::map<uint8_t, Callback<void()>> commands);
    
        void decode_fall();
        void decode_rise();
    private:
        bool good_startcode();
        void decode_bit(uint8_t bit);
        void decode_reset();

        Timer clock;
        bool decoding = false;

        std::map<uint8_t, Callback<void()>> commands;
        uint16_t command;
        uint8_t cur_bit;
    };

    void on_edge();
    void init(std::vector<Decoder *> decoders);
}