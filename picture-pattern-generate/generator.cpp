
#include "gen.h"

#include <omnn/math/Variable.h>
#include <omnn/math/VarHost.h>
using namespace omnn::math;

#include <boost/archive/binary_oarchive.hpp>
#include <boost/compute.hpp>
#include <boost/compute/core.hpp>
#include <boost/filesystem.hpp>
#include <boost/gil.hpp>
#include <boost/gil/extension/io/bmp.hpp>
#include <boost/gil/extension/io/targa.hpp>
#include <boost/gil/extension/io/png.hpp>
// #include <boost/gil/extension/io/jpeg.hpp>

#include <boost/lambda2.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/program_options.hpp>

#include <string>


int main(int argc, char** argv)
{
    auto& programOptionValues = gen::Init(argc, argv);
	auto& backgroundColorOption = programOptionValues["background-color"];
	auto& filePathOption = programOptionValues["file"];
	auto filepath = filePathOption.as<boost::filesystem::path>();
	std::cout << "BG: " << backgroundColorOption.as<uint32_t>() << std::endl;

	auto width = 0, height = 0;
	std::cout << "Image size (pixels):" << std::endl
		<< " width="; std::cin >> width;
	std::cout << " height="; std::cin >> height;
	std::cout << std::endl;

	std::string str;
	std::cout << "Image pattern (formulas for channels of width, height, x, y):" << std::endl
		<< " Red(w,h,x,y)="; std::cin >> str;
	auto& i = gen::InitialVarNames().begin()->second;
	auto varhost = i.getVaHost();
	Valuable red(str, varhost);
	std::cout << std::endl << " Green(w,h,x,y)="; std::cin >> str;
	Valuable green(str, varhost);
	std::cout << std::endl << " Blue(w,h,x,y)="; std::cin >> str;
	Valuable blue(str, varhost);
    std::cout << std::endl;

	auto img = gen::GeneratePatternImage(width, height, red, green, blue);
	auto v = boost::gil::view(img);

	auto ext = filepath.extension();
	if(ext == ".bmp"){
		boost::gil::write_view(filepath.c_str(), v, boost::gil::bmp_tag());
		std::cout << filepath << " written" << std::endl;
	// } else if(ext == ".png") {
	// 	boost::gil::write_view(filepath.c_str(), v, boost::gil::png_tag());
	// 	std::cout << filepath << " written" << std::endl;
	// } else if(ext == ".jpg" || ext == ".jpeg") {
	// 	boost::gil::write_view(filepath.c_str(), v, boost::gil::jpeg_tag());
	// 	std::cout << filepath << " written" << std::endl;
	} else if(ext != ".tga") {
		std::cout << ext << " extension is not supported, using TGA instead" << std::endl;
		filepath.replace_extension(".tga");
	}
	if(ext == ".tga"){
		boost::gil::write_view(filepath.c_str(), v, boost::gil::targa_tag());
		std::cout << filepath << " written" << std::endl;
	}

    return 0;
}
