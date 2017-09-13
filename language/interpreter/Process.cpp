//
// Created by liquidcore7 on 9/12/17.
//

#include "Process.h"

std::pair<std::string, std::string> Process::readBlock(const std::string &label)
{
    std::string rdBuf;
    // find block`s begin
    do  {
        getline(*streamHandle, rdBuf);
    } while (!streamHandle->eof() &&
            rdBuf.find(label) == std::string::npos);
    // throws if reading was terminated due to EOF
    if (streamHandle->eof())
        throw std::logic_error("No \"" + label + "\" block found");
    std::string blockType;
    auto delimiterPos = rdBuf.find(':');
    if (delimiterPos == std::string::npos && delimiterPos + 1 >= rdBuf.size())
        throw std::logic_error("No type specified for \"" + label + "\" block");
    blockType = rdBuf.substr(delimiterPos + 1, rdBuf.find_first_of("\n {") - delimiterPos);
    // devnulls the block`s opening symbol
    if (rdBuf.find('{') == std::string::npos)
        getline(*streamHandle, rdBuf);
    // block body is saved here
    std::string accu;
    do {
        getline(*streamHandle, rdBuf);
        accu += rdBuf;
        accu.push_back('\n');
    }
    while (!streamHandle->eof() &&
            rdBuf.find('}') == std::string::npos);

    if (streamHandle->eof())
        throw std::runtime_error("Unexpected EOF while reading \"" +
                                         label + "\" block body");
    return {accu, blockType};
}

Process::Process(const std::string &filename)
:   memoryHeap(new std::map<std::string, double>)
{
    streamHandle = new std::ifstream(filename);
    current = new Block("Begin", std::move(readBlock("Begin").first), memoryHeap);
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

    std::string lbl = (*current)["Next"];

    if (lbl == "None")
        throw std::runtime_error("nextStep() on last block");

    auto parsedBlock = readBlock(lbl);
    std::string accu = parsedBlock.first, type = parsedBlock.second;
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

