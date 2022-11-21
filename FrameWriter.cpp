#include "FrameWriter.h"
#include "sstream"
#include "cmath"


FrameWriter::FrameWriter(const std::string &filename)
{
    buffer.reserve(15200);
    std::string output_name= filename;
    output_name+=".output";
    std::string report_name= filename.substr(0, filename.find_last_of('/')+1);
    report_name+="report.txt";
    output_file.open(output_name, std::ios::binary | std::ios::trunc);
    report_file.open(report_name, std::ios::app);
}

void FrameWriter::writeFrame()
{
    output_file.write(buffer.data(), buffer.size());
    buffer.clear();
}

void FrameWriter::writeReport(Report & data , int type)
{
    std::stringstream report;
    time_t now= time(nullptr);
    char* dt = ctime(&now);
    tm *local= localtime(&now);
    switch (type)
    {
        case 0:
            report << local->tm_year+1900 <<"/" << local->tm_mon << "/" << local->tm_hour<<":" << local->tm_min << ":" << local->tm_sec<<" ";
            report << " Исходный файл: "  << data.getInputFile() << std::endl;
            report << local->tm_year+1900 <<"/" << local->tm_mon << "/" << local->tm_hour<<":" << local->tm_min << ":" << local->tm_sec<<" ";
            report << " Размер файла: "  <<  data.getFileSize()/1024/1024 <<" мегабайт" << std::endl;
            report << local->tm_year+1900 <<"/" << local->tm_mon << "/" << local->tm_hour<<":" << local->tm_min << ":" << local->tm_sec<<" ";
            report << " Выходной файл: "  << data.getOutputFile() <<  std::endl;
            report << std::endl;
            break;
        case 1:
            float percent=(data.getBip8FrameErrors()/(data.getFrameCnt()-2))*100;
            report << local->tm_year+1900 <<"/" << local->tm_mon << "/" << local->tm_hour<<":" << local->tm_min << ":" << local->tm_sec<<" ";
            report << " Размер кадра: "  << data.getFrameSize() <<  std::endl;
            report << local->tm_year+1900 <<"/" << local->tm_mon << "/" << local->tm_hour<<":" << local->tm_min << ":" << local->tm_sec<<" ";
            report << " Байт PT: "  << std::hex << data.getPt() <<std::dec <<  std::endl;
            report << local->tm_year+1900 <<"/" << local->tm_mon << "/" << local->tm_hour<<":" << local->tm_min << ":" << local->tm_sec<<" ";
            report << " Обработано кадров: "  << data.getFrameCnt() << ""<< std::endl;
            report << local->tm_year+1900 <<"/" << local->tm_mon << "/" << local->tm_hour<<":" << local->tm_min << ":" << local->tm_sec<<" ";
            report << " Кадров с ошибкой BIP-8:  "  << data.getFrameCnt()-data.getBip8FrameErrors()-2 << ""<< std::endl;
            report << local->tm_year+1900 <<"/" << local->tm_mon << "/" << local->tm_hour<<":" << local->tm_min << ":" << local->tm_sec<<" ";
            report << " Процентное соотношение кадров без ошибки BIP-8 к общему числу кадров: "  << percent << "%"<< std::endl;
            report << local->tm_year+1900 <<"/" << local->tm_mon << "/" << local->tm_hour<<":" << local->tm_min << ":" << local->tm_sec<<" ";
            report << " Время работы: "  << data.getTimer() <<  std::endl;
            report << std::endl<< std::endl;
            break;
    }
    report_file << report.rdbuf();
}
