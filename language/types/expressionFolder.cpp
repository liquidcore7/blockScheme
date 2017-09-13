//
// Created by liquidcore7 on 9/7/17.
//

#include "expressionFolder.h"

bool BinCmp::operator()(double, double)
{
    return false;
}


double BinOp::operator()(double, double)
{
    return false;
}

namespace misc
{

    bool isNum(const std::string &num)
    {
        return std::all_of(num.begin(), num.end(), [](const char &digit) { return (isdigit(digit) || digit == '.'); });
    }

    std::string lower(std::string &&arg)
    {
        for (auto &c : arg)
            c = tolower(c);
        return arg;
    }

    double stod(std::string var, const std::shared_ptr< std::map<std::string, double> > &additional)
    {
        // if additional variables heap exists and var is found there, grab it from heap
        if (additional)
            if (std::find_if(additional->begin(), additional->end(),
                             [&var](const std::pair<std::string, double> &l) { return l.first == var; }) !=
                additional->end())
                return additional->at(var);
        // else try converting it to number or see if its a constant. can throw
        return ((isNum(var)) ? std::stod(var) : constants[lower(std::move(var))]);
    }

    // run it at the end of program execution
    void _cleanup()
    {
        for (auto &cmp : comparisons)
            delete cmp.second;
        for (auto &op : operations)
            delete op.second;
    }

    bool startsWith(const std::string& l, const std::string& r)
    {
        return l.substr(0, r.size()) == r;
    }

};


namespace parser
{
    bool parseBinCmp(const std::string& Cmp,
                       const std::shared_ptr< std::map<std::string, double> > &additional)
    {
        auto relation = Cmp.find_first_of("<>=");

        if (relation == std::string::npos)
            throw std::logic_error("Not a binary comparison");

        std::string left = Cmp.substr(0, relation), right = Cmp.substr(relation + 1),
                comp = Cmp.substr(relation, 1);
        if (Cmp.find('=', relation + 1) != std::string::npos)
        {
            right = right.substr(1);
            comp.push_back(Cmp[relation + 1]);
        }

        return comparisons[comp]->operator()
                (misc::stod(left, additional), misc::stod(right, additional));

    }

    double parseBinOp(const std::string& Op,
                      const std::shared_ptr< std::map<std::string, double> > &additional)
    {
        auto opr = Op.find_first_of("+-*/^%");

        if (opr == std::string::npos)
            return misc::stod(Op, additional);

        std::string left = Op.substr(0, opr), right = Op.substr(opr + 1);
        return operations[ Op[opr] ]->operator()
                (misc::stod(left, additional), misc::stod(right, additional));
    }

};
