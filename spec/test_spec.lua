describe("Test", function()
    local cjson = require("cjson")
    local ErrorCodes = {
        NONE                 = 0,
        INVALID_METHOD       = -1,
        INVALID_ROUTE        = -2,
        INVALID_PATH_PARAM   = -3,
        INVALID_QUERY_PARAM  = -4,
        INVALID_HEADER_PARAM = -5,
        INVALID_BODY         = -6,
        INVALID_RSP          = -7
    }

    describe("oasvalidator", function()
        local oasvalidator = require("oasvalidator")

        it("loaded", function()
            assert.is_not_nil(oasvalidator)
        end)

        it("version", function()
            assert.is_not_nil(oasvalidator._VERSION)
        end)

        it("GetValidators", function()
            local validators, err = oasvalidator.GetValidators("data/openAPI_example.json")
            assert.is_nil(err)
            assert.is_not_nil(validators)
        end)

        local validators, err = oasvalidator.GetValidators("data/openAPI_example.json")

        it("InvalidMethod", function()
            local err_code, err_msg = validators:ValidateRoute("GETT", "/test/dummy")
            assert.is_not_nil(err_code)
            assert.is_not_nil(err_msg)
            assert.are.equal(err_code, ErrorCodes.INVALID_METHOD)
            local json = cjson.decode(err_msg)
            assert.are.equal("INVALID_METHOD", json["errorCode"])
        end)

        it("InvalidRoute", function()
            local err_code, err_msg = validators:ValidateRoute("GET", "/invalid/route")
            assert.is_not_nil(err_code)
            assert.is_not_nil(err_msg)
            assert.are.equal(err_code, ErrorCodes.INVALID_ROUTE)
            local json = cjson.decode(err_msg)
            assert.are.equal("INVALID_ROUTE", json["errorCode"])
        end)

        it("InvalidPathParam", function()
            local err_code, err_msg = validators:ValidatePathParam("GET", "/test/integer_simple_true/1001")
            assert.is_not_nil(err_code)
            assert.is_not_nil(err_msg)
            assert.are.equal(err_code, ErrorCodes.INVALID_PATH_PARAM)
            local json = cjson.decode(err_msg)
            assert.are.equal("INVALID_PATH_PARAM", json["errorCode"])
        end)

        it("InvalidQueryParam", function()
            local err_code, err_msg = validators:ValidateQueryParam("GET", "/test/query_integer_form_true?param=not_an_integer")
            assert.is_not_nil(err_code)
            assert.is_not_nil(err_msg)
            assert.are.equal(err_code, ErrorCodes.INVALID_QUERY_PARAM)
            local json = cjson.decode(err_msg)
            assert.are.equal("INVALID_QUERY_PARAM", json["errorCode"])
        end)

        it("InvalidHeaderParam", function()
            local headers = {
                ["intHeader"] = "not_an_integer"
            }
            local err_code, err_msg = validators:ValidateHeaders("GET", "/test/header_single1", headers)
            assert.is_not_nil(err_code)
            assert.is_not_nil(err_msg)
            assert.are.equal(err_code, ErrorCodes.INVALID_HEADER_PARAM)
            local json = cjson.decode(err_msg)
            assert.are.equal("INVALID_HEADER_PARAM", json["errorCode"])
        end)

        it("ValidRequest: Method, Path", function()
            local err_code, err_msg = validators:ValidateRequest("GET", "/test/query_integer_form_true?param=123")
            assert.is_not_nil(err_code)
            assert.is_nil(err_msg)
            assert.are.equal(err_code, ErrorCodes.NONE)
        end)

        it("InvalidRequest: Method, Path", function()
            local err_code, err_msg = validators:ValidateRequest("GET", "/test/query_integer_form_true?param=not_an_integer")
            assert.is_not_nil(err_code)
            assert.is_not_nil(err_msg)
            assert.are.equal(err_code, ErrorCodes.INVALID_QUERY_PARAM)
            local json = cjson.decode(err_msg)
            assert.are.equal("INVALID_QUERY_PARAM", json["errorCode"])
        end)

        it("ValidRequest: Method, Path, Body", function()
            local err_code, err_msg = validators:ValidateRequest("POST", "/test/body_scenario4", "{\"field1\":123,\"field2\":\"str\"}")
            assert.is_not_nil(err_code)
            assert.is_nil(err_msg)
            assert.are.equal(err_code, ErrorCodes.NONE)
        end)

        it("InvalidRequest: Method, Path, Body", function()
            local err_code, err_msg = validators:ValidateRequest("POST", "/test/body_scenario4", "{\"field1\":123,\"field2\":123}")
            assert.is_not_nil(err_code)
            assert.is_not_nil(err_msg)
            assert.are.equal(err_code, ErrorCodes.INVALID_BODY)
            local json = cjson.decode(err_msg)
            assert.are.equal("INVALID_BODY", json["errorCode"])
        end)

        it("ValidRequest: Method, Path, Body, Headers", function()
            local headers = {
                ["param11"] = "true"
            }
            local err_code, err_msg = validators:ValidateRequest("POST", "/test/all/123/abc/str1,str2/field1,0,field2,string?param4=string1&param4=string2&param5=field1,0,field2,string&param6=field1,0,field2,string&param7=field1,0,field2,string&param8=field1,0,field2,string&param9=field1,0,field2,string&param10=false"
            ,"{\"field1\":123,\"field2\":\"abc\",\"field3\":[\"abc\",\"def\"],\"field4\":{\"subfield1\":123,\"subfield2\":\"abc\"},\"field5\":{\"subfield1\":123},\"field6\":123,\"field7\":[123,456],\"field8\":[123,456],\"field9\":\"abc\",\"field10\":\"option1\",\"field11\":{\"field\":123},\"field12\":[{\"name\":\"abc\"},{\"name\":\"def\"}]}"
            ,headers)
            assert.is_not_nil(err_code)
            assert.is_nil(err_msg)
            assert.are.equal(err_code, ErrorCodes.NONE)
        end)

        it("InvalidBody", function()
            local headers = {
                ["intHeader"] = "not_an_integer"
            }
            local err_code, err_msg = validators:ValidateRequest("POST", "/test/all/123/abc/str1,str2/field1,0,field2,string?param4=string1&param4=string2&param5=field1,0,field2,string&param6=field1,0,field2,string&param7=field1,0,field2,string&param8=field1,0,field2,string&param9=field1,0,field2,string&param10=false",
                    "{\"field1\":123,\"field2\":\"abc\",\"field3\":[\"abc\",\"def\"],\"field4\":{\"subfield1\":123,\"subfield2\":\"abc\"},\"field5\":{\"subfield1\":123},\"field6\":true,\"field7\":[123,456],\"field8\":[123,456],\"field9\":\"abc\",\"field10\":\"option1\",\"field11\":{\"field\":123},\"field12\":[{\"name\":\"abc\"},{\"name\":\"def\"}]}",
                    headers)
            assert.is_not_nil(err_code)
            assert.is_not_nil(err_msg)
            assert.are.equal(err_code, ErrorCodes.INVALID_BODY)
            local json = cjson.decode(err_msg)
            assert.are.equal("INVALID_BODY", json["errorCode"])
        end)

    end)
end)