/*
 * Copyright (c) 2024 Muhammad Nawaz
 * Licensed under the MIT License. See LICENSE file for more information.
 */
// [ END OF LICENSE c6bd0f49d040fca8d8a9cb05868e66aa63f0e2e0 ]

#include "lua_oasvalidator_imp.hpp"

static inline OASValidatorImp* LuaCheckInstance(lua_State* L)
{
    OASValidatorImp** ud = reinterpret_cast<OASValidatorImp**>(luaL_checkudata(L, 1, METATABLE_NAME));
    if (!*ud) {
        luaL_error(L, "Instance of %s already closed", METATABLE_NAME);
    }
    return *ud;
}

// Helper function to convert Lua table to C++ unordered_map
static inline void LuaTableToUnorderedMap(lua_State* L, int index, std::unordered_map<std::string, std::string>& headers)
{
    lua_pushnil(L);
    while (lua_next(L, index) != 0) {
        const char* key = luaL_checkstring(L, -2);
        const char* value = luaL_checkstring(L, -1);
        headers.emplace(key, value);
        lua_pop(L, 1);
    }
}

// Helper function to convert Lua table to C++ unordered_map with unordered_set as values
static inline void LuaTableToUnorderedMapSet(lua_State* L, int index, std::unordered_map<std::string, std::unordered_set<std::string>>& headers) {
    lua_pushnil(L);  // First key
    while (lua_next(L, index) != 0) {
        const char* key = luaL_checkstring(L, -2);
        luaL_checktype(L, -1, LUA_TTABLE);  // Ensure the value is a table

        std::unordered_set<std::string> values;
        int subtable_index = lua_gettop(L);

        lua_pushnil(L);  // First element of the subtable
        while (lua_next(L, subtable_index) != 0) {
            const char* value = luaL_checkstring(L, -1);
            values.insert(value);
            lua_pop(L, 1);
        }

        headers.emplace(key, values);
        lua_pop(L, 1);  // Remove the value, keep the key for the next iteration
    }
}

int lua_GetValidators(lua_State* L)
{
    int num_args = lua_gettop(L);
    try {
        const char* oas_specs = luaL_checkstring(L, 1);
        std::unordered_map<std::string, std::unordered_set<std::string>> method_map = {};
        if (num_args == 2) {
            LuaTableToUnorderedMapSet(L, 2, method_map);
        }
        auto* oas_validators_imp = new OASValidatorImp(oas_specs, method_map);
        if (oas_validators_imp) {
            OASValidatorImp** ud = reinterpret_cast<OASValidatorImp**>(lua_newuserdata(L, sizeof(*ud)));
            if (!ud) {
                delete oas_validators_imp;
                luaL_error(L, "Out of memory");
            }
            *ud = oas_validators_imp;
            luaL_getmetatable(L, METATABLE_NAME);
            lua_setmetatable(L, -2);

        } else {
            lua_pushnil(L);
        }
    } catch (const ValidatorInitExc& ex) {
        lua_pushnil(L);
        luaL_error(L, ex.what());
    }
    return 1;
}

int metamethod_gc(lua_State* L)
{
    OASValidatorImp** ud = reinterpret_cast<OASValidatorImp**>(luaL_checkudata(L, 1, METATABLE_NAME));
    if (*ud) {
        delete *ud;
        *ud = nullptr;
    }
    return 0;
}

int metamethod_tostring(lua_State* L)
{
    auto** ud = reinterpret_cast<OASValidatorImp**>(lua_touserdata(L, 1));
    if (*ud) {
        lua_pushfstring(L, "%s (%p)", METATABLE_NAME, *ud);
    } else {
        lua_pushfstring(L, "%s (closed)", METATABLE_NAME);
    }
    return 1;
}

int lua_ValidateRoute(lua_State* L)
{
    OASValidatorImp* oas_validator_imp = LuaCheckInstance(L);
    const char* method = luaL_checkstring(L, 2);
    const char* path = luaL_checkstring(L, 3);
    std::string error_msg;
    auto err_code = oas_validator_imp->ValidateRoute(method, path, error_msg);
    lua_pushinteger(L, static_cast<lua_Integer>(err_code));
    if (ValidationError::NONE == err_code) {
        return 1;
    } else {
        lua_pushstring(L, error_msg.c_str());
        return 2;
    }
}

