```c++
    jsoncons::json

    typedef basic_json<char,std::allocator<char>> json
```
The `json` class is an instantiation of the `basic_json` class template that uses `char` as the character type. The `jsoncons` library will always rebind the given allocator from the template parameter to internal data structures.

### Header

    #include "jsoncons/json.hpp"

### Member types

Member type                         |Definition
------------------------------------|------------------------------
`allocator_type`|Allocator type
`string_allocator`|String allocator type
`array_allocator`|Array allocator type
`object_allocator`|Object allocator type
`string_type`|Default `string_type` is `std::string`
`member_type`|[member_type](json_member_type) is a class that stores a name and a json value
`null_type`|An alias for `jsoncons::null_type`
`object`|json object type
`array`|json array type
`object_iterator`|A bidirectional iterator to `json::member_type`
`const_object_iterator`|A bidirectional iterator to `const json::member_type`
`array_iterator`|A random access iterator to `json`
`const_array_iterator`|A random access iterator to `const json`
`object_range`|An `object_range` provides a pair of bidirectional iterators, defined by `begin()` and `end()`, for accessing the members of a json object. An `object_range` supports range-based for loops over the members.
`const_object_range`|A `const_object_range` provides a pair of const bidirectional iterators, defined by `begin()` and `end()`, for accessing the members of a json object.
`array_range`|An `array_range` provides a pair of random access iterators, defined by `begin()` and `end()`, for accessing the elements of a json array. An `array_range` supports range-based for loops over the elements.
`const_array_range`|A `const_array_range` provides a pair of const random access iterators, defined by `begin()` and `end()`, for accessing the elements of a json array. 

### Static member functions

    static json parse(const string_type& s)
    static json parse(const string_type& s, 
                      parse_error_handler& err_handler)
Parses a string of JSON text and returns a json object or array value. 
Throws [parse_exception](parse_exception) if parsing fails.

    static json parse_stream(std::istream& is)
    static json parse_stream(std::istream& is, 
                             parse_error_handler& err_handler)
Parses an input stream of JSON text and returns a json object or array value. 
Throws [parse_exception](parse_exception) if parsing fails.

    static json parse_file(const std::string& filename)
    static json parse_file(const std::string& filename, 
                           parse_error_handler& err_handler)
Opens a binary input stream to a JSON unicode file, parsing the file assuming UTF-8, and returns a json object or array value. This method expects that the file contains UTF-8 (or clean 7 bit ASCII), if that is not the case, use the `parse` method that takes an `std::istream` instead, imbue your stream with the appropriate facet for handling unicode conversions.
Throws [parse_exception](parse_exception) if parsing fails.

    static json make_array()

    static json make_array(size_t n, const array_allocator& allocator = array_allocator())

    template <typename T>
    static json make_array(size_ n, const T& val)

    template <typename T>
    static json make_array(size_ n, const T& val, const array_allocator& allocator = array_allocator())

    template <size_t N>
    static json make_array(size_t size1 ... size_t sizeN)

    template <size_t N,typename T>
    static json make_array(size_t size1 ... size_t sizeN, const T& val)

    template <size_t N,typename T>
    static json make_array(size_t size1 ... size_t sizeN, const T& val, const array_allocator& allocator)
Makes a multidimensional array with the number of dimensions specified as a template parameter. The size of each dimension is passed as a parameter, and optionally an inital value. If no initial value, the default is an empty json object. The elements may be accessed using familiar C++ native array syntax.

### Constructors

    json()
Constructs an empty json object. 

    json(const allocator_type& allocator)
Constructs a json object. 

    json(std::initializer_list<json> list, const allocator_type& allocator)
Constructs a json array with the elements of the initializer list `init`. 

    json(const json& val)
Constructs a copy of val

    json(const json& val, const allocator_type& allocator)
Copy with allocator

    json(json&& val)
Acquires the contents of val, leaving val a `null` value

    json(json&& val, const allocator_type& allocator)
Move with allocator

    template <typename T>
    json(T val, const allocator_type& allocator=allocator_type())
Constructs a `json` value for types supported in `json_type_traits`

    template <class InputIterator>
    json(InputIterator first, InputIterator last, 
         const allocator_type& allocator=allocator_type())

Constructs a json array with the elements in the range [first,last).

### Destructor

    ~json()
Destroys all values and deletes all memory allocated for strings, arrays, and objects.

### Assignment operator

    json& operator=(const json& rhs)
    json& operator=(json&& rhs)

    template <class T>
    json& operator=(T rhs)
Assigns a new value to a `json` variable, replacing it's current contents.

