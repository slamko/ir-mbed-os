#include "mbed.h"
#include <map>

namespace RC5 {

    static constexpr size_t COMMAND_LEN = 14;
    static const microseconds long_pulse = 1778us;
    static const microseconds short_pulse = 889us;
    static const microseconds med_pulse = (long_pulse + short_pulse) / 2;
    static const microseconds max_long_pulse = 3000us;

    #define GET_BIT(X, BIT) ((X & (1 << BIT)) >> BIT)

    class Decoder {
    public:
        bool prev_signal_val;
        InterruptIn signal;
        Decoder(PinName pin, std::map<uint8_t, Callback<void()>> commands);
    
    private:
        bool good_startcode();
        void decode_bit(uint8_t bit);
        void decode_reset();
        void decode_fall();
        void decode_rise();

        Timer clock;
        bool decoding = false;

        std::map<uint8_t, Callback<void()>> commands;
        uint16_t command;
        uint8_t cur_bit;
    };

    Decoder::Decoder(PinName pin, std::map<uint8_t, Callback<void()>> commands) 
        : signal{pin}, commands{commands} 
    {
        signal.mode(PullNone);
        signal.rise(decode_rise);
        signal.fall(&decode_fall);
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

    class RC5 {
        
    }
    static Decoder decoders_list;

    void init(Decoder *decoders) {
        decoders_list = decoders;    
    }

    void on_fall() {
        for (auto &dec : decoders_list) {

        }
    }
}