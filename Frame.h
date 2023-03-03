#ifndef FRAME_H
#define FRAME_H

#include <memory>
#include <algorithm>
#include <array>
#include <cstring>
#include <vector>
#include <map>
#include <fstream>
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
protected:
    std::vector<char> buffer;
    size_t frame_size;
    int pm = 190;
    bool init=false;
    static size_t serial;
    static bool gmp_cnt;

    static char BIP_8[2];

public:
    explicit Frame(char frame[], int size);
    Frame()=default;
    char operator[](size_t pos) const { return buffer[pos]; }
    explicit operator bool() const{ return init; }

    void descramble(Descrambler& descrambler);

    char getMFAS() const { return buffer[6]; }
    char getPayloadByte(size_t pos);
    size_t getBIP_8() const { return BIP_8[serial%2]; }
    size_t getSerial() { return serial; }
    size_t getPm() const { return pm; }
    size_t getSize() const { return buffer.size(); }
    size_t getPT() const { return buffer[getSize() / 4 * 3 + 14]; }
    size_t getGMP_cnt() const { return buffer[getSize() / 4 * 3 + 15]; }
    char* getiter(int pos){return &buffer[pos];};
    void setPayload();
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

    //bool checkGMP_cnt() { return gmp_cnt; }
    void setPm(int num) { pm = num; }

};


#endif //FRAME_H