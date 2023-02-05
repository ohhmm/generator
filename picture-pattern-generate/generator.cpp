
#include "gen.h"

#include <omnn/math/Variable.h>
#include <omnn/math/VarHost.h>
using namespace omnn::math;

#include <boost/archive/binary_oarchive.hpp>
#include <boost/compute.hpp>
#include <boost/compute/core.hpp>
#include <boost/filesystem.hpp>
#include <boost/gil.hpp>
#include <boost/gil/extension/io/targa.hpp>
#include <boost/lambda2.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/program_options.hpp>

#include <string>



namespace {
	boost::program_options::options_description Options("Options");
	boost::filesystem::path filepath;
	auto& desc = Options.add_options()
		("help", "produce help message")
		("file,f,o", boost::program_options::value(&filepath), "Load task description")
		;
	boost::program_options::variables_map vm;

	DECL_VA(i);
	auto BuildFormula(auto w, auto h, auto r, auto g, auto b)
	{
		auto N = w * h;
		std::cout << "For i=[0;" << N << ") pattern f(i):" << std::endl;

		Valuable::vars_cont_t vars = {
			{"w"_va, w},
			{"h"_va, h},
			{"x"_va, i % w},
			{"y"_va, (i - (i % w)) / w},
			{"N"_va, N}
		};
		r.eval(vars);
		g.eval(vars);
		b.eval(vars);
		constexpr auto alpha = 0xff;
		auto f =  ((g.And(8, 0xff) + (alpha << 8)).shl(8) + r.And(8, 0xff)).shl(8) + b.And(8, 0xff);
		auto s = f.str();
		return gen::BuildFormula(s, N);
	}
}
int main()
{
	auto width = 0, height = 0;
	std::cout << "Image size (pixels):" << std::endl
		<< " width="; std::cin >> width;
	std::cout << " height="; std::cin >> height;
	std::cout << std::endl;

	std::string str;
	std::cout << "Image pattern (formulas for channels of width, height, x, y):" << std::endl
		<< " Red(w,h,x,y)="; std::cin >> str;
	auto varhost = i.getVaHost();
	Valuable red(str, varhost);
	std::cout << std::endl << " Green(w,h,x,y)="; std::cin >> str;
	Valuable green(str, varhost);
	std::cout << std::endl << " Blue(w,h,x,y)="; std::cin >> str;
	Valuable blue(str, varhost);
    std::cout << std::endl;


	auto y = BuildFormula(width, height, red, green, blue);
    std::cout << "Y = " << y << std::endl;
    auto openCLcode = y.OpenCLuint();
    std::cout << "\nGenerated OpenCL code:\n" << openCLcode << std::endl;

	auto N = width * height;
	boost::gil::rgba8_image_t img(width, height);
	auto v = view(img);
	assert(v.is_1d_traversable());
	auto p = v.begin().x();
	gen::Generate(y, (uint32_t*)p, N);

	boost::gil::write_view("wow.tga", v, boost::gil::targa_tag());

    return 0;
}
