#include "OASValidator.hpp"

#include "Userdata.hpp"
#include "values.hpp"

template<>
const char* const Userdata<OASValidator>::metatable()
{
    return "oasvalidator.init";
}

template<>
OASValidator* Userdata<OASValidator>::construct(lua_State * L)
{
    int t = lua_type(L, 1);
    if (t != LUA_TSTRING ) { //TODO also take as table in case specs provided as JSON
        luax::typerror(L, 1, "string");
        return NULL;
    }

    const char* s = luaL_checkstring(L, 1);
    try {
        return new OASValidator(s);
    }catch (const OASValidatorException& oasValidatorException)
    {
        luaL_error(L, oasValidatorException.what());
        return NULL;
    }
}

static int validate_body(lua_State * L)
{
    OASValidator* oasValidator = Userdata<OASValidator>::get(L, 1);
    const char* path = luaL_checkstring(L, 2);
    luaL_checktype(L, 3, LUA_TTABLE);

    auto* body = new rapidjson::Document();
    values::toDocument(L, 3, body);
    ValueType error;
    bool valid = oasValidator->ValidateBody(path, *body, error);
    lua_pushboolean(L, valid);
    if(valid)
    {
        return 1;
    }
    else
    {
        values::pushValueType(L, error);
        return 2;
    }
}

template <>
const luaL_Reg* Userdata<OASValidator>::methods() {
    static const luaL_Reg reg[] = {
            { "__gc", metamethod_gc },
            { "validate_body", validate_body },
            { NULL, NULL }
    };
    return reg;
}