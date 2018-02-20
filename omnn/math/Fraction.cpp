//
// Created by Сергей Кривонос on 01.09.17.
//
#include "Fraction.h"
#include "Integer.h"
#include "Sum.h"
#include "Product.h"
#include <boost/lexical_cast.hpp>

namespace std{
    template<>
    struct is_signed<boost::multiprecision::cpp_int> {
        bool value = true;
    };
}

namespace omnn{
namespace math {
	Fraction::Fraction(const Integer& n)
		:numerator(n), denominator(1)
	{
        hash = numerator.Hash() ^ denominator.Hash();
	}
    
    Fraction::Fraction(const boost::multiprecision::cpp_dec_float_100& f)
    {
        auto s = boost::lexical_cast<std::string>(f);
        if (s=="nan") {
            throw s;
        }
        auto doti = s.find_first_of('.');
        if (doti == -1)
        {
            numerator = Integer(boost::multiprecision::cpp_int(f));
            denominator = 1;
        }
        else
        {
            auto fsz = s.length() - doti - 1;
            s.erase(doti);
            numerator = Integer(boost::multiprecision::cpp_int(s));
            denominator = 10_v^Integer(fsz);
        }
    }
    
	Valuable Fraction::operator -() const
    {
        return Fraction(-numerator, denominator);
    }

    void Fraction::optimize()
    {
//        if (!optimizations) {
//            hash = numerator.Hash() ^ denominator.Hash();
//            return;
//        }
    reoptimize_the_fraction:
        numerator.optimize();
        denominator.optimize();
        
        while(denominator.IsFraction())
        {
            auto dn = cast(denominator);
            numerator *= dn->denominator;
            denominator = std::move(dn->numerator);
        }

        if(denominator.IsInt() && denominator == 1_v)
        {
            Become(std::move(numerator));
            return;
        }

        // integers
        auto n = Integer::cast(numerator);
        auto dn = Integer::cast(denominator);
        if (n)
        {
            if (*n == 0_v)
            {
                if (dn && *dn == 0_v)
                    throw "NaN";
                Become(0);
                return;
            }
            if (dn)
            {
                if (*dn == 1_v)
                {
                    Become(Integer(*n));
                    return;
                }
                Integer::base_int d = boost::gcd(
                    static_cast<Integer::base_int>(*n),
                    static_cast<Integer::base_int>(*dn));
                if (d != 1) {
                    numerator /= Integer(d);
                    denominator /= Integer(d);
                    optimize();
                }
            }
        }
        else
        {
            // products
            auto n = Product::cast(numerator);
            auto dn = Product::cast(denominator);
            if (n) {
                if (dn) {
                    auto commonVarsPart = n->getCommVal(*dn);
                    numerator /= commonVarsPart;
                    denominator /= commonVarsPart;
                    goto reoptimize_the_fraction;
                }
                else
                {
                    if (n->Has(denominator)) {
                        Become(*n / denominator);
                        return;
                    }
                }
            }
            else if (dn)
            {
                if (dn->Has(numerator)) {
                    denominator /= numerator;
                    numerator = 1_v;
                    goto reoptimize_the_fraction;
                }
            }
            else // no products
            {
                // sum
//                auto s = Sum::cast(numerator);
//                if (s) {
//                    auto sum(std::move(*s));
//                    sum /= denominator;
//                    Become(std::move(sum));
//                    return;
//                }
            }
        }
        
        if (IsFraction()) {
            hash = numerator.Hash() ^ denominator.Hash();
        }
    }
    
    Valuable& Fraction::operator +=(const Valuable& v)
    {
        auto i = cast(v);
        if (i){
			auto f = *i;
            if(denominator != f.denominator) {
                f.numerator *= denominator;
                f.denominator *= denominator;
                numerator *= i->denominator;
                denominator *= i->denominator;
            }

            numerator += f.numerator;
            optimize();
        }
        else
        {
            auto i = Integer::cast(v);
            if(i)
            {
                *this += Fraction(*i);
            }
            else
            {
                return Become(Sum {*this, v});
            }
        }
        return *this;
    }

    Valuable& Fraction::operator *=(const Valuable& v)
    {
        auto f = cast(v);
        if (f)
        {
            numerator *= f->numerator;
            denominator *= f->denominator;
        }
        else{
            if (v.IsInt()) {
                numerator *= v;
            }
            else
            {
                return Become(Product{*this, v});
            }
        }
		
		optimize();
        return *this;
    }

    Valuable& Fraction::operator /=(const Valuable& v)
    {
        auto f = cast(v);
        if (f)
		{
			numerator *= f->denominator;
			denominator *= f->numerator;
		}
        else
        {
			denominator *= v;
        }
		optimize();
		return *this;
    }

    Valuable& Fraction::operator %=(const Valuable& v)
    {
        Integer d(*this / v);
		return *this -= d * v;
    }

    Valuable& Fraction::operator --()
    {
        return *this+=-1;
    }

    Valuable& Fraction::operator ++()
    {
        return *this+=1;
    }

