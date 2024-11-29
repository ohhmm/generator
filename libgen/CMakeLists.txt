
if(MSVC)
	option(PNG_SHARED "Build shared lib" OFF)
endif()
option(PNG_STATIC "Build static lib" ON)

set(deplist 
	libskrypt

    Boost::filesystem
    Boost::program_options
    Boost::serialization
	)
if(NOT OPENMIND_USE_VCPKG)
	set(deplist ${deplist}
		libjpeg-turbo/libjpeg-turbo
		libjpeg-turbo::turbojpeg-static)
endif()

if(NOT PNG_FOUND)
	find_package(PNG)
	if(NOT PNG_FOUND)
		set(deplist ${deplist}
			madler/ZLIB
			glennrp/libPNG
			PNG::PNG_STATIC
			)
	endif()
endif()
IF(PNG_FOUND)
	set(deplist ${deplist} PNG)
endif()
if(TARGET Boost::program_options)
	set(deplist ${deplist} Boost::program_options)
elseif(Boost_PROGRAM_OPTIONS_LIBRARY)
	set(deplist ${deplist} ${Boost_PROGRAM_OPTIONS_LIBRARY})
else()
	set(deplist ${deplist} boost)
endif()

lib(${deplist})

if(OPENMIND_USE_VCPKG)
	find_package(libjpeg-turbo CONFIG REQUIRED)
	target_link_libraries(${this_target} PUBLIC
		$<IF:$<TARGET_EXISTS:libjpeg-turbo::turbojpeg-static>,libjpeg-turbo::turbojpeg-static,libjpeg-turbo::turbojpeg>
	)
endif()
