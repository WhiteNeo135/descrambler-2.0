#include <iostream>
#include "FrameReader.h"
#include "FrameWriter.h"
#include "Frame.h"

char sync_bytes[]={char(0xf6), char(0xf6),char(0xf6),char(0x28),char(0x28),char(0x28)};

uint8_t allowed_err=2;

bool check_byte(char byte, int cnt, int & errors)
{
    byte^=sync_bytes[cnt];
    while(byte!=0 && errors<=allowed_err)
    {
        if (byte & 1)
            ++errors;
        byte= byte>>1;

    }
    return allowed_err>errors;
}
int find_sync(const std::unique_ptr<char[]>& buf, size_t begin)
{
    int error=0;
    bool done;
    for (int i=begin; i<(begin+min_frame*6) -5;)
    {
        done=true;
        error=0;
        for (int j=5; j>=0; --j)
        {
            if(!check_byte(buf.get()[i+j], j, error))
            {
                switch (buf.get()[i + 5])
                {
                    case char(0xf6):
                        i+=3;
                        break;
                    case char(0x28):
                        ++i;
                        break;
                    default:
                        i+=6;
                }
                done=false;
                break;
            }
        }
        if(done)
            return i;
    }
    return -1;
}


bool FrameReader::checkSync(char frame[])
{
    int errors=0;
    for (int i=5; i>=0; --i)
        if (!check_byte(frame[i],i,errors))
            return true;
    return false;
}

void shift(){}

bool FrameReader::find_begin_and_sync(size_t pos)
{
    int pointers[4];
    size_t shifted=0;
    bool done;
    while(shifted!=8)
    {
        done=true;
        for (size_t i = 0; i < 4; ++i)
        {
            pointers[i] = find_sync(buf, (i == 0) ? pos : pointers[i - 1] + (min_frame)); //positions of syncs
            if (pointers[i] == -1)
            {
                done=false;
                shift();
                ++shifted;
                break;
            }
        }
        if(done)
            break;
    }
    if(shifted==8)
        return false;


    int length[3];
    for (size_t i=0; i<3; ++i)
    {
        length[i]=pointers[i+1]-pointers[i];
    }

    if (length[0]==length[1] || length[0]==length[2])
    {
        size_of_frame = length[0];
        begin=pointers[0];
    }
    else
    {
        size_of_frame = length[1];
        begin=pointers[1];
    }
    return true;

}

Frame FrameReader::getFrame(Report &report)
{
    if(size_of_frame==0) //"first enter" sync
    {
        readfile(max_frame * 6);
        if(!find_begin_and_sync())
            return {};

        buf.reset(new char[size_of_frame*100]);
    }

    if(proccessed == 0 || begin + size_of_frame > buf_size)  //frame if possible either new buffer
    {
        if (readfile(size_of_frame*100, false, (proccessed) == 0? begin : proccessed)<size_of_frame)
            return {};
        begin=0;
    }

    char frame[size_of_frame];
    memcpy(frame, &buf.get()[begin], size_of_frame);
    begin+=size_of_frame;

    if (checkSync(frame))
        ++sync_err;
    else
        sync_err=0;
    if(sync_err==3)
    {
        report.increaseSyncErr();
        if (!find_begin_and_sync(begin))
            return {};
        proccessed-=(size_of_frame*100)-begin;
        if (readfile(size_of_frame*100, false, proccessed)<size_of_frame)
            return {};
        sync_err=0;
        begin=0;
        memcpy(frame, &buf.get()[begin], size_of_frame);
        begin+=size_of_frame;
    }

    return Frame(frame, size_of_frame);
}

FrameReader::FrameReader(const std::string &filename)
{
     file.open(filename, std::ios::binary | std::ios::ate);
     file_size = file.tellg();
     file.seekg(0);
}

size_t FrameReader::readfile(size_t size, bool stuff, size_t pos)
{
    size_t read = 0;
    buf_size = 0;
    if (stuff)
    {
        file.seekg(pos);
        file.read(buf.get(), size);
    }
    else
    {
        if (proccessed==0)
            proccessed+=begin;
        file.seekg(pos);
        while (buf_size < (size_of_frame * 100) && proccessed<file_size)
        {
            read = file.readsome(reinterpret_cast<char*>(buf.get()+buf_size), (file_size >= proccessed + size) ? ((size_of_frame*100) - buf_size- read) : file_size - proccessed);
            proccessed += read;
            buf_size += read;
        }
    }
    return buf_size;
}
