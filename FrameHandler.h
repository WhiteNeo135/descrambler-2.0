//
// Created by andrey on 27.02.23.
//

#ifndef DESCRAMBLER_FRAMEHANDLER_H
#define DESCRAMBLER_FRAMEHANDLER_H

#include "Frame.h"
#include "FrameHolders.h"

class Frame_handler_pt_7
{
protected:
    std::vector<char> buf;

public:
    int get_payload(FrameHolder &holder);
    void debug(FrameHolder& holder, Frame& frame);
};

class Frame_handler_pt_21
{
    std::vector<char> buffer;

public:
    Frame_handler_pt_21(){ buffer.reserve(15200); }
    ~Frame_handler_pt_21()=default;

    void get_payload(FrameHolder &holder);
};

#endif //DESCRAMBLER_FRAMEHANDLER_H
