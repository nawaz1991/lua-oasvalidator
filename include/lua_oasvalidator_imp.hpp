/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */

#ifndef LUA_OAS_VALIDATOR
#define LUA_OAS_VALIDATOR

#include <lua.hpp>

#include "oas_validator_imp.hpp"

#define METATABLE_NAME "oasvalidator.GetValidators"

// Initializes instance of OASValidatorImp
int lua_GetValidators(lua_State* L);

// garbage collector
int metamethod_gc(lua_State* L);

// __tostring
int metamethod_tostring(lua_State* L);

// LUA <-> C++ binding for OASValidatorImp::ValidateRoute
int lua_ValidateRoute(lua_State* L);

// LUA <-> C++ binding for OASValidatorImp::ValidateBody
int lua_ValidateBody(lua_State* L);

// LUA <-> C++ binding for OASValidatorImp::ValidatePathParam
int lua_ValidatePathParam(lua_State* L);

// LUA <-> C++ binding for OASValidatorImp::ValidateQueryParam
int lua_ValidateQueryParam(lua_State* L);

// LUA <-> C++ binding for OASValidatorImp::ValidateHeaders
int lua_ValidateHeaders(lua_State* L);

// LUA <-> C++ binding for OASValidatorImp::ValidateRequest (4 overloaded methods)
int lua_ValidateRequest(lua_State* L);

#endif