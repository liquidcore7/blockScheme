//
// Created by liquidcore7 on 9/7/17.
//

#include <iostream>
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

    auto positionalFind(std::string rng, const std::string& query) -> decltype(rng.begin())
    {
        for (const auto& searchToken : query)
        {
            auto hit = std::find(rng.begin(), rng.end(), searchToken);
            if (hit != rng.end())
                return hit;
        }
        return rng.end();
    }

    std::vector<std::string> split(const std::string &source, const std::string &delimList)
    {
        std::vector<std::string> sequence;
        for (auto bg = source.begin(); bg != source.end();)
        {
            auto next = std::find_if(delimList.begin(), delimList.end(), [&bg]
                    (const char& x) { return x == *bg;});
            sequence.emplace_back(bg, next);
            bg = next;
        }
        return sequence;
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

    std::string evalExpression(std::string Exp,
                          const std::shared_ptr<std::map<std::string, double> > &varHeap)
    {
        // order is important here
        std::string operations("^*/%-+");

        // finds and evaluates all parenthesized expressions, as their
        // evaluation priority is highest
        auto parenth = std::find(Exp.begin(), Exp.end(), '(');
        while (parenth != Exp.end())
        {
            auto parenthClose = std::find(parenth + 1, Exp.end(), ')');
            // fixes things like (a*(b+c)) by moving the expression begin
            // to next parenthesis
            for (auto walk = parenth + 1; walk < parenthClose; ++walk)
                if (*walk == '(')
                    parenth = walk;

            std::string innerExp(parenth + 1, parenthClose),
                    evaluated = evalExpression(innerExp, varHeap);
            // insert the evaluated part into original string
            auto inserter = Exp.erase(parenth, parenthClose + 1);
            Exp.insert(inserter, evaluated.begin(), evaluated.end());
            parenth = std::find(Exp.begin(), Exp.end(), '(');
        }

        auto opPos = misc::positionalFind(Exp, operations);
        while (opPos < Exp.end())
        {
            auto opBegin = Exp.begin();
            // problematically to solve with STL algorithms (reverse_iter cast)
            for (auto bg = opPos; bg > Exp.begin(); --bg)
            {
                if (operations.find(*(bg - 1)) != std::string::npos)
                {
                    opBegin = bg;
                    break;
                }
            }
            auto opEnd = Exp.end();

            for (auto bg = opPos + 1; bg < Exp.end(); ++bg)
                if (operations.find(*bg) != std::string::npos)
                {
                    opEnd = bg;
                    break;
                }

            std::string tmp(opBegin, opEnd);
            std::string evaluated = std::to_string(
                    parser::parseBinOp(tmp, varHeap)
            );
            auto inserter = Exp.erase(opBegin, opEnd)++;
            Exp.insert(inserter, evaluated.begin(), evaluated.end());
            opPos = misc::positionalFind(Exp, operations);
        }

	    return Exp;
    }

    bool foldComparsion(const std::string &Cmp,
                        const std::shared_ptr<std::map<std::string, double> > &varHeap)
    {
        auto relation = Cmp.find_first_of("<>=");

        if (relation == std::string::npos)
            throw std::logic_error("Not a comparison");

        std::string left = Cmp.substr(0, relation), right = Cmp.substr(relation + 1),
                comp = Cmp.substr(relation, 1);

        if (Cmp.find('=', relation + 1) != std::string::npos)
        {
            right = right.substr(1);
            comp.push_back(Cmp[relation + 1]);
        }
        left = evalExpression(left, varHeap), right = evalExpression(right, varHeap);
        return parser::parseBinCmp(left + comp + right, varHeap);
    }

};
