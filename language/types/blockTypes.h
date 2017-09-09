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
    Block(std::string, std::string&&);
    virtual ~Block() = default;

    virtual std::vector<std::string> getBlockProperties();
    std::string operator[] (const std::string&);

protected:
    std::string blockLabel;
    std::map<std::string, std::string> properties;
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
    std::shared_ptr< std::map<std::string, double> > variablesHeap;
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

// pass by IOBlock constructor
struct IOBridge : public Block
{
    IOBridge() : Block() {}
    IOBridge(std::string a, std::string&&b) : Block(std::move(a), std::move(b))
    {}
    ~IOBridge() override = default;
};

struct InitBlock : public IOBlock, IOBridge
{
    InitBlock() : IOBlock() {};
    InitBlock(std::string, std::string&&,
              std::shared_ptr<std::map<std::string, double> >);
    ~InitBlock() override = default;

    std::vector<std::string> getBlockProperties() override;
    void operator() (const std::vector<std::string>&);
};

struct SwitchBlock : public Block
{
    SwitchBlock() : Block() {}
    SwitchBlock(std::string, std::string&&,
                std::shared_ptr<std::map<std::string, double> >);
    ~SwitchBlock() override  = default;

    std::vector<std::string> getBlockProperties() override;

};


#endif //BLOCKSCHEME_BLOCKTYPES_H
