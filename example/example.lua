--[[
  Copyright (c) 2024 Muhammad Nawaz
  Licensed under the MIT License. See LICENSE file for more information.
--]]

oasvalidator = require("oasvalidator")

print(oasvalidator._VERSION)

http_method_mappings = {
    HEAD = { "GET" } -- Treat `HEAD` requests as `GET` requests, if `HEAD` method is not defined for the path
}

validators, err = oasvalidator.GetValidators("data/openAPI_example.json", http_method_mappings)

err_code, err_msg = validators:ValidateRoute("GETT", "/test/dummy")
if (err_code ~= 0) then
    print(err_msg)
end

err_code, err_msg = validators:ValidateRoute("GET", "/test/dummy")
if err_code ~= 0 then
    print(err_msg)
end

err_code, err_msg = validators:ValidateRoute("head", "/test/dummy") -- lower case head
if err_code ~= 0 then
    print(err_msg)
end

err_code, err_msg = validators:ValidatePathParam("GET", "/test/integer_simple_true/1001")
if err_code ~= 0 then
    print(err_msg)
end

err_code, err_msg = validators:ValidatePathParam("GET", "/test/integer_simple_true/not_an_integer")
if err_code ~= 0 then
    print(err_msg)
end

err_code, err_msg = validators:ValidateQueryParam("GET", "/test/query_integer_form_true?param=not_an_integer")
if err_code ~= 0 then
    print(err_msg)
end

err_code, err_msg = validators:ValidateQueryParam("HEAD", "/test/query_integer_form_true?param=not_an_integer") -- upper case head
if err_code ~= 0 then
    print(err_msg)
end

headers = {
    ["intHeader"] = "not_an_integer"
}

err_code, err_msg = validators:ValidateHeaders("GET", "/test/header_single1", headers)
if err_code ~= 0 then
    print(err_msg)
end

err_code, err_msg = validators:ValidateRequest("POST", "/test/all/123/abc/str1,str2/field1,0,field2,string?param4=string1&param4=string2&param5=field1,0,field2,string&param6=field1,0,field2,string&param7=field1,0,field2,string&param8=field1,0,field2,string&param9=field1,0,field2,string&param10=false",
        "{\"field1\":123,\"field2\":\"abc\",\"field3\":[\"abc\",\"def\"],\"field4\":{\"subfield1\":123,\"subfield2\":\"abc\"},\"field5\":{\"subfield1\":123},\"field6\":true,\"field7\":[123,456],\"field8\":[123,456],\"field9\":\"abc\",\"field10\":\"option1\",\"field11\":{\"field\":123},\"field12\":[{\"name\":\"abc\"},{\"name\":\"def\"}]}",
        headers)
if err_code ~= 0 then
    print(err_msg)
end