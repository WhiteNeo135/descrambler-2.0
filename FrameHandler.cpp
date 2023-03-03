// Created by andrey on 27.02.23.
//

#include "FrameHandler.h"


int setJc(const Frame& frame)
{
    int jc;
    jc = static_cast<unsigned char>(frame[JC_POS]);
    jc <<= 8;
    jc |= static_cast<unsigned char>(frame[JC_POS + frame.getSize() / 4]);

    int indicator = jc & 0b11;
    jc >>= 2;
    switch (indicator)
    {
        case 0b10:
        {
            jc ^= inverted_nums[0];
            return jc;
        }
        case 0b01:
        {
            jc ^= inverted_nums[1];
            return jc;
        }
        default:
        {
            return jc;
        }
    }
}

int Frame_handler_pt_7::get_payload(FrameHolder &holder)
{
    for (int i = 0; i < holder.framesSize(); ++i)
    {
        Frame frame = holder.get(i);
        std::vector<char> payload;
        size_t jc = setJc(frame);

        frame.setPayload();

        for (int j = 0; j < frame.getPm(); ++j)
        {
            if (((j + 1) * jc) % frame.getPm() < jc)
            {
                payload.insert(payload.end(), frame.getiter(j * 80), frame.getiter((j * 80) + 80));
            }
        }
    }
}

void getJC(FrameHolder &holder, int pos)
{
    int start=pos;
    int finish=pos+80;
    std::vector<int> jc_8;
    std::vector<int> jc_10;
    std::vector<int> jc_check;

    for (int i=start; i<finish; ++i)
    {
        if (holder.get(i).getGMP_cnt() % 0x08== 0x07)
        {
            jc_8.emplace_back(setJc( holder.get(i)));
        }
    }
}

std::array<std::vector<char>, 10> getOPU(FrameHolder &holder, int pos)
{
    std::array<std::vector<char>, 10> handle;
    std::vector<char> buffer;
    Frame frame;
    for (int k=pos; k<pos+80; ++k)
    {
        frame=holder.get(k);
        frame.setPayload();
        for (int i = 0; i < 10; ++i)
        {
            buffer.clear();
            for (int j = 0; j < 15200 / 80; ++j)
            {
                buffer.insert(buffer.end(), frame.getiter((j * 80) + (i * 8)), frame.getiter((j * 80) + (i * 8) + 8));
            }
            handle[i].insert(handle[i].end(), buffer.begin(), buffer.end());
        }
    }
    return handle;
}

void Frame_handler_pt_21::get_payload(FrameHolder &holder)
{
    if (holder.framesSize() == 0 || holder.framesSize() % 80 != 0)
        return;

    std::array<std::vector<char>, 10> handle;
    for (int series = 0; series < holder.framesSize() / 80; ++series)
    {
        getJC(holder, series*80);
        handle= getOPU(holder, series*80);
        
        int jc;
        std::vector<char> res;
        for (int k = 0; k < 10; ++k)
        {

            jc = holder.getJC(k);

            for (int i = 0; i < 15200; ++i)
            {
                if (((i + 1) * jc) % 15200 < jc)
                    res.insert(res.end(), &handle[k][i * 8], &handle[k][i * 8 + 8]);
            }
            holder.setPayload(k, res);
            handle[k].clear();
            res.clear();
        }
    }
    holder.clearFrames();
}