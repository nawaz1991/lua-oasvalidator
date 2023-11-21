/*
 * Copyright (c) 2023 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */

#include "lua_oasvalidator_imp.hpp"

#ifndef LUA_OASVALIDATOR_VERSION
#define LUA_OASVALIDATOR_VERSION "1.0.0"
#endif

static const luaL_Reg oas_validator_methods[] = {{"GetValidators", lua_GetValidators},
                                                 {nullptr, nullptr}};

static const luaL_Reg oas_validator_imp_methods[] = {{"ValidateRoute", lua_ValidateRoute},
                                                     {"ValidateBody", lua_ValidateBody},
                                                     {"ValidatePathParam", lua_ValidatePathParam},
                                                     {"ValidateQueryParam", lua_ValidateQueryParam},
                                                     {"ValidateHeaders", lua_ValidateHeaders},
                                                     {"ValidateRequest", lua_ValidateRequest},

                                                     {"__gc", metamethod_gc},
                                                     {"__tostring", metamethod_tostring},

                                                     {nullptr, nullptr}};

extern "C"
{
    LUALIB_API int luaopen_oasvalidator(lua_State* L)
    {
        // Create the oasvalidator table
        lua_newtable(L);

        // Register methods to the table
        luaL_register(L, NULL, oas_validator_methods);

        // Set meta attributes
        lua_pushliteral(L, "oasvalidator");
        lua_setfield(L, -2, "_NAME");
        lua_pushliteral(L, LUA_OASVALIDATOR_VERSION);
        lua_setfield(L, -2, "_VERSION");

        // Set metatable for OASValidatorImp
        luaL_newmetatable(L, METATABLE_NAME);
        lua_pushvalue(L, -1);
        luaL_register(L, NULL, oas_validator_imp_methods);
        lua_setfield(L, -2, "__index");
        lua_pop(L, 1);

        return 1;
    }
}
