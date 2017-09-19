//
// Created by liquidcore7 on 9/5/17.
//

#ifndef BLOCKSCHEME_BLOCKTYPES_H
#define BLOCKSCHEME_BLOCKTYPES_H

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <map>

#include "expressionFolder.h"

struct Block
{
    Block() = default;
    Block(std::string, std::string&&,
          std::shared_ptr<std::map<std::string, double> >);
    virtual ~Block() = default;

    virtual std::vector<std::string> getBlockProperties();
    std::string operator[] (const std::string&);
    std::string getLabel() const;

protected:
    std::string blockLabel;
    std::map<std::string, std::string> properties;
    std::shared_ptr<std::map<std::string, double> > varHeap;
    static std::map<std::string, std::string> parseProps(std::string&&);
};

struct IOBlock : public Block
{
    IOBlock() : Block() {};
    IOBlock(std::string, std::string&&,
            std::shared_ptr<std::map<std::string, double> >);
    ~IOBlock() override = default;

    std::vector<std::string> getBlockProperties() override;

protected:
    static std::vector<std::string> parseVars(std::string&&);
};

struct InputBlock : public IOBlock
{
    InputBlock() : IOBlock() {};
    InputBlock(std::string, std::string&&,
               std::shared_ptr<std::map<std::string, double> >);
    ~InputBlock() override = default;

    void operator() (const std::vector<std::string>&);
};

struct OutputBlock : public IOBlock
{
    OutputBlock() : IOBlock() {};
    OutputBlock(std::string, std::string&&,
                std::shared_ptr<std::map<std::string, double> >);
    ~OutputBlock() override = default;

    void operator() (const std::vector<std::string>&);
};

struct InitBlock : public IOBlock
{
    InitBlock() : IOBlock() {};
    InitBlock(std::string, std::string&&,
              std::shared_ptr<std::map<std::string, double> >);
    ~InitBlock() override = default;

    void operator() (const std::vector<std::string>&);

};

struct SwitchBlock : public Block
{
    SwitchBlock() : Block() {}
    SwitchBlock(std::string, std::string&&,
                std::shared_ptr<std::map<std::string, double> >);
    ~SwitchBlock() override  = default;

    std::vector<std::string> getBlockProperties() override;
    void operator() ();
};


#endif //BLOCKSCHEME_BLOCKTYPES_H
