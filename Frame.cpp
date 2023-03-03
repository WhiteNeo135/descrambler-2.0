#include "Frame.h"
#include <memory>

size_t Frame::serial =0;
bool Frame::gmp_cnt =false;
char Frame::BIP_8[2] ={0,0};



Frame::Frame(char *frame, int size)
{
//    buffer.reset(new char[size]);
    buffer.resize(size);
    memcpy(buffer.data(), frame, size);
    frame_size=size;
    init = true;
    ++serial;
}

char Frame::getPayloadByte(size_t pos)
{
    size_t a = 16 + ((pos / 3800) * getSize() / 4) + pos%3800;
    return buffer[a];
}

void Frame::descramble(Descrambler& descrambler)
{
    for (int j = 6; j < getSize(); ++j)
        buffer[j] ^= descrambler[j - 6];
}

void Frame::setPayload()
{
    std::vector<char> payload;
    for (int i=0; i<4; ++i)
        payload.insert(payload.end(), &buffer[16+(i*frame_size/4)], &buffer[3816+(i*frame_size/4)]);
    buffer=payload;
    frame_size=payload.size();
}

char updateScrambler(char* initial, char bit)
{
    char res = initial[15];
    for (size_t i = 15; i > 0; --i)
        initial[i] = initial[i-1];

    initial[0] = bit;
    return res;

}

Descrambler::Descrambler(size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        char byte = 0;
        for (size_t j = 0; j < 8; ++j)
        {
            char bit = initial[0] ^ initial[2] ^ initial[11] ^ initial[15];
            byte= (byte << 1) + updateScrambler(initial, bit);
        }
        sequence[i] = byte;
    }
}
