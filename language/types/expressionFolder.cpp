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
    // TODO: rewrite with regexp
    bool isNum(const std::string &num)
    {
        return ( (num[0] == '-' || isdigit(num[0]) )
                 && std::all_of(num.begin() + 1, num.end(),
                                [](const char &digit) { return (isdigit(digit) || digit == '.');})
        );
    }

    // pi can be marked as Pi or PI
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


    bool isOperator(const char &c)
    {
        const std::string operators("()^*/%+-");
        return operators.find(c) != std::string::npos;
    }
};


namespace parser
{

    numOpVariant::numOpVariant(double arg)
            : _val(arg), _contains(0)
    {}

    numOpVariant::numOpVariant(char arg)
            : _op(arg), _contains(1)
    {}

    bool numOpVariant::hasOperator() const
    {
        return _contains;
    }

    template <typename T>
    T numOpVariant::get() const
    {
        if (hasOperator())
            return _op;
        else
            return _val;
    }



    Expression::Expression(const std::string &source,
            const std::shared_ptr<std::map<std::string, double> >& vmap)
    : varMap(vmap)
    {
        auto bg = source.begin(), next = bg + 1;
        for (bg, next; bg < source.end(); ++next)
        {
            if (next == source.end())
            {
                expArr.emplace_back(
                        misc::stod( std::string(bg, next), varMap)
                );
                break;
            }

            if (misc::isOperator(*next))
            {
                if (bg != next)
                    expArr.emplace_back(
                            misc::stod( std::string(bg, next), varMap) );
                expArr.emplace_back(*next);
                bg = next + 1;
            }
        } // end for
    }

    Expression::Expression(std::vector<parser::numOpVariant> prevExp,
                           const std::shared_ptr<std::map<std::string, double> > &vmap)
    : expArr(std::move(prevExp)), varMap(vmap)
    {}


    It Expression::find_op(const char &op) const
    {
        return std::find_if(expArr.begin(), expArr.end(), [&op]
                (const numOpVariant& v)
        {
            if (v.hasOperator())
                return v.get<char>() == op;
            else
                return false;
        });
    }

    double Expression::calculate()
    {
        // take care of brackets
        auto openBr = find_op('('), closeBr = find_op(')');

        while (openBr != expArr.end())
        {
            for (auto walker = openBr; walker < closeBr; ++walker)
                if (walker->hasOperator())
                    if (walker->get<char>() == '(')
                        openBr = walker; // openBr now points to closest to closeBr bracket

            std::vector<parser::numOpVariant> innerExp(openBr + 1, closeBr);
            auto insertPos = expArr.erase(openBr, closeBr + 1);
            expArr.insert(insertPos, parser::numOpVariant(
                    Expression(innerExp, varMap).calculate()
            ));

            openBr = find_op('('), closeBr = find_op(')');
        }

        const std::string opQueue("^*/%+-");
        for (const auto &op : opQueue)
        {
            auto opPos = find_op(op);
            while (opPos != expArr.end())
            {
                double a = (opPos - 1)->get<double>(), b = (opPos + 1)->get<double>();
                double result = operations[opPos->get<char>()]->operator() (a, b);
                auto insertPos = expArr.erase(opPos - 1, opPos + 2);
                expArr.insert(insertPos, numOpVariant(result));

                opPos = find_op(op);
            }
        }
        return expArr.front().get<double>();
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

        double a = Expression(left, varHeap).calculate(), b = Expression(right, varHeap).calculate();
        return comparisons[comp]->operator()(a, b);
    }

    double evalExpression(const std::string &Exp,
                          const std::shared_ptr<std::map<std::string, double> > &vmap)
    {
        return Expression(Exp, vmap).calculate();
    }

}; // namespace parser
