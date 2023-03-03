//
// Created by andrey on 1/7/23.
//

#ifndef DESCRAMBLER_FRAMEHANDLERS_H
#define DESCRAMBLER_FRAMEHANDLERS_H

#include "vector"
#include "map"
#include "array"
#include "Frame.h"

class FrameHolder
{
    int series=0;
    int channels=0;
    std::vector<Frame> frames;
    std::array<std::vector<char>, 10> buf;
    int jc[10]{};


public:
    FrameHolder()=default;
    ~FrameHolder()=default;


    void setPayload(const int &num, const std::vector<char> &payload);
    void clear() { for (auto &i: buf) i.clear(); frames.clear(); series=0; }
    void clearFrames() { frames.clear(); }
    void setJC(const int &num, const int &value);
    void emplace_frame(Frame frame){ frames.emplace_back(frame); }
    void remove(int end){ frames.erase(frames.begin(), frames.begin()+end); }

    std::vector<char> getPayload(const int& num) { return buf[num]; }
    Frame get(int num){ return frames[num];}
    int getJC(const int& num) { return jc[num]; }
    int threads() const { return channels+1; }
    int getseries() const {return series;}
    int framesSize() const { return frames.size(); }

};
#endif //DESCRAMBLER_FRAMEHANDLERS_H
