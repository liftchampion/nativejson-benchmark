# The Art of C++ / JSON

[![Release](https://img.shields.io/github/release/taocpp/json.svg)](https://github.com/taocpp/json/releases/latest)
[![TravisCI](https://travis-ci.org/taocpp/json.svg)](https://travis-ci.org/taocpp/json)
[![AppVeyor](https://ci.appveyor.com/api/projects/status/github/taocpp/json?svg=true)](https://ci.appveyor.com/project/taocpp/json)
[![Coverage](https://img.shields.io/coveralls/taocpp/json.svg)](https://coveralls.io/github/taocpp/json)

The Art of C++ / JSON is a zero-dependency C++11 header-only library that provides a generic JSON value object, conversions from and to JSON string representation, and supports JSON Pointer and JSON Patch. It is designed for correctness and simplicity, is based on C++ standard containers, and strives for speed through minimalism (rather than complicated optimisations).

## Features

* Serious standard conformance!

  * [RFC7159], [ECMA-404]: The JavaScript Object Notation (JSON) Data Interchange Format.
  * Achieves a **100% score** in the [Native JSON Benchmark](https://github.com/miloyip/nativejson-benchmark) conformance tests.
  * [RFC6901]: JavaScript Object Notation (JSON) Pointer.
  * [RFC6902]: JavaScript Object Notation (JSON) Patch.
  * Experimental support for [JSON Reference](https://tools.ietf.org/html/draft-pbryan-zyp-json-ref-03) (local URI fragments only).
  * Experimental support for [JSON Schema](http://json-schema.org/documentation.html) (draft v4).

* General

  * Provides **DOM** and **SAX**-style APIs.
  * Numeric values are stored as `std::int64_t`, `std::uint64_t` or `double`.
  * Does *not* support duplicate keys in JSON objects.
  * Currently only supports UTF-8 as input and output encoding.

* DOM API (aka "The value class")

  * Construction of objects and arrays via `std::initializer_list`.
  * Allows construction of JSON value objects from arbitrary *user-defined types* with specialised traits class template.
  * Standard STL containers `std::string` for JSON strings, `std::vector< tao::json::value >` for JSON arrays, and `std::map< std::string, tao::json::value >` for JSON objects.
  * No memory allocations by the JSON value class itself (the wrapped standard containers perform their memory allocations normally).
  * Indirect JSON values via non-owning C++ raw pointers for object sharing, reference resolution, and important optimization opportunities.
  * C++11 literal operator for JSON values.

* SAX API

  * Simple, straight forward SAX API.
  * Parse JSON string representation to SAX events.
  * Stream SAX to (prettified) JSON string representation.
  * SAX-to-DOM value construction.
  * DOM-to-SAX value traversal.
  * Supports conversion from and to *foreign* DOM objects (check [`contrib/nlohmann.cpp`](contrib/nlohmann.cpp)).
  * SAX comparator (against an existing DOM value).
  * SAX hash algorithm (SHA-256 based).
  * SAX schema validator.

## Status

While most parts have stabilized, some parts of the API are still under discussion and development and might change and/or be extended before the first release.

The library currently serves as a fully functional real-world example for the [Parsing Expression Grammar Template Library (PEGTL)](https://github.com/taocpp/PEGTL), which is used for parsing JSON string representations.

## Documentation

The documentation will be finished once the functionality and interfaces are finalised.

Until then, here are a few short indications on how to use this library:

* Requires Clang or GCC with `-std=c++11` (or other compiler with sufficient C++11 support).
* The library is header-only, to install and use simply copy the directory [`include/tao`](include/tao) to a convenient place and include the file [`include/tao/json.hpp`](include/tao/json.hpp).
* The generic JSON value class, the main part of this library, is in [`include/tao/json/value.hpp`](include/tao/json/value.hpp).
* To parse a JSON string representation to a DOM value, use one of the functions in [`include/tao/json/from_string.hpp`](include/tao/json/from_string.hpp) or [`include/tao/json/parse_file.hpp`](include/tao/json/parse_file.hpp).
* To produce a JSON string representation from a DOM value, use one of the appropriate functions in [`include/tao/json/to_string.hpp`](include/tao/json/to_string.hpp) or [`include/tao/json/stream.hpp`](include/tao/json/stream.hpp).

## License

The Art of C++ is certified [Open Source](http://www.opensource.org/docs/definition.html) software. It may be used for any purpose, including commercial purposes, at absolutely no cost. It is distributed under the terms of the [MIT license](http://www.opensource.org/licenses/mit-license.html) reproduced here.

> Copyright (c) 2015-2017 Dr. Colin Hirsch and Daniel Frey
>
> Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
>
> The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## Contact

For questions and suggestions about the Art of C++ / JSON please contact the authors at `jsonl(at)colin-hirsch.net`.

## Thank You

* Niels Lohmann and his [JSON library](https://github.com/nlohmann/json) for the inspiration.
* Milo Yip and his [Native JSON Benchmark](https://github.com/miloyip/nativejson-benchmark) for a reference to measure progress against.

## Additional Licenses

### PEGTL License

The Art of C++ / JSON contains portions of the [PEGTL](https://github.com/taocpp/PEGTL) which is licensed as follows:

> Copyright (c) 2007-2017 Dr. Colin Hirsch and Daniel Frey
>
> Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
>
> The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

### Double-Conversion License

The Art of C++ / JSON contains slightly modified portions of the [double-conversion library](https://github.com/google/double-conversion) which is licensed as follows:

> Copyright 2006-2011, the V8 project authors. All rights reserved. Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
>
> * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
> * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
> * Neither the name of Google Inc. nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
>
> THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

[RFC7159]: https://tools.ietf.org/html/rfc7159
[ECMA-404]: http://www.ecma-international.org/publications/standards/Ecma-404.htm
[RFC6901]: https://tools.ietf.org/html/rfc6901
[RFC6902]: https://tools.ietf.org/html/rfc6902
