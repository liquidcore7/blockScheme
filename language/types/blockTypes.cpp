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
Block::Block(std::string label, std::string &&body)
: blockLabel(std::move(label))
{
    auto gotPropList = parseProps(std::move(body));
   // auto exceptedProps = getBlockProperties();

   /* if (exceptedProps.size() != gotPropList.size())
        throw std::length_error(
                "Error while constructing " + blockLabel +
                " block: " +
                ((exceptedProps.size() > gotPropList.size()) ?
                "Properties missing":
                "Unrecognized properties") ); */
    // there is a HUGE issue here. Do smth with getBlockProps, it calls for Block not for caller class
    properties = gotPropList;
}

std::string Block::operator[] (const std::string &key)
{
    return properties.at(key);
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
    for (auto bg = initList.begin(), next = std::find(bg, initList.end(), ',');
            bg != initList.end(); bg = next, next = std::find(bg, initList.end(), ','))
    {
        varList.push_back( std::string(bg, next) );
    }

    return varList;
}

IOBlock::IOBlock(std::string label, std::string &&block,
                 std::shared_ptr<std::map<std::string, double> > heap)
: Block(std::move(label), std::move(block)), variablesHeap(heap)
{
    auto varList = parseVars( std::move(this->operator[]("Variables")) );
    this->operator()(varList);
}

void IOBlock::operator()(const std::vector<std::string> &)
{

}


// actual reading of variables
void InputBlock::operator()(const std::vector<std::string> &varList)
{
    for (const auto &vName : varList)
    {
        double value;
        std::cin >> value;
        variablesHeap->insert({vName, value});
    }
}

InputBlock::InputBlock(std::string label, std::string &&block,
                       std::shared_ptr<std::map<std::string, double> > heap)
: IOBlock(std::move(label), std::move(block), std::move(heap))
{}

void OutputBlock::operator() (const std::vector<std::string> &outputVars)
{
    for (const auto &vr : outputVars)
        // assume vr is a comment if not in variablesHeap
        if (std::find_if(variablesHeap->begin(), variablesHeap->end(),
                               [&vr] (const std::pair<std::string, int> &lhs)
                               { return lhs.first == vr; }) != variablesHeap->end())
            std::cout << variablesHeap->at(vr) << ' ';
        else
            std::cout << vr << ' ';
}

OutputBlock::OutputBlock(std::string label, std::string &&body,
                         std::shared_ptr<std::map<std::string, double> > heap)
: IOBlock(std::move(label), std::move(body), std::move(heap))
{}

// TODO: support for initializer list-style variable definitions
std::vector<std::string> InitBlock::getBlockProperties()
{
    return std::vector<std::string>{"Next", "Variables", "Values"};
}

void InitBlock::operator()(const std::vector<std::string> &varList)
{
    // note that elements of valueList need to be casted to double
    auto valueList = parseVars(this->operator[]("Values"));
    if (varList.size() > valueList.size())
        throw std::range_error("Not enough initializers");
    for (size_t idx = 0; idx < varList.size(); ++idx)
    {
        bool inserted = variablesHeap->insert(
                {varList[idx], parser::parseBinOp(valueList[idx], variablesHeap) }
        ).second;
        // if value already in heap and needs to be redefined
        if (!inserted)
            (*variablesHeap)[ varList[idx] ] = parser::parseBinOp(valueList[idx], variablesHeap);
    }
}

InitBlock::InitBlock(std::string label, std::string &&body,
                     std::shared_ptr<std::map<std::string, double> > heap)
: IOBlock(std::move(label), std::move(body), std::move(heap))
{}


/* ***********************************************************************************************
 * Switch block
 * ******************************************************************************************** */


std::vector<std::string> SwitchBlock::getBlockProperties()
{
    return std::vector<std::string>{"NextSuccess", "NextFailure", "Predicate"};
}



