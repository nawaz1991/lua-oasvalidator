# lua-oasvalidator: REST Request Validator

`lua-oasvalidator` is a Lua binding for the [cpp-oasvalidator](https://github.com/nawaz1991/cpp-oasvalidator) library, designed for the validation of HTTP requests against OpenAPI 3.x specifications. By integrating this module with your Lua-based REST servers or API gateways, you can ensure that only compliant requests interact with your backend services.

With support for OpenAPI 3.x, this module streamlines the process of validating various components of an HTTP request, from methods and routes to detailed parameters and `JSON` body.

## 🌟 Key Features
- **Lua Integration**: Seamless integration with Lua, allowing you to use the capabilities of `cpp-oasvalidator` within API gateways like Kong/NGINX.
- **Efficient, Sequential Validation**: Validates requests in a logical order, starting from the HTTP method down to the header parameters. This means if you validate a later stage, preceding steps are validated as well.
- **In-Depth Error Reports**: Returns an insightful error enumeration coupled with an extensive error message in JSON format to pinpoint inaccuracies.
- **Optimized Performance**: Utilizes lazy deserialization, only processing content when all prior checks pass.
- **Broad Parameter Support**: Deserializes parameters across a spectrum of styles and data types, ensuring a wide range of OpenAPI configurations are supported.

## 📜 Validation Sequence
`lua-oasvalidator` validates HTTP requests in the following order:

1. **HTTP Method Validation**: Ensures that the HTTP method (GET, POST, PUT, etc.) aligns with the OpenAPI spec.
2. **Route Validation**: Checks if the provided route matches the specification.
3. **Body Validation**: Validates the JSON body structure and data against the OpenAPI spec.
4. **Path Parameter Validation**: Validates path parameters.
5. **Query Parameter Validation**: Ensures query parameters are consistent with the OpenAPI spec.
6. **Header Parameter Validation**: Confirms headers are in line with the OpenAPI specification.
7. **Request Validation**: Validates the whole HTTP request starting from method, route, body (if provided), path/query params (if specified in specs) and/or headers. To address all variations, four overloaded methods are provided.

For a comprehensive understanding, refer to [API Reference](API.md).

## 🛠 Parameter Styles, data types & Deserialization
`cpp-oasvalidator` can deserialize and parse parameters of all data types serialized in various styles provided by Swagger/OpenAPI. Following tables provide the details.

### Path Parameters
| **Style** | **Explode** | **Primitive** | **String** | **Array of primitives** | **Array of strings** | **Object** |
|:---------:|:-----------:|:-------------:|:----------:|:-----------------------:|:--------------------:|:----------:|
|  simple*  |   false*    |       ✅       |     ✅      |            ✅            |          ✅           |     ✅      |
|  simple   |    true     |       ✅       |     ✅      |            ✅            |          ✅           |     ✅      |
|   label   |    false    |       ✅       |     ✅      |            ✅            |          ✅           |     ✅      |
|   label   |    true     |       ✅       |     ✅      |            ✅            |          ✅           |     ✅      |
|  matrix   |    false    |       ✅       |     ✅      |            ✅            |          ✅           |     ✅      |
|  matrix   |    true     |       ✅       |     ✅      |            ✅            |          ✅           |     ✅      |

&#42; Default serialization method

### Query Parameters
|   **Style**    | **Explode** | **Primitive** | **String** | **Array of primitives** | **Array of strings** | **Object** |
|:--------------:|:-----------:|:-------------:|:----------:|:-----------------------:|:--------------------:|:----------:|
|     form*      |    true*    |       ✅       |     ✅      |            ✅            |          ✅           |     ✅      |
|      form      |    false    |       ✅       |     ✅      |            ✅            |          ✅           |     ✅      |
| spaceDelimited |    true     |      N/A      |    N/A     |            ✅            |          ✅           |    N/A     |
| spaceDelimited |    false    |      N/A      |    N/A     |            ✅            |          ✅           |    N/A     |
| pipeDelimited  |    true     |      N/A      |    N/A     |            ✅            |          ✅           |    N/A     |
| pipeDelimited  |    false    |      N/A      |    N/A     |            ✅            |          ✅           |    N/A     |
|   deepObject   |    false    |      N/A      |    N/A     |           N/A           |         N/A          |     ❌      |

&#42; Default serialization method

### Header Parameters
| **Style** | **Explode** | **Primitive** | **String** | **Array of primitives** | **Array of strings** | **Object** |
|:---------:|:-----------:|:-------------:|:----------:|:-----------------------:|:--------------------:|:----------:|
|  simple*  |   false*    |       ✅       |     ✅      |            ✅            |          ✅           |     ✅      |
|  simple   |    true     |       ✅       |     ✅      |            ✅            |          ✅           |     ✅      |

&#42; Default serialization method

## 🚫 Error Handling

`lua-oasvalidator` returns a specific `ERROR_CODE` value, indicating the error type:

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

An accompanying detailed error message, structured in JSON, elucidates the error:

```JSON
{
  "errorCode": "INVALID_BODY",
  "detail": {
    "specRef": "#/paths/%2Fpet/put/requestBody/content/application%2Fjson/schema",
    "code": "type",
    "description": "Property has a type 'string' that is not in the following list: 'array'.",
    "instance": "#/photoUrls",
    "schema": "#/properties/photoUrls"
  }
}
```


## 🚀 Getting Started

### 🔧 Installation

**Prerequisites:**
* lua development environment
   * `lua-devel` (linux)
   * or [luavm](https://github.com/xpol/luavm)(windows)
   * or `brew install lua luarocks`
   * or any equivalent on your system
* `cmake` >= `3.1.0`, cmake 2.8 may work but not well tested.
* A C++11 compatible compiler.

**Building and Installing:**

To build and install `lua-oasvalidator`, follow the steps below:

1. Clone and initialize the git submodules
2. Navigate to the root directory of the `lua-oasvalidator`
3. Run `luarocks make` to build and install
   ```bash
   git clone --recurse-submodules https://github.com/nawaz1991/lua-oasvalidator.git
   cd lua-oasvalidator
   sudo luarocks make
   ```

### 🎬 Initialization
To utilize `lua-oasvalidator`, include the module and initialize the validator with your OpenAPI specification:

```lua
local oasvalidator = require("oasvalidator")
print(oasvalidator._VERSION)

-- Initialize and get validators
local validators = oasvalidator.GetValidators("/path/to/your/spec.json")

err_code, err_msg = validators:ValidateRoute("PUT", "/pet")
if err_code ~= 0 then
   print(err_msg)
end
```

For a detailed breakdown of each API, refer to the [API Reference](API.md).

## 📜 Conclusion

`lua-oasvalidator` bridges the power of `cpp-oasvalidator` to the Lua based requirements like to write the plugins for Kong and NGINX etc. With its organized validation order, expansive parameter style support, and meticulous error reporting, it ensures your Lua-based services stay compliant with your OpenAPI specs.

## 📄 License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for the full license text.

© 2023 [Muhammad Nawaz](mailto:m.nawaz2003@gmail.com). All Rights Reserved.