### Ranges and Iterators

    object_range members();  
    const_object_range members() const;  
Returns a "range" defined by `begin()` and `end()` over the members of a `json` object      
Throws `std::runtime_error` if not an object.

    array_range elements();
    const_array_range elements() const;
Returns a "range" defined by `begin()` and `end()` over the elements of a `json` array      
Throws `std::runtime_error` if not an array.

### Capacity

    size_t size() const noexcept
Returns the number of elements in a json array, or the number of members in a json object, or `zero`

    bool empty() const noexcept
Returns `true` if a json string, object or array has no elements, otherwise `false`.

    size_t capacity() const
Returns the size of the storage space currently allocated for a json object or array.

    void reserve(size_t n)
Increases the capacity of a json object or array to allow at least `n` members or elements. 

    void resize(size_t n)
Resizes a json array so that it contains `n` elements. 

    void resize(size_t n, const json& val)
Resizes a json array so that it contains `n` elements that are initialized to `val`. 

### Accessors

    size_t count(const string_type& name) const
Returns the number of object members that match `name`.    

    template <typename T>
    bool is() const noexcept
Returns `true` if json value has type `T`, `false` otherwise.  

    bool is<char> const noexcept 
    bool is<signed char> const noexcept
    bool is<unsigned char> const noexcept
    bool is<wchar_t> const noexcept
    bool is<short> const noexcept
    bool is<unsigned short> const noexcept 
    bool is<int> const noexcept 
    bool is<unsigned int> const noexcept 
    bool is<long> const noexcept 
    bool is<unsigned long> const noexcept 
    bool is<long long> const noexcept 
    bool is<unsigned long long> const noexcept 
Return `true` if json value is of integral type and within the range of the template type, `false` otherwise.  

    is<double> 
Return true if the json value is of floating point type and within the range of the template type, `false` otherwise.  

    is<string_type> 
Returns `true` if the json value is of string type, `false` otherwise.  

    is<bool>
Returns `true` if the json value is of boolean type, `false` otherwise.  

    is<json::null_type>
Returns `true` if the json value is null, `false` otherwise.  

    is<json::object> 
Returns `true` if the json value is an object, `false` otherwise.  

    is<json::array> 
Returns `true` if the json value is an array, `false` otherwise.  

    is<json::std::vector<T>>
Returns `true` if the json value is an array and each element has type `T`, `false` otherwise.

    bool is_null() const noexcept
    bool is_string() const noexcept
    bool is_number() const noexcept
    bool is_integer() const noexcept
    bool is_uinteger() const noexcept
    bool is_double() const noexcept
    bool is_bool() const noexcept
    bool is_object() const noexcept
    bool is_array() const noexcept
Non-generic versions of `is_` methods

    json& operator[](size_t i)
    const json& operator[](size_t i) const
Returns a reference to the value at position i in a json object or array.
Throws `std::runtime_error` if not an object.

    json& operator[](const string_type& name)
Returns a proxy to a keyed value. If written to, inserts or updates with the new value. If read, evaluates to a reference to the keyed value, if it exists, otherwise throws. 
Throws `std::runtime_error` if not an object.
If read, throws `std::out_of_range` if the object does not have a member with the specified name.  

    const json& operator[](const string_type& name) const
If `name` matches the name of a member in the json object, returns a reference to the json object, otherwise throws.
Throws `std::runtime_error` if not an object.
Throws `std::out_of_range` if the object does not have a member with the specified name.  

    object_iterator find(const string_type& name)
    object_iterator find(const char* name)
    const_object_iterator find(const string_type& name) const
    const_object_iterator find(const char* name) const
Returns an object iterator to a member whose name compares equal to `name`. If there is no such member, returns `end_member()`.
Throws `std::runtime_error` if not an object.

    json& at(const string_type& name)
    const json& at(const string_type& name) const
Returns a reference to the value with the specifed name in a json object.
Throws `std::runtime_error` if not an object.
Throws `std::out_of_range` if the object does not have a member with the specified name.  

    json& at(size_t i)
    const json& at(size_t i) const
Returns a reference to the element at index `i` in a json array.  
Throws `std::runtime_error` if not an array.
Throws `std::out_of_range` if the index is outside the bounds of the array.  

    template <typename T>
    const json get(const string_type& name, T&& default_val) const
If `name` matches the name of a member in the json object, returns a copy of the json object, otherwise returns a copy of `default_val`.
Throws `std::runtime_error` if not an object.

    template <typename T>
    T as() const
Attempts to coerce the json value to the template type

    as<bool>
