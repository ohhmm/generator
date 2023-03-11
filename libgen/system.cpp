#include "system.hpp"

using namespace gen;


PatternRelationSystem::PatternRelationSystem()
    : base()
{
    Add(w, width);
    Add(h, height);
    Add(r, red);
    Add(g, green);
    Add(b, blue);
    Add(a, alpha);
}

boost::gil::rgba8_image_t PatternRelationSystem::GeneratePatternImage() {
    boost::gil::rgba8_image_t img;
    auto widthValue = Solve(w);
    return img;
}
