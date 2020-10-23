    jsoncons::output_format

    typedef basic_output_format<char> output_format

The output_format class is an instantiation of the basic_output_format class template that uses `char` as the character type.

The default floating point formatting produces digits in decimal format if possible, if not, it produces digits in exponential format. Trailing zeros are removed, except the one immediately following the decimal point. The period character (�.�) is always used as the decimal point, non English locales are ignored.  A `precision` gives the maximum number of significant digits, the default precision is `16`. On most modern machines, 17 digits is usually enough to capture a floating-point number's value exactly, however, if you change precision to 17, conversion to text becomes an issue for floating point numbers that do not have an exact representation, e.g. 1.1 read may become 1.1000000000000001 when written. 

When parsing text, the precision of the fractional number is retained, and used for subsequent serialization, to allow round-trip.

### Header

    #include "jsoncons/output_format.hpp"

### Member constants

    default_precision
The default precision is 16

    default_indent
The default indent is 4

### Constructors

    output_format()
Constructs an `output_format` with default values. 

### Accessors

    int indent() const
Returns the level of indentation, the default is 4

    uint8_t precision() const 
Returns the maximum number of significant digits.

    bool escape_all_non_ascii() const
The default is false

    bool escape_solidus() const
The default is false

    bool replace_nan() const
The defult is `true`

    bool replace_pos_inf() const
The defult is `true`

    bool replace_neg_inf() const
The defult is `true`

    std::string nan_replacement() const 
The default is "null"

    std::string pos_inf_replacement() const 
The default is "null"

    std::string neg_inf_replacement() const 
The default is "null"

### Modifiers

    output_format& indent(int value)

    output_format& escape_all_non_ascii(bool value)

    output_format& escape_solidus(bool value)

    output_format& replace_nan(bool replace)

    output_format& replace_inf(bool replace)

    output_format& replace_pos_inf(bool replace)

    output_format& replace_neg_inf(bool replace)

    output_format& nan_replacement(const std::string& replacement)

    output_format& pos_inf_replacement(const std::string& replacement)

    output_format& neg_inf_replacement(const std::string& replacement)
Sets replacement text for negative infinity.

    output_format& precision(uint8_t prec)

    output_format& object_object_block_option(block_options value)
Set object block formatting within objects to `block_options::same_line` or `block_options::next_line`. The default is `block_options::same_line`.

    output_format& array_object_block_option(block_options value)
Set object block formatting within arrays to `block_options::same_line` or `block_options::next_line`.  The default is `block_options::next_line`.

    output_format& object_array_block_option(block_options value)
Set array block formatting within objects to `block_options::same_line` or `block_options::next_line`. The default is `block_options::same_line`.

    output_format& array_array_block_option(block_options value)
Set array block formatting within arrays to `block_options::same_line` or `block_options::next_line`. The default is `block_options::next_line`.

## Examples

### Default NaN, inf and -inf replacement
```c++
    json obj;
    obj["field1"] = std::sqrt(-1.0);
    obj["field2"] = 1.79e308*1000;
    obj["field3"] = -1.79e308*1000;
    std::cout << obj << std::endl;
```
The output is
```json
    {"field1":null,"field2":null,"field3":null}
```
### User specified `Nan`, `Inf` and `-Inf` replacement

```c++
    json obj;
    obj["field1"] = std::sqrt(-1.0);
    obj["field2"] = 1.79e308*1000;
    obj["field3"] = -1.79e308*1000;

    output_format format;
    format.nan_replacement("null");        // default is "null"
    format.pos_inf_replacement("1e9999");  // default is "null"
    format.neg_inf_replacement("-1e9999"); // default is "null"

    std::cout << pretty_print(obj,format) << std::endl;
```

The output is
```json
    {
        "field1":null,
        "field2":1e9999,
        "field3":-1e9999
    }
```

### Object-array block formatting

```c++
    json val;

    val["verts"] = {1, 2, 3};
    val["normals"] = {1, 0, 1};
    val["uvs"] = {0, 0, 1, 1};

    std::cout << "Default format" << std::endl;
    std::cout << pretty_print(val) << std::endl;

    std::cout << "Array same line format" << std::endl;
    output_format format1;
    format1.array_block_option(block_options::same_line);
    std::cout << pretty_print(val,format1) << std::endl;

    std::cout << "Object array next line format" << std::endl;
    output_format format2;
    format2.object_array_block_option(block_options::next_line);
    std::cout << pretty_print(val,format2) << std::endl;
```

The output is
```json
Default format
{
    "normals": [1,0,1],
    "uvs": [0,0,1,1],
    "verts": [1,2,3]
}
Array same line format
{
    "normals": [1,0,1],
    "uvs": [0,0,1,1],
    "verts": [1,2,3]
}
Object array next line format
{
    "normals":
    [1,0,1],
    "uvs":
    [0,0,1,1],
    "verts":
    [1,2,3]
}
```

### Array-array block formatting
```c++
    json val;
    val["data"]["id"] = {0,1,2,3,4,5,6,7};
    val["data"]["item"] = {{2},{4,5,2,3},{4},{4,5,2,3},{2},{4,5,3},{2},{4,3}};

    std::cout << "Default array-array block format" << std::endl;
    std::cout << pretty_print(val) << std::endl;

    std::cout << "Same line array-array block format" << std::endl;
    output_format format1;
    format1.array_array_block_option(block_options::same_line);
    std::cout << pretty_print(val, format1) << std::endl;

    std::cout << "Next line object-array and same line array-array format" << std::endl;
    output_format format2;
    format2.object_array_block_option(block_options::next_line)
           .array_array_block_option(block_options::same_line);
    std::cout << pretty_print(val, format2) << std::endl;
```
The output is
```json
Default array-array block format
{
    "data": {
        "id": [0,1,2,3,4,5,6,7],
        "item": [
            [2],
            [4,5,2,3],
            [4],
            [4,5,2,3],
            [2],
            [4,5,3],
            [2],
            [4,3]
        ]
    }
}
Same line array-array block format
{
    "data": {
        "id": [0,1,2,3,4,5,6,7],
        "item": [[2],[4,5,2,3],[4],[4,5,2,3],[2],[4,5,3],[2],[4,3]]
    }
}
Next line object-array and same line array-array format
{
    "data": {
        "id":
        [0,1,2,3,4,5,6,7],
        "item":
        [[2],[4,5,2,3],[4],[4,5,2,3],[2],[4,5,3],[2],[4,3]]
    }
}
```