Returns `false` if value is `false` or `null`, if value is a zero length string, or if value is a zero length array or object. Everything else returns `true`.

    as<double>
If value is double, returns value, if value is signed or unsigned integer, casts to double, if value is `null`, returns `NaN`, otherwise throws.

    char as<char> const
    signed char as<signed char> const
    unsigned char as<unsigned char> const
    wchar_t as<wchar_t> const
    short as<short> const
    unsigned short as<unsigned short> const 
    int as<int> const 
    unsigned int as<unsigned int> const 
    long as<long> const 
    unsigned long as<unsigned long> const 
    long long as<long long> const 
    unsigned long long as<unsigned long long> const 
Return integer value if value has integral type, performs cast if value has double type, returns 1 or 0 if value has bool type, otherwise throws.

    string_type as<string_type>() const noexcept
    string_type as<string_type>(const string_allocator& allocator) const noexcept
If value is string, returns value, otherwise returns result of `to_string`.

    bool as_bool() const noexcept
    int64_t as_integer() const
    uint64_t as_uinteger() const
    double as_double() const
    string_type as_string() const noexcept
    string_type as_string(const string_allocator& allocator) const noexcept
    unsigned int as<unsigned int> const 
Non-generic versions of `as` methods

### Modifiers

    void clear()
Remove all elements from an array or object value, otherwise do nothing

    void erase(array_iterator first, array_iterator last)
Remove the elements from an array in the range '[first,last)'.
Throws `std::runtime_error` if not an object.

    void erase(object_iterator first, object_iterator last)
Remove the members from an object in the range '[first,last)'.
Throws `std::runtime_error` if not an object.

    void erase(const string_type& name)
Remove a member with the specified name from an object
Throws `std::runtime_error` if not an object.

    void shrink_to_fit()
Requests the removal of unused capacity.

    void set(const string_type& name, const json& val)
    void set(string_type&& name, const json& val)
    void set(const string_type& name, json&& val)
    void set(string_type&& name, json&& val)
Inserts a new member or replaces an existing member in a json object.
Throws `std::runtime_error` if not an object.

    object_iterator set(object_iterator hint, const string_type& name, const json& val)
    object_iterator set(object_iterator hint, string_type&& name, const json& val)
    object_iterator set(object_iterator hint, const string_type& name, json&& val)
    object_iterator set(object_iterator hint, string_type&& name, json&& val)
Inserts a new member or replaces an existing member in a json object.
Insertion time is optimized if `hint` points to the member that will precede the inserted member.
Returns a `member_iterator` pointing at the member that was inserted or updated
Throws `std::runtime_error` if not an object.

    void add(const json& val)
    void add(json&& val)
Adds a new element at the end of a json array. The content of `val` is copied (or moved) to the new element.
Throws `std::runtime_error` if not an array.

    array_iterator add(const_array_iterator pos, const json& val)
    array_iterator add(const_array_iterator pos, json&& val)
Adds a new element at the specified position of a json array, shifting all elements currently at or above that position to the right.
The content of `val` is copied (or moved) to the new element.
Returns an `array_iterator` that points to the new value
Throws `std::runtime_error` if not an array.

    void swap(json& val)
Exchanges the content of the `json` value with the content of `val`, which is another `json` value.

### Relational operators

    bool operator==(const json& rhs) const
Returns `true` if two json objects compare equal, `false` otherwise.

    bool operator!=(const json& rhs) const
Returns `true` if two json objects do not compare equal, `false` otherwise.

### Serialization

    string_type to_string(const string_allocator& allocator = string_allocator()) const noexcept
Inserts json value into string.

    string_type to_string(const output_format& format, const string_allocator& allocator = string_allocator()) const
Inserts json value into string using specified [output_format](output_format).

    std::ostream& to_stream(std::ostream& os) const
Inserts json value into stream with default output format.

    std::ostream& to_stream(std::ostream<CharT> os, const output_format& format) const
Inserts json value into stream using specified [output_format](output_format).

    void to_stream(json_output_handler& handler) const
Reports JSON related events for JSON objects, arrays, object members and array elements to a [json_output_handler](json_output_handler), such as a [json_serializer](json_serializer).

### Non member functions

    std::istream& operator>> (std::istream& os, json& val)
Reads a `json` value from a stream.

    std::ostream& operator<< (std::ostream& os, const json& val)
Inserts json value into stream.

    std::ostream& print(const json& val)  
    std::ostream& print(const json& val, const output_format<CharT>& format)  
Inserts json value into stream using the specified [output_format](output_format) if supplied.

    std::ostream& pretty_print(const json& val)  
    std::ostream& pretty_print(const json& val, const output_format<CharT>& format)  
