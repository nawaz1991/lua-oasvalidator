cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)

project(lua-oasvalidator)

# compile in release  mode by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

if(NOT LUA_OASVALIDATOR_VERSION)
    find_package(Git)
    if(GIT_FOUND)
        execute_process(COMMAND ${GIT_EXECUTABLE} describe --dirty
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            OUTPUT_VARIABLE LUA_OASVALIDATOR_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    endif()
endif()

if (LUA_OASVALIDATOR_VERSION)
    message("-- LUA_OASVALIDATOR_VERSION: ${LUA_OASVALIDATOR_VERSION}")
    add_definitions(-DLUA_OASVALIDATOR_VERSION="${LUA_OASVALIDATOR_VERSION}")
endif()

if(UNIX)
    if(APPLE)
        set(PLAT "macosx")
        set(LINK_FLAGS "-bundle -undefined dynamic_lookup -all_load")
    else(APPLE)
        set(PLAT "linux")
        set(LINK_FLAGS "-shared")
    endif(APPLE)
    add_compile_options(-g -Wall -fPIC)
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_ARCH_NATIVE)
    if (COMPILER_SUPPORTS_ARCH_NATIVE)
        add_compile_options(-march=native)
    endif()
else(UNIX)
    if(WIN32)
        set(PLAT "win32")
        add_definitions(-DLUA_BUILD_AS_DLL -DLUA_LIB -D_CRT_SECURE_NO_WARNINGS)
    else(WIN32)
        set(PLAT "unsupported")
    endif(WIN32)
endif(UNIX)

if(NOT LUA_INCLUDE_DIR OR (WIN32 AND NOT LUA_LIBRARIES))
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
    find_package(Lua REQUIRED)
endif()

if (NOT RAPIDJSON_INCLUDE_DIRS)
    set(RAPIDJSON_INCLUDE_DIRS rapidjson/include)
endif()

include_directories(${LUA_INCLUDE_DIR} ${RAPIDJSON_INCLUDE_DIRS})
include_directories(${LUA_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/include")


set(SOURCES
    src/Document.cpp
    src/Schema.cpp
    src/rapidjson.cpp
    src/values.cpp
        src/OASValidator.cpp
        src/LUA_OASValidator.cpp
        src/json_parser.cpp
)

add_library(lua-oasvalidator MODULE ${SOURCES})
if(${CMAKE_VERSION} VERSION_LESS "3.1.0")
    if(CMAKE_COMPILER_IS_GNUCXX)
        execute_process(COMMAND "${CMAKE_CXX_COMPILER} -dumpversion" OUTPUT_VARIABLE GCC_VERSION)
        if (GCC_VERSION VERSION_LESS 4.7)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        endif()
    endif()
else()
    target_compile_features(lua-oasvalidator PRIVATE cxx_rvalue_references)
endif()

source_group(src FILES ${SOURCES})

if (WIN32)
    target_link_libraries(lua-oasvalidator ${LUA_LIBRARIES})
endif()

if (LINK_FLAGS)
    set_target_properties(lua-oasvalidator PROPERTIES
        LINK_FLAGS ${LINK_FLAGS}
    )
endif()

set_target_properties(lua-oasvalidator PROPERTIES
    OUTPUT_NAME "oasvalidator"
    PREFIX ""
)


if (CMAKE_INSTALL_PREFIX)
  install(TARGETS lua-oasvalidator DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
endif(CMAKE_INSTALL_PREFIX)
