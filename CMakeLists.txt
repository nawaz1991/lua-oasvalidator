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
    set(RAPIDJSON_INCLUDE_DIRS cpp-oasvalidator/thirdparty/rapidjson/include)
endif()

include_directories(${LUA_INCLUDE_DIR} ${RAPIDJSON_INCLUDE_DIRS})
include_directories(${LUA_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/cpp-oasvalidator/include")
include_directories(${LUA_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/include")


set(SOURCES
        src/lua_oasvalidator.cpp
        src/lua_oasvalidator_imp.cpp

        cpp-oasvalidator/src/oas_validator_imp.cpp
        cpp-oasvalidator/src/deserializers/array_deserializer.cpp
        cpp-oasvalidator/src/deserializers/base_deserializer.cpp
        cpp-oasvalidator/src/deserializers/content_deserializer.cpp
        cpp-oasvalidator/src/deserializers/object_deserializer.cpp
        cpp-oasvalidator/src/deserializers/primitive_deserializer.cpp
        cpp-oasvalidator/src/utils/path_trie.cpp
        cpp-oasvalidator/src/validators/validators_store.cpp
        cpp-oasvalidator/src/validators/param_validators.cpp
        cpp-oasvalidator/src/validators/method_validator.cpp
        cpp-oasvalidator/src/validators/json_validator.cpp
        cpp-oasvalidator/src/validators/base_validator.cpp
)

add_library(${PROJECT_NAME} MODULE ${SOURCES})
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
    target_compile_features(${PROJECT_NAME} PRIVATE cxx_rvalue_references)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -ffast-math -funroll-loops")

source_group(src FILES ${SOURCES})

if (WIN32)
    target_link_libraries(${PROJECT_NAME} ${LUA_LIBRARIES})
endif()

if (LINK_FLAGS)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS ${LINK_FLAGS}
    )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "oasvalidator"
    PREFIX ""
)


if (CMAKE_INSTALL_PREFIX)
  install(TARGETS ${PROJECT_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")
endif(CMAKE_INSTALL_PREFIX)
