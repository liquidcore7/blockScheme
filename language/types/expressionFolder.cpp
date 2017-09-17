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
        return std::all_of(num.begin(), num.end(), [](const char &digit) { return (isdigit(digit)
                                                                                   || digit == '.'
                                                                                   || digit == '-'); });
    }

    std::string lower(std::string &&arg)
    {
        for (auto &c : arg)
            c = tolower(c);
        return arg;
    }

    double stod(std::string var, const std::shared_ptr< std::map<std::string, double> > &additional)
    {
        if (var.empty())
            var = "0.0";
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

    auto positionalFind(const std::vector<std::string> &rng, const std::string& query) -> decltype(rng.begin())
    {
        for (const auto& searchToken : query)
        {
            auto hit = std::find(rng.begin(), rng.end(), std::string(1, searchToken));
            if (hit != rng.end())
                return hit;
        }
        return rng.end();
    }

    std::vector<std::string> split(const std::string &source, const std::string &delimList)
    {
        std::vector<std::string> sequence;
        auto bg = source.begin(), next = bg + 1;
        for (bg, next; bg < source.end(); ++next)
        {
            if (next == source.end())
            {
                sequence.emplace_back(bg, next);
                return sequence;
            }
            bool match = std::any_of(delimList.begin(), delimList.end(), [&next]
                    (const char& x) { return x == *next;});
            if (match)
            {
                if (bg != next)
                    sequence.emplace_back(bg, next);
                sequence.emplace_back(1, *next);
                bg = next + 1;
            }

        }
    }

    std::string join(const std::string& delimiter, const std::vector<std::string>& source)
    {
        std::string joined;
        for (const auto& elem : source)
            joined += elem + delimiter;
        return joined.substr(0, joined.size() - delimiter.size());
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
        auto opr = Op.find_first_of("+*/^%-", 1);

        if (opr == std::string::npos)
            return misc::stod(Op, additional);

        std::string left = Op.substr(0, opr), right = Op.substr(opr + 1);
        return operations[ Op[opr] ]->operator()
                (misc::stod(left, additional), misc::stod(right, additional));
    }

    // TODO: remove binOp
    std::string evalExpression(std::string Exp,
                          const std::shared_ptr<std::map<std::string, double> > &varHeap)
    {
        // order is important here
        std::string operators("()^*/%-+");

        auto expSequence = misc::split(Exp, operators);
        auto prBegin = std::find(expSequence.begin(), expSequence.end(), "(");
        while (prBegin != expSequence.end())
        {
            auto prEnd = std::find(prBegin, expSequence.end(), ")");

            for (auto walk = prBegin + 1; walk < prEnd; ++walk)
                if (*walk == "(")
                    prBegin = walk;

            std::string parsed = evalExpression(
                    misc::join("", std::vector<std::string>(prBegin + 1, prEnd)),
                    varHeap
            );
            auto inserter = expSequence.erase(prBegin, prEnd + 1);
            expSequence.insert(inserter, parsed);

            prBegin = std::find(expSequence.begin(), expSequence.end(), "(");
        }

        auto expBegin = misc::positionalFind(expSequence, operators);
        while (expBegin != expSequence.end())
        {
            std::string parsed = std::to_string( parseBinOp(
                    misc::join("", std::vector<std::string>(expBegin - 1, expBegin + 2)),
                    varHeap
            ));

            auto inserter = expSequence.erase(expBegin - 1, expBegin + 2);
            expSequence.insert(inserter, parsed);


            expBegin = misc::positionalFind(expSequence, operators);
        }

        return misc::join("", expSequence);
    }

    // TODO: remove binCmp
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
