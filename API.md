# lua-oasvalidator: API Reference
`lua-oasvalidator` offers a comprehensive API for validating HTTP requests in accordance with OpenAPI specifications. It encompasses various validation categories including *Path, Body, Parameters, and Response*.

The API functions return a an `error_code` of type `integer` and optionally `string` for having error messages in case of validation failure. A successful validation returns `0` as error code. The error message follows this JSON schema:

```json
{
  "errorCode": "INVALID_BODY",
  "detail": {
    "specRef": "#/paths/%2Fpet/put/requestBody/content/application%2Fjson/schema",
    "code": "type",
    "description": "The property type 'string' is not on the approved list: 'array'.",
    "instance": "#/photoUrls",
    "schema": "#/properties/photoUrls"
  }
}
```
- `errorCode`: Corresponds to the `ValidationError` enum value.
- `detail`: Provides a `specRef` pointing to the exact location in the OpenAPI spec where the request failed. This is URI-encoded to handle special characters.
  - Additional fields in `detail` offer context-specific insights such as the type of error and relevant references.

Following table categorize various types of validation errors:

```lua
NONE                 = 0
INVALID_METHOD       = -1
INVALID_ROUTE        = -2
INVALID_PATH_PARAM   = -3
INVALID_QUERY_PARAM  = -4
INVALID_HEADER_PARAM = -5
INVALID_BODY         = -6
INVALID_RSP          = -7
```

Designed with performance and optimization as priorities, `lua-oasvalidator` performs **lazy** deserialization and parsing of request components (path, query, header parameters, and body) only when all preceding validations pass.

Four overloaded methods are available to validate different combinations of request components (e.g., with/without body or headers) to accommodate various HTTP methods and use-cases.

The following API reference outlines each function and its sequence of validation checks.

