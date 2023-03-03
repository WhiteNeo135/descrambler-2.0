#ifndef FRAME_WRITER_H
#define FRAME_WRITER_H

#include "Frame.h"
#include "FrameHolders.h"
#include <vector>
#include <chrono>

class Report
{
protected:
    bool setted=false;
    size_t pt_temp[3]= {0,0,0};
    std::chrono::system_clock::time_point time[2];
    std::string input_file;
    std::string output_file;
    std::string report_file;

    std::map<std::string, long > props;

public:
    Report()=default;
    explicit Report(const std::string& filename);
    ~Report()=default;

    void frameHandle(Frame& frame);
    void addProp(const std::string& key, long value);
    void increaseProp(std::string key);
    long getProp(const std::string& key);
    void setPt(size_t num);
    auto getTimer() const{ return std::chrono::duration_cast<std::chrono::seconds>(time[1]-time[0]).count(); }
    void writeReport( int type);
};

class FrameWriter
{
protected:
    const size_t max_size=100;
    size_t writes=0;
    std::string file_root;
    std::string file_type;
    std::string file_name;
    std::ofstream output_file;
    std::vector<std::ofstream> files;
//    std::map<std::string, std::vector<char> > buf;
    std::vector< std::pair<  std::string, std::vector<char>> > buf;
public:
    explicit FrameWriter(const std::string& _filename);
    void pushPayload(FrameHolder& holder);
    void writeFrame();
    std::vector<std::string> getFilenames();

};

#endif