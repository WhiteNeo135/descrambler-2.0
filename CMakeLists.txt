cmake_minimum_required(VERSION 3.10)
project(OTN_payload_searcher)

set(CMAKE_CXX_STANDARD 20)

add_executable(OTN_payload_searcher main.cpp Frame.cpp FrameReader.cpp FrameWriter.cpp FrameHolders.cpp FrameHandler.cpp FrameHandler.h)