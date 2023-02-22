
if(MSVC)
	option(PNG_SHARED "Build shared lib" OFF)
endif()
option(PNG_STATIC "Build static lib" ON)

set(deplist 
	math

	libjpeg-turbo/libjpeg-turbo
	libjpeg-turbo::turbojpeg-static

	madler/zlib

	glennrp/libpng
	PNG::PNG_STATIC
	)

if(TARGET Boost::program_options)
	set(deplist ${deplist} Boost::program_options)
elseif(Boost_PROGRAM_OPTIONS_LIBRARY)
	set(deplist ${deplist} ${Boost_PROGRAM_OPTIONS_LIBRARY})
else()
	set(deplist ${deplist} boost)
endif()

lib(${deplist})
