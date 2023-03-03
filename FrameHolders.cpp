#include "FrameHolders.h"

void FrameHolder::setPayload(const int &num, const std::vector<char> &payload)
{
    buf[num].insert(buf[num].end(), payload.begin(), payload.end());
    if (channels < num)
    {
        channels = num;
        series = 1;
    }
    else if (series == num)
    {
        ++series;
    }
}

void FrameHolder::setJC(const int &num, const int &value)
{
    jc[num]=value;
}
