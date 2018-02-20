#define BOOST_TEST_MODULE Fraction test
#include <boost/test/unit_test.hpp>

#include "Fraction.h"

using namespace omnn::math;

std::string l(const omnn::math::Valuable& v)
{
    std::stringstream ss;
    ss << v;
    return ss.str();
}

BOOST_AUTO_TEST_CASE(Fraction_tests)
{
    const auto a = 1_v / 2;
	auto c = 3_v / 1;
    auto b = a * 4;
	auto d = 2_v / 4;
    
    BOOST_TEST(a*b==1);
	BOOST_TEST((c += b) == 5);
	BOOST_TEST((c *= a) == 5_v / 2);
	BOOST_TEST((c /= a) == 5);
	BOOST_TEST((c--) == 5);
	BOOST_TEST((c++) == 4);
	BOOST_TEST(c > a);
	BOOST_TEST(a == d);
	BOOST_TEST(a - d == 0);

    Valuable _ = 3.1_v;
    BOOST_TEST(Fraction::cast(_));

    BOOST_TEST((1_v/2)^2_v == 1_v/4);
    
    Variable v;
    _ = 1_v / (1_v / v);
    BOOST_TEST(_ == v);
}