Inserts json value into stream using the specified [output_format](output_format) if supplied.

    void swap(json& a, json& b)
Exchanges the values of `a` and `b`

### Deprecated names

As the `jsoncons` library has evolved, names have sometimes changed. To ease transition, jsoncons deprecates the old names but continues to support many of them. See the [deprecated list](deprecated) for the status of old names. The deprecated names can be suppressed by defining macro JSONCONS_NO_DEPRECATED, which is recommended for new code.

## Examples

### Range-based for loop over members of an object
```c++
    json book = json::parse(R"(
    {
        "category" : "Fiction",
        "title" : "Pulp",
        "author" : "Charles Bukowski",
        "date" : "2004-07-08",
        "price" : 22.48,
        "isbn" : "1852272007"  
    }
    )");

    for (auto member: book.members())
    {
        std::cout << member.name() << ":" << member.value().as<string>() << std::endl;
    } 
```
### Range-based for loop over elements of an array
```c++
    json book1;    
    book1["category"] = "Fiction";
    book1["title"] = "A Wild Sheep Chase: A Novel";
    book1["author"] = "Haruki Murakami";

    json book2;    
    book2["category"] = "History";
    book2["title"] = "Charlie Wilson's War";
    book2["author"] = "George Crile";

    json book3;    
    book3["category"] = "Fiction";
    book3["title"] = "Kafka on the Shore";
    book3["author"] = "Haruki Murakami";

    // Constructing a json array with an initializer list 
    json booklist = {book1, book2, book3};    

    for (auto book: booklist.elements())
    {
        std::cout << book["title"].as<string_type>() << std::end;
    } 
```    
### Accessors and defaults
```c++
    json obj;

    obj["field1"] = 1;
    obj["field3"] = "Toronto";

    double x1 = obj.count("field1") > 0 ? obj["field1"].as<double>() : 10.0;
    double x2 = obj.count("field2") > 0 ? obj["field2"].as<double>() : 20.0;

    string_type x3 = obj.get("field3","Montreal").as<string_type>();
    string_type x4 = obj.get("field4","San Francisco").as<string_type>();

    std::cout << "x1=" << x1 << std::endl;
    std::cout << "x2=" << x2 << std::endl;
    std::cout << "x3=" << x3 << std::endl;
    std::cout << "x4=" << x4 << std::endl;
```
The output is
```c++
    x1=1
    x2=20
    x3=Toronto
    x4=San Francisco
```
### Nulls
```c++
    json obj;
    obj["field1"] = json::null_type();
    std::cout << obj << std::endl;
```
The output is 
```json
    {"field1":null}
```
### Array
```c++
    json arr = json::make_array();
    arr.add(10);
    arr.add(20);
    arr.add(30);

    std::cout << arr << std::endl;
```
The output is 
```json
    [10,20,30]
```
### Array from std::vector
```c++
    std::vector<int> v;
    v.push_back(10);
    v.push_back(20);
    v.push_back(30);

    json arr(v.begin(),v.end());

    std::cout << arr << std::endl;
```
The output is 
```c++
    [10,20,30]
```
### Object iterator
```c++
    json obj;
    obj["city"] = "Toronto";
    obj["province"] = "Ontario";
    obj["country"] = "Canada";

    for (auto it = obj.members().begin(); it != obj.members().end(); ++it)
    {
        std::cout << it->name() << "=" << it->value().as<string_type>() << std::endl;
    }
```
The output is
```c++
    city=Toronto
    country=Canada
    province=Ontario
```
### Array iterator
```c++
    json arr = {"Toronto", "Vancouver", "Montreal"};

    for (auto it = arr.elements().begin(); it != arr.elements().end(); ++it)
    {
        std::cout << it->as<string_type>() << std::endl;
    }
```
The output is
```json
    Toronto
    Vancouver 
    Montreal
```
### Constructing json structures
```c++
    json root;

    root["persons"] = json::make_array();

    json person;
    person["first_name"] = "John";
    person["last_name"] = "Smith";
    person["birth_date"] = "1972-01-30";
    
    json address;
    address["city"] = "Toronto";
    address["country"] = "Canada";
    
    person["address"] = std::move(address);

    root["persons"].add(std::move(person));

    std::cout << pretty_print(root) << std::endl;
```
The output is
```c++
    {
        "persons":
        [
            {
                "address":
                {
                    "city":"Toronto",
                    "country":"Canada"
                },
                "birth_date":"1972-01-30",
                "first_name":"John",
                "last_name":"Smith"
            }
        ]
    }
```
