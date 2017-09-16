//
// Created by liquidcore7 on 9/7/17.
//

#ifndef BLOCKSCHEME_EXPRESSIONFOLDER_H
#define BLOCKSCHEME_EXPRESSIONFOLDER_H

#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>

struct BinCmp
{
    virtual bool operator() (double, double);
};

struct BinOp
{
    virtual double operator() (double, double);
};


namespace cmp
{
    struct less : public BinCmp
    {
        bool operator() (double a, double b) override
        { return a < b; }
    };

    struct lessEq : public BinCmp
    {
        bool operator() (double a, double b) override
        { return a <= b; }
    };

    struct greater : public BinCmp
    {
        bool operator() (double a, double b) override
        { return a > b; }
    };

    struct greaterEq : public BinCmp
    {
        bool operator() (double a, double b) override
        { return a >= b; }
    };

    struct equal : public BinCmp
    {
        bool operator() (double a, double b) override
        { return a == b; }
    };

}

namespace op
{
    struct add : public BinOp
    {
        double operator() (double a, double b) override
        { return a + b;}
    };

    struct mult : public BinOp
    {
        double operator() (double a, double b) override
        { return a * b;}
    };

    struct subtr : public BinOp
    {
        double operator() (double a, double b) override
        { return a - b;}
    };

    struct div : public BinOp
    {
        double operator() (double a, double b) override
        { return a / b;}
    };

    struct mod : public BinOp
    {
        double operator() (double a, double b) override
        { return long(a) % long(b);}
    };

    struct pow : public BinOp
    {
        double operator() (double a, double b) override
        { return std::pow(a, b);}
    };
}


static std::map<std::string, double> constants =
        {
                {"pi", M_PI},
                {"e", M_E},
        };

static std::map<char, BinOp*> operations =
        {
                {'+', new op::add()},
                {'-', new op::subtr()},
                {'*', new op::mult()},
                {'/', new op::div()},
                {'%', new op::mod()},
                {'^', new op::pow()}
        };

static std::map<std::string, BinCmp*> comparisons =
        {
                {">=", new cmp::lessEq()},
                {">=", new cmp::greaterEq()},
                {"<", new cmp::less()},
                {">", new cmp::greater()},
                {"=", new cmp::equal()}
        };

namespace misc
{
    bool isNum(const std::string& num);

    std::string lower(std::string&& arg);

    double stod(std::string var,
                const std::shared_ptr< std::map<std::string, double> > &additional = nullptr);

    // run it at the end of program execution
    void _cleanup();

    template <typename K, typename V>
    std::vector<K> asKeys(const std::map<K, V>& mp)
    {
        std::vector<K> keys(mp.size());
        for (const auto &pair : mp)
            keys.push_back(pair.first);
        return keys;
    }

    bool startsWith(const std::string&, const std::string&);

    // similar to find_first_of, but the order of queries in second arg matters
    auto positionalFind(std::string rng, const std::string& query) -> decltype(rng.begin());

    std::vector<std::string> split(const std::string&, const std::string&);
};


// add equality check for vectors

template <typename T>
bool operator!=(const std::vector<T>& lhs, const std::vector<T>& rhs)
{
    if (lhs.size() != rhs.size())
        return true;
    for (size_t idx = 0; idx < lhs.size(); ++idx)
        if (lhs[idx] != rhs[idx])
            return true;

    return false;
}

namespace parser
{
    bool parseBinCmp(const std::string& Cmp,
                       const std::shared_ptr< std::map<std::string, double> >& = nullptr);

    double parseBinOp(const std::string& Op,
                      const std::shared_ptr< std::map<std::string, double> >& = nullptr);

    std::string evalExpression(std::string Exp,
                          const std::shared_ptr< std::map<std::string, double> >& = nullptr);

    bool foldComparsion(const std::string& Cmp,
                        const std::shared_ptr< std::map<std::string, double> >& = nullptr);
};

#endif //BLOCKSCHEME_EXPRESSIONFOLDER_H
