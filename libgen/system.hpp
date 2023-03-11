#pragma once

#include "skrypt.h"

#include <boost/gil.hpp>

namespace gen {

    class PatternRelationSystem
        : public ::skrypt::Skrypt 
    {
        using base = ::skrypt::Skrypt;

    public:
        DECL_VA(x); DECL_VA(y);
        DECL_VA(w); DECL_VA(width);
        DECL_VA(h); DECL_VA(height);
        DECL_VA(r); DECL_VA(red);
        DECL_VA(g); DECL_VA(green);
        DECL_VA(b); DECL_VA(blue);
        DECL_VA(a); DECL_VA(alpha);	

		PatternRelationSystem();

		boost::gil::rgba8_image_t GeneratePatternImage();
    };

}