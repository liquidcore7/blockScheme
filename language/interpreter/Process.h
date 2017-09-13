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

    std::shared_ptr<std::map<std::string, double> >
    getHeap() const;

    void dbg();

    ~Process();

private:
    std::string filename;
    Block* current;
    std::shared_ptr<std::map<std::string, double> > memoryHeap;
    //        block body , block type
    std::pair<std::string, std::string> readBlock(const std::string&);
};


#endif //BLOCKSCHEME_PROCESS_H
