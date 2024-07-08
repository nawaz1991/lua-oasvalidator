--[[
  Copyright (c) 2024 Muhammad Nawaz
  Licensed under the MIT License. See LICENSE file for more information.
--]]

local oasvalidator = require("oasvalidator")
local socket = require("socket")

local its = 2000000

-- Initialize validators once
local validators, err = oasvalidator.GetValidators("data/openAPI_example.json")
if err ~= nil then
    print("Error loading validators: ", err)
    return
end

local function benchmark(method, path, body, headers)
    body = body or ""
    headers = headers or {}

    -- Warmup
    for i = 1, 10000 do
        validators:ValidateRequest(method, path, body, headers)
    end

    local start = socket.gettime()
    for i = 1, its do
        validators:ValidateRequest(method, path, body, headers)
    end
    local elapsed = socket.gettime() - start
    print(string.format("Method: %s, Path: %s, Elapsed time: %.2f us", method, path, elapsed/its*1000000))
end

benchmark("GETT", "/test/dummy", "", {})
benchmark("GET", "/invalid/route", "", {})
benchmark("GET", "/test/integer_simple_true/1001", "", {})
benchmark("GET", "/test/query_integer_form_true?param=not_an_integer", "", {})
benchmark("GET", "/test/header_single1", "", {
    ["intHeader"] = "not_an_integer"
})
benchmark("GET", "/test/query_two_integer_form_mixed?param1=123&param2=6", "", {
    ["intHeader"] = "123",
    ["strHeader"] = "abc",
})