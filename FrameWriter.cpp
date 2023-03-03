#include "FrameWriter.h"
#include "sstream"
#include "iostream"
#include "cmath"


FrameWriter::FrameWriter(const std::string &_filename)
{
    std::ofstream debug("debug.txt");
//    file_root=_filename.substr(0, _filename.find_last_of('/') + 1);
    file_type= _filename.substr(_filename.find_last_of('.'));
    //file_name=_filename.substr(_filename.find_last_of('/') + 1, _filename.find_last_of('.'));
    file_name=_filename.substr(0,_filename.find('.'));
    file_name+=" ";
    std::string wrap="\n";
    debug.write(file_name.c_str(), file_name.size());
    debug.write(std::to_string(file_name.size()).c_str(), std::to_string(file_name.size()).size());
    debug.write(wrap.c_str(), wrap.size());
    debug.write(file_type.c_str(), file_type.size());
    debug.write(wrap.c_str(), wrap.size());

//    file_name=file_name.substr(0, file_name.size()-file_type.size());
    debug.write(file_name.c_str(), file_name.size());
    debug.write(std::to_string(file_name.size()).c_str(), std::to_string(file_name.size()).size());
    debug.write(wrap.c_str(), wrap.size());

    file_name+="_data_ch";
    buf.resize(10);

    for (int i=1; i<=10; ++i) {
        buf[i - 1].first = file_root + file_name + ((i < 10) ? "0" + std::to_string(i) : std::to_string(i)) + file_type;
        debug.write(buf[i-1].first.c_str(), buf[i-1].first.size());
        debug.write(wrap.c_str(), wrap.size());

    }

}

void FrameWriter::writeFrame()
{
    std::ofstream file;
    for (int i=0; i<10; ++i)
    {
        if (!buf[i].second.empty())
        {
            file.open(buf[i].first, std::ios::binary | std::ios::app);
            file.write(buf[i].second.data(), buf[i].second.size());
            file.close();
            buf[i].second.clear();
        }
    }
}
void FrameWriter::pushPayload(FrameHolder& holder)
{
    for (int i = 0; i < holder.threads(); ++i)
    {
            auto payload=holder.getPayload(i);
            buf[i].second.insert(buf[i].second.end(), payload.begin(), payload.end());

    }
    holder.clear();
    ++writes;
    if (writes>=max_size)
    {
        writeFrame();
        writes=0;
    }
}

std::vector<std::string> FrameWriter::getFilenames()
{
    std::vector<std::string> res;
    for (int i=0; i<10; ++i)
        res.emplace_back(buf[i].first);

    return res;
}

std::string getTime()
{
    time_t now= time(nullptr);
    tm *local= localtime(&now);
    std::stringstream ss;
    ss<< local->tm_year+1900 <<"/" << local->tm_mon << "/" << local->tm_hour<<":" << local->tm_min << ":" << local->tm_sec<<" ";
    return ss.str();
}

void Report::writeReport( int type)
{
    time[1]=std::chrono::system_clock::now();
    std::stringstream report;
    std::string time=getTime().append(" ");
    switch (type)
    {
        case 0:
        {
            report << time << " Исходный файл: " << input_file << std::endl;
            report << time << " Размер файла: " << getProp("file_size")/1024/1024 << " мегабайт" << std::endl;
            report << time << " Выходной файл: " << output_file << std::endl;
            report << std::endl;
            break;
        }
        case 1:
        {
            float percent = ( float((getProp("BIP-8")+2 )) / getProp("frame_cnt") );
            percent *= 100;
            report << time << " Синхронизация найдена " << std::endl;
            report << time << " Размер кадра: " << getProp("frame_size") << std::endl;
            report << time << " Байт PT: " << std::hex<< getProp("PT") << std::dec << std::endl;
            report << time << " Обработано кадров: " << getProp("frame_cnt") << "" << std::endl;
            report << time << " Обнаружено срывов синхронизации: " << getProp("miss_sync") << "" << std::endl;
            report << time << " Кадров с ошибкой BIP-8:  " << (getProp("frame_cnt")) - (getProp("BIP-8"))-2 << ""
                   << std::endl;
            report << time << " Процентное соотношение кадров без ошибки BIP-8 к общему числу кадров: " << percent
                   << "%" << std::endl;
            report << time << " Время работы: " << getTimer() << "с " << std::endl;
            report << std::endl << std::endl;
            break;
        }
        case 2:
        {
            report << time << " Исходный файл: " << input_file << std::endl;
            report << time << " Размер файла: " << getProp("file_size")/1024/1024 << " мегабайт" << std::endl;
            report << std::endl;
            break;
        }
        case -1:
        {
            report << time << " Синхронизация не найдена" << std::endl;
            break;
        }
    }
    switch (type)
    {
        case 0:
            break;
        case 1:
            break;
        case 3:
          //  report << time << " Время работы: "  << data.getTimer() <<"с "<<  std::endl;
            break;
        default:
            break;
    }
    std::ofstream out_report(report_file, std::ios::app);
    out_report << report.rdbuf();
}

void Report::setPt(size_t num)
{
    long pt;
    if (getProp("PT_cnt")<3)
    {
        increaseProp("PT_cnt");
        pt_temp[getProp("PT_cnt")-1] = num;
        return;
    }
    if (getProp("PT_cnt")==3)
    {
        if (pt_temp[0] == pt_temp[1] && pt_temp[0] == pt_temp[2])
            addProp("PT", pt_temp[0]);
        else if (pt_temp[1] == pt_temp[0] && pt_temp[1] == pt_temp[2])
            addProp("PT", pt_temp[1]);
        else if (pt_temp[2] == pt_temp[0] && pt_temp[2] == pt_temp[1])
            addProp("PT", pt_temp[2]);
        else
        {
            addProp("PT_cnt", 0);
            addProp("PT", pt);
        }
    }
}

long Report::getProp(const std::string& key)
{
    if (props.find(key)!=props.end())
        return props.at(key);
    else
        return -1;
}

Report::Report(const std::string& filename)
{
    output_file= filename;
    std::string type= filename.substr(filename.find('.'));
    output_file=output_file.substr(0,filename.find('.'));
    output_file+="_data_ch01";
    output_file+=type;

    input_file=filename;
    report_file= filename.substr(0, filename.find_last_of('/')+1);
    report_file += "report.txt";
    time[0]=std::chrono::system_clock::now();
}

void Report::frameHandle(Frame &frame)
{

    if (getProp("PT")==-1)
    {
        if (frame.getMFAS()==0x0)
            setPt(frame.getPT());
    }
    else if (!setted)
    {
        props.emplace("size", frame.getSize());
        props.emplace("frame_cnt", 1);
        setted=true;
    }
    else
    {
        increaseProp("frame_cnt");
        if (frame.getBIP_8() == frame[8])
        {
            increaseProp("BIP-8");
        }
        frame.setBIP_8();
    }
}

void Report::increaseProp(std::string key)
{
    if (props.find(key)==props.end())
        props.emplace(key, 1);
    else
        props.find(key)->second+=1;
}

void Report::addProp(const std::string& key, long value)
{
    props.emplace(key, value);
}
