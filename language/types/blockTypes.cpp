//
// Created by liquidcore7 on 9/5/17.
//

#include "blockTypes.h"

// the only needed property for general block like begin or end
// is info about the next block
std::vector<std::string> Block::getBlockProperties()
{
    return std::vector<std::string>{"Next"};
}

// block body string to property-value dict
// TODO: parse with regular expressions
// TODO: custom exception class with block info
std::map<std::string, std::string> Block::parseProps(std::string&& body)
{
    std::istringstream stream(std::move(body));
    std::string tempLine = " ";
    std::map<std::string, std::string> propMap;

    while (getline(stream, tempLine) && !tempLine.empty())
    {
        auto delimiter = std::find(tempLine.begin(), tempLine.end(), ':');
        if (delimiter == tempLine.end())
            throw std::runtime_error("No delimiter found");

        propMap.insert({std::string(tempLine.begin(), delimiter),
                        std::string(delimiter + 1, tempLine.end())
                       });
    }

    return propMap;
}

// TODO: add info about the property missing/obsolete
Block::Block(std::string label, std::string &&body,
             std::shared_ptr<std::map<std::string, double> > vars)
: blockLabel(std::move(label)), varHeap(std::move(vars))
{
    properties = parseProps(std::move(body));
}

std::string Block::operator[] (const std::string &key)
{
    return this->Block::properties.at(key);
}

std::shared_ptr<std::map<std::string, double> > Block::getHeapPtr() const
{
    return varHeap;
}

/* *****************************************************************************
 * I/O Blocks
 * ************************************************************************** */

std::vector<std::string> IOBlock::getBlockProperties()
{
    return std::vector<std::string>{"Next", "Variables"};
}

std::vector<std::string> IOBlock::parseVars(std::string &&initList)
{
    std::vector<std::string> varList;
    for (auto bg = initList.begin(), next = std::find(bg + 1, initList.end(), ',');
            bg < initList.end();)
    {
        varList.emplace_back( std::string(bg, next) );
        bg = next + 1;
        next = std::find(bg + 1, initList.end(), ',');
    }

    return varList;
}

IOBlock::IOBlock(std::string label, std::string &&block,
                 std::shared_ptr<std::map<std::string, double> > heap)
: Block(std::move(label), std::move(block), std::move(heap))
{
    // getBlockProperties from IOBlock. Don`t use it for InitBlock
    auto exceptedProps = getBlockProperties();

    if (exceptedProps.size() != properties.size())
         throw std::length_error(
                 "Error while constructing \"" + blockLabel +
                 "\" block: " +
                 ((exceptedProps.size() > properties.size()) ?
                 "Properties missing":
                 "Unrecognized properties") );
}


// actual reading of variables
void InputBlock::operator()(const std::vector<std::string> &varList)
{
    for (const auto &vName : varList)
    {
        double value;
        std::cin >> value;
        varHeap->insert({vName, value});
    }
}

InputBlock::InputBlock(std::string label, std::string &&block,
                       std::shared_ptr<std::map<std::string, double> > heap)
: IOBlock(std::move(label), std::move(block), std::move(heap))
{
    auto varList = parseVars( std::move(this->operator[]("Variables")) );
    this->operator()(varList);
}

void OutputBlock::operator() (const std::vector<std::string> &outputVars)
{
    for (const auto &vr : outputVars)
        // assume vr is a comment if not in variablesHeap
        if (std::find_if(varHeap->begin(), varHeap->end(),
                               [&vr] (const std::pair<std::string, double> &lhs)
                               { return lhs.first == vr; }) != varHeap->end())
            std::cout << varHeap->at(vr) << ' ';
        else if (std::find_if(constants.begin(), constants.end(), [&vr]
                (const std::pair<std::string, double> &cnst)
                { return cnst.first == vr;}) != constants.end())
            std::cout << constants[vr] << ' ';
        else
            std::cout << vr << ' ';
}

OutputBlock::OutputBlock(std::string label, std::string &&body,
                         std::shared_ptr<std::map<std::string, double> > heap)
: IOBlock(std::move(label), std::move(body), std::move(heap))
{
    auto varList = parseVars( std::move(this->operator[]("Variables")) );
    this->operator()(varList);
}



void InitBlock::operator()(const std::vector<std::string> &varList)
{
    // note that elements of valueList need to be casted to double
    for (const auto &init : varList)
    {
        auto delimiter = std::find(init.begin(), init.end(), '=');
        std::string varName(init.begin(), delimiter), varValue(delimiter + 1, init.end());
        bool inserted = varHeap->insert(
                {varName, parser::parseBinOp(varValue, varHeap) }
        ).second;
        // if value already in heap and needs to be redefined
        if (!inserted)
            (*varHeap)[varName] = parser::parseBinOp(varValue, varHeap);
    }
}

InitBlock::InitBlock(std::string label, std::string &&body,
                     std::shared_ptr<std::map<std::string, double> > heap)
: IOBlock(std::move(label), std::move(body), std::move(heap))
{
    auto varList = parseVars( std::move(this->operator[]("Variables")) );
    this->operator()(varList);
}


/* ***********************************************************************************************
 * Switch block
 * ******************************************************************************************** */


std::vector<std::string> SwitchBlock::getBlockProperties()
{
    return std::vector<std::string>{"NextSuccess", "NextFailure", "Predicate"};
}

void SwitchBlock::operator()()
{
    bool predSucc = parser::parseBinCmp(properties["Predicate"], varHeap);
    properties.insert({
            "Next",
            ((predSucc) ? properties["NextSuccess"] : properties["NextFailure"])
                      });
}

SwitchBlock::SwitchBlock(std::string label, std::string &&body,
                         std::shared_ptr<std::map<std::string, double> > heap)
: Block(std::move(label), std::move(body), std::move(heap))
{
    auto exceptedProps = SwitchBlock::getBlockProperties();

    if (exceptedProps.size() != properties.size())
        throw std::length_error(
                "Error while constructing \"" + blockLabel +
                "\" block: " +
                ((exceptedProps.size() > properties.size()) ?
                 "Properties missing":
                 "Unrecognized properties") );
    this->operator()();
}







