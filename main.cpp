#include <iostream>
#include "Frame.h"
#include "FrameReader.h"
#include "FrameWriter.h"
#include "FrameHolders.h"
#include "FrameHandler.h"


int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << "+files+flags(optional)";
        return 0;
    }
    bool flag = false;
    std::string a = argv[argc - 1];
    if (a == "-d")
        flag = true;

    for (auto k = 1; k < argc - flag; ++k)
    {
        std::string file_name = argv[k];

        FrameReader frameReader(file_name);
        if (!frameReader.is_init())
            continue;

        FrameHolder holder;
        FrameWriter frameWriter(file_name);
        Report report(file_name);

        report.addProp("file_size", frameReader.getSize());

        bool gmp_cnt = false;

        Descrambler descrambler(22000);

        report.writeReport(0);


        while (Frame frame = (frameReader.getFrame(report)))
        {
            if (flag)
                frame.descramble(descrambler);

            report.frameHandle(frame);

            if (report.getProp("PT") == -1)
            {
                if (report.getProp("MFAS")==-1)
                {
                    if (frame.getMFAS()==0x0)
                    {
                        report.addProp("MFAS", 1);
                        holder.emplace_frame(frame);
                    }
                }
                else
                {
                    holder.emplace_frame(frame);
                }
                continue;
            }
            holder.emplace_frame(frame);


            switch (report.getProp("PT"))
            {
                case 0x07:
                {
                    frame.setPm(190);
                    Frame_handler_pt_7 handler7;
                    handler7.get_payload(holder);
                    frameWriter.pushPayload(holder);
                    break;
                }


                case 0x21:
                {
                    frame.setPm(15200);
                    if (!gmp_cnt)
                    {
                        for (int i = 0; i < holder.framesSize(); ++i)
                        {
                            if (holder.get(i).getGMP_cnt() == 0x0)
                            {
                                gmp_cnt = true;
                                holder.remove(i);
                            }
                        }
                    }
                    Frame_handler_pt_21 handler21;
                    handler21.get_payload(holder);
                    if (holder.getseries() > 0)
                    {
                        frameWriter.pushPayload(holder);
                    }
                    break;
                }
                default:
                    continue;
            }
        }
        report.writeReport(1);

        frameWriter.writeFrame();

        if (report.getProp("PT") == 0x21)
        {
            std::vector<std::string> files = frameWriter.getFilenames();
            for (const auto &i: files)
            {
                FrameReader subReader(i);
                if (!subReader.is_init())
                    continue;
                Report subreport(i);

                subreport.addProp("file_size", subReader.getSize());
                subreport.writeReport(2);
                subreport.addProp("miss_sync", 0);

                while (Frame frame = (subReader.getFrame(subreport)))
                {
                    subreport.frameHandle(frame);
                }
                subreport.writeReport(1);
            }

        }
    }

    return 0;
}
