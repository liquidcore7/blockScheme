//
// Created by liquidcore7 on 9/12/17.
//

#ifndef BLOCKSCHEME_PROCESS_H
#define BLOCKSCHEME_PROCESS_H

#include "../types/blockTypes.h"
#include <fstream>


struct Process
{
    explicit Process(const std::string&);
    void nextStep();
    void operator() ();

    decltype(memoryHeap) getHeap() const;

    ~Process();

private:
    Block* current;
    std::ifstream* streamHandle;
    std::shared_ptr<std::map<std::string, double> > memoryHeap;
    std::string readBlock(const std::string&);
};


#endif //BLOCKSCHEME_PROCESS_H
