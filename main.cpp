#include <iostream>
#include "Frame.h"
#include "FrameReader.h"
#include "FrameWriter.h"


int main(int argc, char* argv[])
{
    if (argc<2)
    {
        std::cout << "Usage: " << argv[0] << "+file+ flags(optional)";
        return 0;
    }
    for (auto k=1; k < argc; ++k)
    {
        FrameReader frameReader(argv[k]);
        std::string out_name = argv[k];

        FrameWriter frameWriter(out_name);

        Report report(out_name);
        report.setFileSize(frameReader.getSize());
        frameWriter.writeReport(report, 0);

        Descrambler descrambler(22000);

        report.startTimer();

        bool mfas = false;
        bool report_setted = false;

        while (Frame frame = frameReader.getFrame(report))
        {
            if (!report_setted)
            {
                report.setFrameSize(frame.getSize());
                report_setted = true;
            }

            //frame.descramble(descrambler);

            if (frame.getSerial() > 2)
            {
                if (frame.getBIP_8() == frame[8])
                    report.increaseBip8Cnt();
            }
            frame.setBIP_8();

            frame.setJC();

            if (!mfas)
            {
                if (frame.getMFAS() == 0)
                {
                    report.setPt(static_cast<int>(frame[frame.getSize() / 4 * 3 + 14]));
                    mfas = true;
                }
            }

            for (int i = 1; i <= frame.getPm(); ++i)
            {
                if ((i * frame.getJC()) % frame.getPm() < frame.getJC())
                {
                    for (int j = 0; j < 80; ++j)
                        frameWriter.setPayloadByte(frame.getPayloadByte(((i - 1) * 80) + j));
                }
            }
            frameWriter.writeFrame();
            report.increaseFrameCnt();
        }

        report.stopTimer();

        frameWriter.writeReport(report, 1);
    }
    return 0;
}