int lua_ValidateBody(lua_State* L)
{
    OASValidatorImp* oas_validator_imp = LuaCheckInstance(L);
    const char* method = luaL_checkstring(L, 2);
    const char* path = luaL_checkstring(L, 3);
    const char* json_body = luaL_checkstring(L, 4);
    std::string error_msg;
    auto err_code = oas_validator_imp->ValidateBody(method, path, json_body, error_msg);
    lua_pushinteger(L, static_cast<lua_Integer>(err_code));
    if (ValidationError::NONE == err_code) {
        return 1;
    } else {
        lua_pushstring(L, error_msg.c_str());
        return 2;
    }
}

int lua_ValidatePathParam(lua_State* L)
{
    OASValidatorImp* oas_validator_imp = LuaCheckInstance(L);
    const char* method = luaL_checkstring(L, 2);
    const char* path = luaL_checkstring(L, 3);
    std::string error_msg;
    auto err_code = oas_validator_imp->ValidatePathParam(method, path, error_msg);
    lua_pushinteger(L, static_cast<lua_Integer>(err_code));
    if (ValidationError::NONE == err_code) {
        return 1;
    } else {
        lua_pushstring(L, error_msg.c_str());
        return 2;
    }
}

int lua_ValidateQueryParam(lua_State* L)
{
    OASValidatorImp* oas_validator_imp = LuaCheckInstance(L);
    const char* method = luaL_checkstring(L, 2);
    const char* path = luaL_checkstring(L, 3);
    std::string error_msg;
    auto err_code = oas_validator_imp->ValidateQueryParam(method, path, error_msg);
    lua_pushinteger(L, static_cast<lua_Integer>(err_code));
    if (ValidationError::NONE == err_code) {
        return 1;
    } else {
        lua_pushstring(L, error_msg.c_str());
        return 2;
    }
}

int lua_ValidateHeaders(lua_State* L)
{
    OASValidatorImp* oas_validator_imp = LuaCheckInstance(L);
    const char* method = luaL_checkstring(L, 2);
    const char* path = luaL_checkstring(L, 3);
    luaL_checktype(L, 4, LUA_TTABLE);
    std::unordered_map<std::string, std::string> headers;
    LuaTableToUnorderedMap(L, 4, headers);
    std::string error_msg;
    auto err_code = oas_validator_imp->ValidateHeaders(method, path, headers, error_msg);
    lua_pushinteger(L, static_cast<lua_Integer>(err_code));
    if (ValidationError::NONE == err_code) {
        return 1;
    } else {
        lua_pushstring(L, error_msg.c_str());
        return 2;
    }
}

int lua_ValidateRequest(lua_State* L)
{
    OASValidatorImp* oas_validator_imp = LuaCheckInstance(L);
    const char* method = luaL_checkstring(L, 2);
    const char* http_path = luaL_checkstring(L, 3);
    std::string error_msg;
    ValidationError err_code;

    int num_args = lua_gettop(L);
    if (num_args == 4 && lua_isstring(L, 4)) {
        const char* json_body = luaL_checkstring(L, 4);
        err_code = oas_validator_imp->ValidateRequest(method, http_path, json_body, error_msg);
    } else if (num_args == 4 && lua_istable(L, 4)) {
        std::unordered_map<std::string, std::string> headers;
        LuaTableToUnorderedMap(L, 4, headers);
        err_code = oas_validator_imp->ValidateRequest(method, http_path, headers, error_msg);
    } else if (num_args == 5) {
        const char* json_body = luaL_checkstring(L, 4);
        std::unordered_map<std::string, std::string> headers;
        LuaTableToUnorderedMap(L, 5, headers);
        err_code = oas_validator_imp->ValidateRequest(method, http_path, json_body, headers, error_msg);
    } else {
        err_code = oas_validator_imp->ValidateRequest(method, http_path, error_msg);
    }

    lua_pushinteger(L, static_cast<lua_Integer>(err_code));
    if (ValidationError::NONE == err_code) {
        return 1;
    } else {
        lua_pushstring(L, error_msg.c_str());
        return 2;
    }
}
