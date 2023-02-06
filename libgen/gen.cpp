
#include <omnn/math/Variable.h>
using namespace omnn::math;

#include <boost/archive/binary_oarchive.hpp>
#include <boost/compute.hpp>
#include <boost/compute/core.hpp>
#include <boost/lambda2.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/program_options.hpp>
#include <boost/serialization/vector.hpp>
#include "gen.h"


namespace {
	DECL_VA(i);
	DECL_VA(n);

	auto ComputeUnitsWinner = []() -> boost::compute::device {
		auto devices = boost::compute::system::devices();
		if (devices.size() == 0) {
			std::cout << "Please, install an OpenCL driver." << std::endl;
			exit(-1);
		}

		auto cuwinner = boost::compute::system::default_device();
		for (auto& p : boost::compute::system::platforms()) {
			for (auto& d : p.devices()) {
				std::cout << d.name() << ':' << d.compute_units() << std::endl;
				if (d.compute_units() > cuwinner.compute_units())
					cuwinner = d;
			}
		}

		std::cout << "max_work_group_size: " << cuwinner.max_work_group_size() << std::endl;

		return cuwinner;
	}();
}

namespace gen {

	const Valuable::va_names_t& InitialVarNames() {
		static Valuable::va_names_t Names = { {"i", i}, {"n", n} };
		return Names;
	}

	Valuable BuildFormula(const std::string& s, const Valuable::va_names_t& varNames) {
		auto names = varNames;
		names["i"] = i;
        names["n"] = n;
		Valuable pattern(s, names);
		return pattern;
	}

	Valuable BuildFormula(const std::string& s, const Valuable::vars_cont_t& knownValues, const Valuable::va_names_t& varNames) {
		auto formula = BuildFormula(s, varNames);
		if (formula.eval(knownValues)) {
			//formula.optimize();
		}
		return formula;
	}

	Valuable BuildFormula(const std::string& s, const Valuable& nValue, const Valuable::va_names_t& varNames) {
		auto formula = BuildFormula(s, varNames);
		formula.Eval(n, nValue);
		return formula;
	}


	bool Generate(const Valuable& pattern, float* data, unsigned wgsz) {
		// build OpenCL kernel
		boost::compute::context context(ComputeUnitsWinner);
		boost::compute::command_queue queue(context, ComputeUnitsWinner);
		boost::compute::kernel k(boost::compute::program::build_with_source(pattern.OpenCL(), context), "f");

		auto sz = wgsz * sizeof(cl_float);
		boost::compute::buffer ctx(context, sz);
		k.set_arg(0, ctx);

		// run the add kernel
		queue.enqueue_1d_range_kernel(k, 0, wgsz, 0);

		// transfer results to the host array 'c'
		queue.enqueue_read_buffer(ctx, 0, sz, data);
		queue.finish();

		return true;
	}

	bool Generate(const Valuable& pattern, uint32_t* data, unsigned wgsz) {
		// build OpenCL kernel
		boost::compute::context context(ComputeUnitsWinner);
		boost::compute::command_queue queue(context, ComputeUnitsWinner);
		boost::compute::kernel k(boost::compute::program::build_with_source(pattern.OpenCL(), context), "f");

		auto sz = wgsz * sizeof(cl_uint);
		boost::compute::buffer ctx(context, sz);
		k.set_arg(0, ctx);

		// run the add kernel
		queue.enqueue_1d_range_kernel(k, 0, wgsz, 0);

		// transfer results to the host array 'c'
		queue.enqueue_read_buffer(ctx, 0, sz, data);
		queue.finish();

		return true;
	}

	Valuable BuildFormulaAGRBofWidthHeightXYiN(uint32_t w, uint32_t h, Valuable r, Valuable g, Valuable b)
	{
		auto N = w * h;
		Valuable::vars_cont_t vars = {
			{"w"_va, w},
			{"h"_va, h},
			{"x"_va, i % w},
			{"y"_va, (i - (i % w)) / w},
			{n, N},
			{"N"_va, N},
		};
		r.eval(vars);
		g.eval(vars);
		b.eval(vars);
		constexpr auto alpha = 0xff;
		auto f =  ((g.And(8, 0xff) + (alpha << 8)).shl(8) + r.And(8, 0xff)).shl(8) + b.And(8, 0xff);
		auto s = f.str();
		return gen::BuildFormula(s, N);
	}

	boost::gil::rgba8_image_t GeneratePatternImage(uint32_t width, uint32_t height, Valuable red, Valuable green, Valuable blue)
	{
		auto y = gen::BuildFormulaAGRBofWidthHeightXYiN(width, height, red, green, blue);
		boost::gil::rgba8_image_t img(width, height);
		auto v = view(img);
		assert(v.is_1d_traversable());
		auto p = v.begin().x();
		gen::Generate(y, (uint32_t*)p, width * height);
		return img;
	}
}