## Table of Contents
1. [Constructor](#1-constructor-)
2. [Validate Route](#2-validate-route-)
3. [Validate Body](#3-validate-body-)
4. [Validate Path Parameters](#4-validate-path-parameters-)
5. [Validate Query Parameters](#5-validate-query-parameters-)
6. [Validate Header Parameters](#6-validate-header-parameters-)
7. [Validate Request](#7-validate-request-)
8. [Validate Request (Overloaded)](#8-validate-request-overloaded-)
9. [Validate Request (Overloaded)](#9-validate-request-overloaded-)
10. [Validate Request (Overloaded)](#10-validate-request-overloaded-)

### 1. Constructor 🏗️

Initializes an `OASValidator` object with the OpenAPI specification from the provided file path in Lua by invoking the `GetValidators` function and returns the validators.

#### Synopsis

```lua
local validators = oasvalidator.GetValidators(spec_path, method_map)
```

#### Arguments

- `spec_path`: The file path to the OpenAPI specification in `JSON` format as a Lua `string`.
- `method_map`: An optional table where each key is an HTTP method and the value is a table/list of methods that can be treated as the key method. This allows certain HTTP methods to be treated as others.

#### Example

```lua
local oasvalidator = require("oasvalidator")

method_map = {
  HEAD = { "GET" }, -- Treat `HEAD` requests as `GET` requests, if `HEAD` method is not defined for the path
  OPTIONS= {"GET", "POST", "PUT", "DELETE", "HEAD", "PATCH"} -- `OPTIONS` will be treated as other methods, of they are not defined for the path
}
local validators = oasvalidator.GetValidators("/path/to/openapi/spec.json", method_map) -- method_map is optional
```
#### Note

The OAS specification can be provided as a file path or as a JSON string. If the `method_map` is provided, it allows certain HTTP methods to be treated as others. For instance, with the mapping `HEAD = { "GET" }`, a HEAD request can be validated as the GET request, if HEAD method is not defined for the path.

#### Returns

This function returns a new instance of `validators`. In case of throws Lua error.

<div style="text-align: right">

[Table of Contents](#table-of-contents)

</div>

--- 

### 2. Validate Route 🗺️

Validates the HTTP method and path/route against the OpenAPI specification. In case of validation failure, an optional second return value provides error details.

#### Validation sequence
1. HTTP method
2. Route

#### Synopsis

```lua
local result, error_msg = validators:ValidateRoute(method, http_path)
```

#### Arguments
- `method`: The HTTP method (e.g., "GET", "POST") as a Lua `string`.
- `http_path`: The HTTP path (e.g., "/api/v1/resource") as a Lua `string`.

#### Returns
- `result`: An integer indicating the result of the validation. A return value of `0` signifies successful validation, whereas other values indicate specific types of validation failures.
- `error_msg`: Optional second return value, a JSON-formatted `string` containing details of the validation error.

#### Possible Error Codes (Integers)
- `NONE = 0`
- `INVALID_METHOD = -1`
- `INVALID_ROUTE = -2`

#### Example

```lua
local result, error_msg = validators:ValidateRoute("GET", "/api/v1/resource")
if result ~= 0 then
    print("Validation failed: " .. error_msg)
end
```

#### Notes
- The `error_msg` will be a JSON-formatted `string` in case of a validation error.

<div style="text-align: right">

[Table of Contents](#table-of-contents)

</div>

--- 

### 3. Validate Body 📦

Validates the `JSON` body of the HTTP request against the `JSON` schema defined in the OpenAPI specification for the specific `route`. In case of validation failure, an optional second return value provides error details.

#### Validation sequence
1. HTTP method
2. Route
3. Body schema

#### Synopsis

```lua
local result, error_msg = validators:ValidateBody(method, http_path, json_body)
```

#### Arguments
- `method`: The HTTP method (e.g., "POST", "PUT") as a Lua `string`.
- `http_path`: The HTTP path (e.g., "/api/v1/resource") as a Lua `string`.
- `json_body`: The JSON body of the HTTP request as a Lua `string`.

#### Returns
- `result`: An integer indicating the result of the validation. A return value of `0` signifies successful validation, while other values indicate specific types of validation failures.
- `error_msg`: Optional second return value, a JSON-formatted `string` containing details of the validation error.

#### Possible Error Codes (Integers)
- `NONE = 0`
- `INVALID_METHOD = -1`
- `INVALID_ROUTE = -2`
- `INVALID_BODY = -6`

#### Example

```lua
local result, error_msg = validators:ValidateBody("POST", "/api/v1/resource", "{\"key\": \"value\"}")
if result ~= 0 then
    print("Validation failed: " .. error_msg)
end
```

#### Notes
- The `error_msg` will be a JSON-formatted `string` in case of a validation error.

<div style="text-align: right">

[Table of Contents](#table-of-contents)

</div>

--- 

### 4. Validate Path Parameters 📂

Validates the path parameters of the HTTP request according to the OpenAPI specification. An optional second return value provides details if validation fails.

#### Validation sequence
1. HTTP method
2. Route
3. Path parameters (in the sequence provided in the OpenAPI spec)

#### Synopsis

```lua
local result, error_msg = validators:ValidatePathParam(method, http_path)
```

#### Arguments
- `method`: The HTTP method (e.g., "GET", "DELETE") as a Lua `string`.
- `http_path`: The HTTP path with parameters (e.g., "/api/v1/resource/{id}") as a Lua `string`.

#### Returns
- `result`: An integer indicating the outcome of the validation. A return value of `0` signifies successful validation, while other values indicate different types of validation failures.
- `error_msg`: Optional second return value, a JSON-formatted `string` with details of the validation error.

#### Possible Error Codes (Integers)
- `NONE = 0`
- `INVALID_METHOD = -1`
- `INVALID_ROUTE = -2`
- `INVALID_PATH_PARAM = -3`

#### Example

```lua
local result, error_msg = validators:ValidatePathParam("GET", "/api/v1/resource/123")
if result ~= 0 then
    print("Validation failed: " .. error_msg)
end
```

#### Notes
- The `error_msg` will be a JSON-formatted `string` if a validation error occurs.

<div style="text-align: right">

[Table of Contents](#table-of-contents)

</div>

--- 

### 5. Validate Query Parameters ❓

Validates the query parameters of the HTTP request according to the OpenAPI specification. An optional second return value provides details if validation fails.

#### Validation sequence
1. HTTP method
2. Route
3. Query parameters (in the sequence provided in the OpenAPI spec)

#### Synopsis

```lua
local result, error_msg = validators:ValidateQueryParam(method, http_path)
```

#### Arguments
- `method`: The HTTP method (e.g., "GET", "DELETE") as a Lua `string`.
- `http_path`: The HTTP path including query parameters (e.g., "/api/v1/resource?name=value") as a Lua `string`.

#### Returns
- `result`: An integer indicating the outcome of the validation. A return value of `0` signifies successful validation, whereas other values indicate different types of validation failures.
- `error_msg`: Optional second return value, a JSON-formatted `string` containing details of the validation error.

#### Possible Error Codes (Integers)
- `NONE = 0`
- `INVALID_METHOD = -1`
- `INVALID_ROUTE = -2`
- `INVALID_QUERY_PARAM = -3`

#### Example

```lua
local result, error_msg = validators:ValidateQueryParam("GET", "/api/v1/resource?name=value")
if result ~= 0 then
    print("Validation failed: " .. error_msg)
end
```

#### Notes
- The `error_msg` will be a JSON-formatted `string` in case of a validation error.

<div style="text-align: right">

[Table of Contents](#table-of-contents)

</div>

--- 

### 6. Validate Header Parameters 📋

Validates the header parameters of the HTTP request according to the OpenAPI specification. An optional second return value provides details if validation fails.

#### Validation sequence
1. HTTP method
2. Route
3. Header parameters

#### Synopsis

```lua
local result, error_msg = validators:ValidateHeaders(method, http_path, headers)
```

#### Arguments
- `method`: The HTTP method (e.g., "GET", "POST") as a Lua `string`.
- `http_path`: The HTTP path (e.g., "/api/v1/resource") as a Lua `string`.
- `headers`: The HTTP headers as a Lua table of key/value pairs.

#### Returns
- `result`: An integer indicating the outcome of the validation. A return value of `0` signifies successful validation, whereas other values indicate different types of validation failures.
- `error_msg`: Optional second return value, a JSON-formatted `string` containing details of the validation error.

#### Possible Error Codes (Integers)
- `NONE = 0`
- `INVALID_METHOD = -1`
- `INVALID_ROUTE = -2`
- `INVALID_HEADER_PARAM = -3`

#### Example

```lua
local headers = {["Authorization"] = "Bearer token"}
local result, error_msg = validators:ValidateHeaders("GET", "/api/v1/resource", headers)
if result ~= 0 then
    print("Validation failed: " .. error_msg)
end
```

#### Notes
- The `error_msg` will be a JSON-formatted `string` in case of a validation error.

<div style="text-align: right">

[Table of Contents](#table-of-contents)

</div>

--- 

### 7. Validate Request ✔️

Validates the entire HTTP request including method, route, path params, and/or query params according to the OpenAPI specification. An optional second return value provides details if validation fails.

#### Validation sequence
1. HTTP method
2. Route
3. Path parameters (if specified in specs, in the sequence provided in OpenAPI spec)
4. Query parameters (if specified in specs, in the sequence provided in OpenAPI spec)

#### Synopsis

```lua
local result, error_msg = validators:ValidateRequest(method, http_path)
```

#### Arguments
- `method`: The HTTP method (e.g., "POST", "PUT") as a Lua `string`.
- `http_path`: The HTTP path (e.g., "/api/v1/resource") as a Lua `string`.

#### Returns
- `result`: An integer indicating the outcome of the validation. A return value of `0` signifies successful validation, whereas other values indicate different types of validation failures.
- `error_msg`: Optional second return value, a JSON-formatted `string` containing details of the validation error.

#### Possible Error Codes (Integers)
- `NONE = 0`
- `INVALID_METHOD = -1`
- `INVALID_ROUTE = -2`
- `INVALID_PATH_PARAM = -3`
- `INVALID_QUERY_PARAM = -4`

#### Example

```lua
local result, error_msg = validators:ValidateRequest("POST", "/api/v1/resource")
if result ~= 0 then
    print("Validation failed: " .. error_msg)
end
```

#### Notes
- The `error_msg` will be a JSON-formatted `string` in case of a validation error.

<div style="text-align: right">

[Table of Contents](#table-of-contents)

</div>

--- 

### 8. Validate Request (Overloaded) ✔️🔠

Validates the entire HTTP request including method, route, JSON body, path parameters, and query parameters (if specified in specs) according to the OpenAPI specification. An optional second return value provides details if validation fails.

#### Validation sequence
1. HTTP method
2. Route
3. JSON body schema
4. Path parameters (if specified in specs, in the sequence provided in OpenAPI spec)
5. Query parameters (if specified in specs, in the sequence provided in OpenAPI spec)

#### Synopsis

```lua
local result, error_msg = validators:ValidateRequest(method, http_path, json_body)
```

#### Arguments
- `method`: The HTTP method (e.g., "POST", "PUT") as a Lua `string`.
- `http_path`: The HTTP path (e.g., "/api/v1/resource") as a Lua `string`.
- `json_body`: The JSON body of the HTTP request as a Lua `string`.

#### Returns
- `result`: An integer indicating the outcome of the validation. A return value of `0` signifies successful validation, whereas other values indicate different types of validation failures.
- `error_msg`: Optional second return value, a JSON-formatted `string` containing details of the validation error.

#### Possible Error Codes (Integers)
- `NONE = 0`
- `INVALID_METHOD = -1`
- `INVALID_ROUTE = -2`
- `INVALID_BODY = -3`
- `INVALID_PATH_PARAM = -4`
- `INVALID_QUERY_PARAM = -5`

#### Example

```lua
local result, error_msg = validators:ValidateRequest("POST", "/api/v1/resource", "{\"key\": \"value\"}")
if result ~= 0 then
    print("Validation failed: " .. error_msg)
end
```

#### Notes
- The `error_msg` will be a JSON-formatted `string` in case of a validation error.

<div style="text-align: right">

[Table of Contents](#table-of-contents)

</div>

--- 

### 9. Validate Request (Overloaded) ✔️🔢

Validates the entire HTTP request including method, route, path parameters, query parameters, and headers according to the OpenAPI specification. An optional second return value provides details if validation fails.

#### Validation sequence
1. HTTP method
2. Route
3. Path parameters (if specified in specs, in the sequence provided in OpenAPI spec)
4. Query parameters (if specified in specs, in the sequence provided in OpenAPI spec)
5. Header parameters

#### Synopsis

```lua
local result, error_msg = validators:ValidateRequestWithHeaders(method, http_path, headers)
```

#### Arguments
- `method`: The HTTP method (e.g., "GET", "DELETE") as a Lua `string`.
- `http_path`: The HTTP path (e.g., "/api/v1/resource") as a Lua `string`.
- `headers`: The HTTP headers as a Lua table where each key-value pair represents a header.

#### Returns
- `result`: An integer indicating the outcome of the validation. A return value of `0` signifies successful validation, whereas other values indicate different types of validation failures.
- `error_msg`: Optional second return value, a JSON-formatted `string` containing details of the validation error.

#### Possible Error Codes (Integers)
- `NONE = 0`
- `INVALID_METHOD = -1`
- `INVALID_ROUTE = -2`
- `INVALID_PATH_PARAM = -3`
- `INVALID_QUERY_PARAM = -4`
- `INVALID_HEADER_PARAM = -5`

#### Example

```lua
local headers = {["Authorization"] = "Bearer token"}
local result, error_msg = validators:ValidateRequestWithHeaders("GET", "/api/v1/resource", headers)
if result ~= 0 then
    print("Validation failed: " .. error_msg)
end
```

#### Notes
- The `error_msg` will be a JSON-formatted `string` in case of a validation error.

<div style="text-align: right">

[Table of Contents](#table-of-contents)

</div>

--- 

### 10. Validate Request (Overloaded) ✔️🔣

Validates the entire HTTP request including method, route, JSON body, path parameters, query parameters, and headers against the OpenAPI specification. Sets `error_msg` with a descriptive error message if validation fails.

#### Validation sequence
1. HTTP method
2. Route
3. Body schema
4. Path parameters (if specified in specs, in the sequence of provided in OpenAPI spec)
5. Query parameters (if specified in specs, in the sequence of provided in OpenAPI spec)
6. Header parameters

#### Synopsis

```lua
local result, error_msg = validators:ValidateRequest(method, http_path, json_body, headers)
```

#### Arguments
- `method`: The HTTP method (e.g., "POST", "PUT") as a Lua `string`.
- `http_path`: The HTTP path (e.g., "/api/v1/resource") as a Lua `string`.
- `json_body`: The JSON body of the HTTP request as a Lua `string`.
- `headers`: The HTTP headers as a table of key-value pairs (`{["Header-Name"] = "Header-Value", ...}`).

#### Returns
- `result`: An integer indicating the outcome of the validation. A return value of `0` signifies successful validation, whereas other values indicate different types of validation failures.
- `error_msg`: Optional second return value, a JSON-formatted `string` containing details of the validation error.

#### Possible Error Codes (Integers)
- `NONE = 0`
- `INVALID_METHOD = -1`
- `INVALID_ROUTE = -2`
- `INVALID_BODY = -3`
- `INVALID_PATH_PARAM = -4`
- `INVALID_QUERY_PARAM = -5`
- `INVALID_HEADER_PARAM = -6`

#### Example

```lua
local headers = {["Authorization"] = "Bearer token"}
local result, error_msg = validators:ValidateRequest("POST", "/api/v1/resource", "{\"key\": \"value\"}", headers)
if result ~= 0 then
    print("Validation failed: " .. error_msg)
end
```

#### Notes
- The `error_msg` will be a JSON-formatted `string` in case of a validation error.

<div style="text-align: right">

[Table of Contents](#table-of-contents)

</div>