
cmake_minimum_required (VERSION 3.8)

# Select C++ active standard
enable_language(CXX)
set (CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(OPENMIND_DEBUG_CHECKS "Additional debug checks" OFF)
set(OPENMIND_USE_OPENCL ON CACHE BOOL "OpenCL GPU calculations" FORCE)
option(OPENMIND_MATH_USE_LEVELDB_CACHE "Cache calculation results with LevelDB" ON)

set(OPENMIND_VERSION_TAG main CACHE STRING "OpenMind library version to use")
set(OPENMIND_VERSION_TAG generator CACHE STRING "OpenMind library version to use")

# Tell OpenMind to use any boost version installed in the system if any
find_package(Boost)
if(Boost_FOUND)
    set(OPENMIND_REQUIRED_BOOST_VERSION ${Boost_VERSION} CACHE STRING "Boost library version to use")
endif()
set(BOOST_ADDITIONAL_COMPONENTS program_options CACHE STRING "Boost components")

# Find some Git
find_package(Git QUIET)
if(NOT GIT_EXECUTABLE)
    find_program(GIT_EXECUTABLE git PATHS
        "$ENV{ProgramFiles}/Microsoft Visual Studio/*/*/Common7/IDE/CommonExtensions/Microsoft/TeamFoundation/Team Explorer/Git/cmd/"
        )
    find_package(Git)
endif()

# Fetch some OpenMind
include(FetchContent)
FetchContent_Declare(openmind
    GIT_REPOSITORY https://github.com/ohhmm/openmind.git
    GIT_TAG ${OPENMIND_VERSION_TAG})
FetchContent_MakeAvailable(openmind)

# Include sub-projects, base component 'libgen' goes first.
fold(
	libgen
	)
