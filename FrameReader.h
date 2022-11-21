#ifndef FRAME_READER_H
#define FRAME_READER_H

#include <memory>
#include <fstream>
#include "constants.h"
#include "Frame.h"

class FrameReader
{
private:
    int begin = 0 ;
    int size_of_frame = 0 ;
    size_t file_size = 0 ;
    size_t buf_size = 0 ;
    size_t proccessed = 0 ;
    size_t sync_err = 0 ;
    std::ifstream file;
    std::unique_ptr<char[]> buf{new char[max_frame*6]};

    bool checkSync(char frame[]);
    size_t readfile(size_t size, bool stuff=true, size_t pos=0);
    bool find_begin_and_sync();

public:
    FrameReader() = delete;
    explicit FrameReader(const std::string& filename);

    ~FrameReader() = default;

    Frame getFrame();
    size_t getSize(){ return file_size; };
};

#endif // FRAME_READER_H