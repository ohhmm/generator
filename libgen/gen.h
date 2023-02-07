#include <boost/gil.hpp>
#include <omnn/math/Variable.h>


namespace boost::program_options {
	class variables_map;
}

namespace gen {
	const boost::program_options::variables_map& Init(int argc, char** argv); // optional, for options

	const omnn::math::Valuable::va_names_t& InitialVarNames();
	omnn::math::Valuable BuildFormula(const std::string& s, const omnn::math::Valuable::va_names_t& varNames = InitialVarNames());
	omnn::math::Valuable BuildFormula(const std::string& s, const omnn::math::Valuable& n, const omnn::math::Valuable::va_names_t& varNames = InitialVarNames());
	omnn::math::Valuable BuildFormula(const std::string& s, const omnn::math::Valuable::vars_cont_t& knownValues, const omnn::math::Valuable::va_names_t& varNames = InitialVarNames());
	omnn::math::Valuable BuildFormulaAGRBofWidthHeightXYiN(uint32_t w, uint32_t h, omnn::math::Valuable r, omnn::math::Valuable g, omnn::math::Valuable b);

	bool Generate(const omnn::math::Valuable& pattern, float* data, unsigned n);
	bool Generate(const omnn::math::Valuable& pattern, uint32_t* data, unsigned n);
	
	boost::gil::rgba8_image_t GeneratePatternImage(uint32_t width, uint32_t height, omnn::math::Valuable red, omnn::math::Valuable green, omnn::math::Valuable blue);
}
