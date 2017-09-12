//
// Created by liquidcore7 on 9/12/17.
//

#include "Process.h"

std::string Process::readBlock(const std::string &label)
{
    return std::__cxx11::string();
}

Process::Process(const std::string &filename)
:   memoryHeap(new std::map<std::string, double>)
{
    streamHandle = new std::ifstream(filename);
    std::string rdBuf;
    do{
        getline(*streamHandle, rdBuf);
    } while (!streamHandle->eof() && rdBuf.find("Begin:Block") == std::string::npos);

    if (streamHandle->eof())
        throw std::logic_error("No Begin block found");

    // if block opening is on the next line
    if (rdBuf.find('{') == std::string::npos)
        getline(*streamHandle, rdBuf);

    std::string accu;
    do {
        getline(*streamHandle, rdBuf);
        accu += rdBuf;
        accu.push_back('\n');
    }
    while (rdBuf.find('}') == std::string::npos);

    current = new Block("Begin", std::move(accu), memoryHeap);
}

Process::~Process()
{
    delete streamHandle;
    delete current;
}

decltype(memoryHeap) Process::getHeap() const
{
    return memoryHeap;
}

void Process::nextStep()
{

    std::string lbl = (*current)["Next"], buffer;

    if (lbl == "None")
        throw std::runtime_error("nextStep() on last block");

    do {
        getline(*streamHandle, buffer);
    } while (!streamHandle->eof() && buffer.find(lbl) == std::string::npos);

    auto delimiter = buffer.find(':');
    std::string type = buffer.substr(delimiter + 1, buffer.find(' ') - delimiter);

    if (buffer.find('{') == std::string::npos)
        getline(*streamHandle, buffer);

    std::string accu;
    do {
        getline(*streamHandle, buffer);
        accu += buffer;
        accu.push_back('\n');
    }
    while (buffer.find('}') == std::string::npos);

    delete current;

    // please add the fucking switch statement for cstrings
    if (type == "Block")
        current = new Block(std::move(lbl), std::move(accu), memoryHeap);
    else if (type == "Input")
        current = new InputBlock(std::move(lbl), std::move(accu), memoryHeap);
    else if (type == "Output")
        current = new OutputBlock(std::move(lbl), std::move(accu), memoryHeap);
    else if (type == "Init")
        current = new InitBlock(std::move(lbl), std::move(accu), memoryHeap);
    else if (type == "Switch")
        current = new SwitchBlock(std::move(lbl), std::move(accu), memoryHeap);
    else
        throw std::logic_error("Block type not recognized");
}

