#ifndef FRAME_H
#define FRAME_H

#include <memory>
#include <algorithm>
#include <array>
#include <cstring>
#include <vector>
#include "constants.h"

class Descrambler
{
private:
    char sequence[22000];
    char initial[16]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    bool init=false;

public:
    Descrambler()=delete;
    Descrambler(size_t size);
    char operator[](size_t pos){ return sequence[pos]; }

};


class Frame
{
    std::unique_ptr<char[]> buffer;
    size_t jc;
    size_t frame_size;
    static constexpr int pm = 190;
    bool init;
    static size_t serial;

    static char BIP_8[2];

public:
    Frame()=default;
    explicit Frame(char frame[], int size);

    char operator[](size_t pos) const { return buffer[pos]; }
    explicit operator bool() const{ return init; }

    void descramble(Descrambler descrambler);

    void setJC();
    void setBIP_8()
    {
        BIP_8[serial%2]=0;
        for (int i=0; i<4; ++i)
        {
            for (int j=14; j<3824; ++j)
            {
                BIP_8[serial%2]^=buffer[(i*frame_size/4)+j];
            }
        }
    }
    char getMFAS() const { return buffer[6]; }
    size_t getJC() const { return jc; }
    size_t getPm() const { return pm; }
    size_t getSize() const { return frame_size; }
    size_t getSerial() const { return serial; }
    size_t getBIP_8() const { return BIP_8[serial%2]; }
    char getPayloadByte(size_t pos);

};

#endif //FRAME_H