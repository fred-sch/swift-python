cmake_minimum_required(VERSION 3.15...3.29)

# There are some incompatibilities with Swift and gcc
# Therefore we have to specify explicitly to use clang
# set(CMAKE_C_COMPILER "clang")
# set(CMAKE_CXX_COMPILER "clang")

##########
# Python #
##########
# Scikit-build-core sets these values for you, or you can just hard-code the name and version.
project(
  ${SKBUILD_PROJECT_NAME}
  VERSION ${SKBUILD_PROJECT_VERSION}
  LANGUAGES CXX)

# Find the module development requirements (requires FindPython from 3.17 or
# scikit-build-core's built-in backport)
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 CONFIG REQUIRED)

#########
# SWIFT #
#########
# Add a custom target to build the Swift project, this will always be built!
add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/Swift/.build/release/libswift-python.a
    COMMAND swift build -c release
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/Swift
    COMMENT "Building Swift static library"
    VERBATIM
)

# Create a custom target that depends on the output of the custom command
add_custom_target(BuildSwift ALL
    DEPENDS ${CMAKE_SOURCE_DIR}/Swift/.build/release/libswift-python.a
)
set_target_properties( PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(SwiftLib STATIC IMPORTED GLOBAL)
add_dependencies(SwiftLib BuildSwift)

# IMPORTED_LOCATION needs to be an absolute path
set_target_properties(
  SwiftLib
  PROPERTIES
  IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/Swift/.build/release/libswift-python.a
)

# Add a library using FindPython's tooling (pybind11 also provides a helper like this)
python_add_library(_core MODULE src/main.cpp WITH_SOABI)
add_dependencies(_core SwiftLib)
target_link_libraries(_core PRIVATE SwiftLib)
target_link_libraries(_core PRIVATE pybind11::headers)

install(TARGETS _core DESTINATION swiftedpy)
