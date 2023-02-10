if(MSVC)
	option(PNG_SHARED "Build shared lib" OFF)
endif()
option(PNG_STATIC "Build static lib" ON)

exe(libgen

	libjpeg-turbo/libjpeg-turbo
	libjpeg-turbo::turbojpeg-static

	madler/zlib

	glennrp/libpng
	PNG::PNG_STATIC
	)
