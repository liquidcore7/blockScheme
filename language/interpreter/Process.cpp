//
// Created by liquidcore7 on 9/12/17.
//

#include "Process.h"

std::pair<std::string, std::string> Process::readBlock(const std::string &label)
{

    std::ifstream streamHandle(filename);
    std::string rdBuf;
    // find block`s begin
    while (!streamHandle.eof() &&
            !misc::startsWith(rdBuf, label))
    {
        getline(streamHandle, rdBuf);
    }
    // throws if reading was terminated due to EOF
    if (streamHandle.eof())
        throw std::logic_error("No \"" + label + "\" block found");

    auto delimiterPos = rdBuf.find(':');

    if (delimiterPos == std::string::npos || delimiterPos + 1 >= rdBuf.size())
        throw std::logic_error("No type specified for \"" + label + "\" block");

    auto typeEnd = rdBuf.find_first_of(" {");
    std::string blockType = rdBuf.substr(++delimiterPos, typeEnd -
            ((typeEnd != std::string::npos) ? delimiterPos : 0));

    // devnulls the block`s opening symbol
    getline(streamHandle, rdBuf);
    if (rdBuf.find('{') != std::string::npos)
        getline(streamHandle, rdBuf);
    // block body is saved here
    std::string accu;
    while (!streamHandle.eof() &&
            rdBuf.find('}') == std::string::npos)
    {
        accu += rdBuf;
        accu.push_back('\n');
        getline(streamHandle, rdBuf);
    }

    if (streamHandle.eof())
        throw std::runtime_error("Unexpected EOF while reading \"" +
                                         label + "\" block body");

    return {accu, blockType};
}

Process::Process(const std::string &_filename)
:   memoryHeap(new std::map<std::string, double>), filename(_filename)
{
    current = new Block("Begin", readBlock("Begin").first, memoryHeap);
}

Process::~Process()
{
    delete current;
}

std::shared_ptr<std::map<std::string, double> > Process::getHeap() const
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
        current = new Block(lbl, std::move(accu), memoryHeap);
    else if (type == "Input")
        current = new InputBlock(lbl, std::move(accu), memoryHeap);
    else if (type == "Output")
        current = new OutputBlock(lbl, std::move(accu), memoryHeap);
    else if (type == "Init")
        current = new InitBlock(lbl, std::move(accu), memoryHeap);
    else if (type == "Switch")
        current = new SwitchBlock(lbl, std::move(accu), memoryHeap);
    else
        throw std::logic_error("Block type not recognized");
}

void Process::operator()()
{
    while ((*current)["Next"] != "None")
        nextStep();
}

// output looks too ugly, rewrite or remove
void Process::dbg()
{
    while ((*current)["Next"] != "None")
    {
        std::cout << "[" + current->getLabel() + "]:" << std::endl;
        std::cout << "Next:" << (*current)["Next"] << '\n';

        std::cout << "Heap:\n" << "Variables\t\tValues\n";
        for (const auto &x : *memoryHeap)
            std::cout << x.first << "\t\t\t" << x.second << std::endl;
        nextStep();
    }

}

