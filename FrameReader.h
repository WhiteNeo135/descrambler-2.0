#ifndef FRAME_READER_H
#define FRAME_READER_H

#include <memory>
#include <fstream>
#include "constants.h"
#include "Frame.h"
#include "FrameWriter.h"
#include "FrameHolders.h"

class FrameReader
{
private:
    bool init=false;
    size_t begin = 0 ;
    int size_of_frame = 0;
    size_t file_size = 0;
    size_t buf_size = 0;
    size_t proccessed = 0;
    size_t sync_err = 0;
    std::ifstream file;
    char frame[max_frame];
    std::unique_ptr<char[]> buf{new char[max_frame*10]};

    static bool checkSync(char _frame[]);
    size_t readfile(size_t size, bool stuff=true, size_t pos=0);
    bool find_begin_and_sync(size_t pos=0);

public:
    FrameReader() = delete;
    explicit FrameReader(const std::string& filename);
    bool is_init() const {return init;}

    ~FrameReader() = default;

    Frame getFrame(Report &report);
    long long getSize() const{ return file_size; };
};

#endif // FRAME_READER_H