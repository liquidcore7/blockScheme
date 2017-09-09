//
// Created by liquidcore7 on 9/7/17.
//

#ifndef BLOCKSCHEME_EXPRESSIONFOLDER_H
#define BLOCKSCHEME_EXPRESSIONFOLDER_H

#include <cmath>
#include <string>
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
};

namespace parser
{
    double parseBinCmp(const std::string& Cmp,
                       const std::shared_ptr< std::map<std::string, double> > &additional = nullptr);

    double parseBinOp(const std::string& Op,
                      const std::shared_ptr< std::map<std::string, double> > &additional = nullptr);
};

#endif //BLOCKSCHEME_EXPRESSIONFOLDER_H