    Valuable& Fraction::operator^=(const Valuable& v)
    {
        auto i = Integer::cast(v);
        if(i)
        {
            if (*i != 0_v) {
                if (*i > 1_v) {
                    auto a = *this;
                    for (auto n = *i; n > 1_v; --n) {
                        *this *= a;
                    }
                    optimize();
                    return *this;
                }
            }
            else { // zero
                if (numerator == 0_v)
                    throw "NaN"; // feel free to handle this properly
                else
                    return Become(1_v);
            }
        }
        else if(IsSimple())
        {
            auto f = Fraction::cast(v);
            if (f->IsSimple())
            {
                auto n = f->getNumerator();
                auto dn = f->getDenominator();

                if (n != 1_v)
                    *this ^= n;
                Valuable nroot;
                bool rootFound = false;
                Valuable left =0, right = *this;

                while (!rootFound)
                {
                    nroot = left +(right - left) / 2_v;
                    auto result = nroot ^ dn;
                    if (result == *this)
                    {
                        rootFound = true;
                        return Become(std::move(nroot));
                    }
                    else if (*this < result)
                    {
                        right = nroot;
                    }
                    else
                    {
                        left = nroot;
                    }
                }
            }
        }

        return Become(Exponentiation(*this, v));
    }
    

    bool Fraction::operator <(const Valuable& v) const
    {
        auto i = cast(v);
		if (i)
		{
			auto f = *i;
			auto l = *this;
			if (l.denominator != f.denominator) {
				f.numerator *= l.denominator;
				l.numerator *= i->denominator;
			}
			return l.numerator < f.numerator;//arbitrary < i->arbitrary;
		}
            
        else
        {
			auto i = Integer::cast(v);
			if (i)
			{
				auto f = (decltype(numerator))(*i);
				auto l = *this;
				return l.numerator < f*denominator;
			}
			else
			{
				// try other type
				// no type matched
				
			}
        }

        // not implemented comparison to this Valuable descent
        return base::operator <(v);
    }

    bool Fraction::operator ==(const Valuable& v) const
    {
        auto i = cast(v);
		if (i)
		{
            return hash == i->Hash()
                && denominator.Hash() == i->denominator.Hash()
                && numerator.Hash() == i->numerator.Hash()
                && ((denominator == i->denominator && numerator == i->numerator)
//                    || (numerator * i->denominator == i->numerator * denominator)
                    );
        }
        else if (Variable::cast(v)
                 || (v.FindVa()==nullptr) != (FindVa()==nullptr))
        {
            return false;
        }
        else
        {
			auto i = Integer::cast(v);
			if (i)
			{
                if (IsSimple())
                {
                    auto& n = Integer::cast(numerator)->operator Integer::const_base_int_ref();
                    auto& dn = Integer::cast(denominator)->operator Integer::const_base_int_ref();
                    auto g = boost::gcd(n,dn);
                    return (g == dn && n/g == *i) || (n == 0 && *i == 0);
                }
                else
                {
                    return false;
                }
			}
        }
        
        // no type matched
        return base::operator ==(v);
    }

    std::ostream& Fraction::print(std::ostream& out) const
    {
        auto noNeedBraces = [](auto& i) {
            return Integer::cast(i) || Variable::cast(i);
        };
        out << '(';
        if(!noNeedBraces(numerator))
            out << '(' << numerator << ')';
        else
            out << numerator;
        out << '/';
        if(!noNeedBraces(denominator))
            out << '(' << denominator << ')';
        else
            out << denominator;
        out << ')';
        return out;
    }
    
    const Valuable::vars_cont_t& Fraction::getCommonVars() const
    {
        vars = numerator.getCommonVars();
        for(auto& r : denominator.getCommonVars())
        {
            vars[r.first] -= r.second;
        }
        return vars;
    }

    Fraction::operator boost::multiprecision::cpp_dec_float_100() const
    {
        if (IsSimple())
        {
            auto& num = *Integer::cast(numerator);
            boost::multiprecision::cpp_dec_float_100 f(static_cast<boost::multiprecision::cpp_int>(num));
            auto & d = *Integer::cast(denominator);
            f /= boost::multiprecision::cpp_dec_float_100(static_cast<boost::multiprecision::cpp_int>(d));
            // TODO : check validity
            return f;
        }
        else
            throw "Implement!";
    }
    
    omnn::math::Fraction Fraction::Reciprocal() const
    {
        return Fraction(denominator, numerator);
    }

    const Variable* Fraction::FindVa() const
    {
        auto va = denominator.FindVa();
        return va ? va : numerator.FindVa();
    }

    void Fraction::CollectVa(std::set<Variable>& s) const
    {
        numerator.CollectVa(s);
        denominator.CollectVa(s);
    }
    
    void Fraction::Eval(const Variable& va, const Valuable& v)
    {
        numerator.Eval(va, v);
        denominator.Eval(va, v);
//        optimize();
    }
    
    bool Fraction::IsSimple() const
    {
        return Integer::cast(numerator)
            && Integer::cast(denominator)
        ;
    }
}}