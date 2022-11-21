#ifndef FRAME_WRITER_H
#define FRAME_WRITER_H

#include <string>
#include <fstream>
#include <utility>
#include <vector>
#include <ctime>
#include <chrono>

class Report
{
    size_t pt=0;
    size_t frame_cnt=0;
    size_t frame_size=0;
    float file_size=0;
    size_t bip_8_cnt=0;
    std::chrono::time_point<std::chrono::_V2::system_clock> time[2];
    std::string input_file;
    std::string output_file;

public:
    Report()=delete;
    Report(std::string filename): input_file(std::move(filename))
    {
        output_file= input_file+".output";
    }
    ~Report()=default;

    void setFileSize(float size) { file_size = size; }
    void setFrameSize(size_t num){ frame_size=num; }
    void setPt(size_t num){ pt=num; }
    void startTimer(){ time[0]= std::chrono::system_clock::now(); }
    void stopTimer(){ time[1]= std::chrono::system_clock::now(); }
    void increaseFrameCnt(){ ++frame_cnt; }

    void increaseBip8Cnt(){ ++bip_8_cnt; }
    size_t getBip8FrameErrors() const{ return bip_8_cnt; }
    size_t getPt() const{ return pt; }
    size_t getFileSize() const{ return file_size; }
    size_t getFrameCnt() const{ return frame_cnt; }
    size_t getFrameSize() const{ return frame_size; }
    auto getTimer() const{ return std::chrono::duration_cast<std::chrono::seconds>(time[1]-time[0]); }
    std::string getInputFile() { return input_file; }
    std::string getOutputFile() { return output_file; }


};

class FrameWriter
{
    std::ofstream output_file;
    std::ofstream report_file;
    std::vector<char> buffer;
public:
    FrameWriter() = delete;
    ~FrameWriter() = default;
    explicit FrameWriter(const std::string& filename);
    void setPayloadByte(char byte){ buffer.emplace_back(byte); }
    void writeFrame();

    void writeReport(Report & data, int type);
};

#endif