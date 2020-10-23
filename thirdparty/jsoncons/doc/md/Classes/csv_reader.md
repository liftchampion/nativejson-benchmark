    jsoncons::csv::csv_reader

The `csv_reader` class is an instantiation of the `basic_csv_reader` class template that uses `char` as the character type. It reads a [CSV file](http://tools.ietf.org/html/rfc4180) and produces JSON parse events.

### Header

    #include "jsoncons_ext/csv/csv_reader.hpp"

### Constructors

    csv_reader(std::istream& is,
               json_input_handler& handler)
Constructs a `csv_reader` that is associated with an input stream
`is` of CSV text and a [json_input_handler](json_input_handler) that receives
JSON events. Uses default [csv_parameters](csv_parameters).
You must ensure that the input stream and input handler exist as long as does `csv_reader`, as `csv_reader` holds pointers to but does not own these objects.

    csv_reader(std::istream& is,
               json_input_handler& handler,
               const csv_parameters& params)
Constructs a `csv_reader` that is associated with an input stream
`is` of CSV text, a [json_input_handler](json_input_handler) that receives
JSON events, and [csv_parameters](csv_parameters).
You must ensure that the input stream and input handler exist as long as does `csv_reader`, as `csv_reader` holds pointers to but does not own these objects.

    csv_reader(std::istream& is,
               json_input_handler& handler,
               parse_error_handler& err_handler)
Constructs a `csv_reader` that is associated with an input stream
`is` of CSV text, a [json_input_handler](json_input_handler) that receives
JSON events and the specified [parse_error_handler](parse_error_handler).
Uses default [csv_parameters](csv_parameters).
You must ensure that the input stream, input handler, and error handler exist as long as does `csv_reader`, as `csv_reader` holds pointers to but does not own these objects.

    csv_reader(std::istream& is,
               json_input_handler& handler,
               parse_error_handler& err_handler,
               const csv_parameters& params)
Constructs a `csv_reader` that is associated with an input stream
`is` of CSV text, a [json_input_handler](json_input_handler) that receives
JSON events, the specified [parse_error_handler](parse_error_handler),
and [csv_parameters](csv_parameters).
You must ensure that the input stream, input handler, and error handler exist as long as does `csv_reader`, as `csv_reader` holds pointers to but does not own these objects.

### Member functions

    bool eof() const
Returns `true` when there is no more data to be read from the stream, `false` otherwise

    void read()
Reports JSON related events for JSON objects, arrays, object members and array elements to a [json_input_handler](json_input_handler), such as a [json_deserializer](json_deserializer).
Throws [parse_exception](parse_exception) if parsing fails.

    size_t buffer_capacity() const

    void buffer_capacity(size_t buffer_capacity)

### Examples

### Reading a comma delimted file into an array of json values

#### Comma delimited input file 
```
    country_code,name
    ABW,ARUBA
    ATF,"FRENCH SOUTHERN TERRITORIES, D.R. OF"
    VUT,VANUATU
    WLF,WALLIS & FUTUNA ISLANDS
```
Note 

- The first record contains a header line, but we're going to ignore that and read the entire file as an array of arrays.
- The third record has a field value that contains an embedded comma, so it must be quoted.

#### Code
```c++
    std::string in_file = "countries.csv";
    std::ifstream is(in_file);

    json_deserializer handler;

    csv_reader reader(is,handler);
    reader.read();
    json countries = handler.get_result();

    std::cout << pretty_print(countries) << std::endl;
```
#### Output 
```
    [
        ["country_code","name"],
        ["ABW","ARUBA"],
        ["ATF","FRENCH SOUTHERN TERRITORIES, D.R. OF"],
        ["VUT","VANUATU"],
        ["WLF","WALLIS & FUTUNA ISLANDS"]
    ]
```
### Reading a tab delimted file into an array of json objects

#### Tab delimited input file
```
    employee-no employee-name   dept    salary  note
    00000001    Smith, Matthew  sales   150,000.00      
    00000002    Brown, Sarah    sales   89,000.00       
    00000003    Oberc, Scott    finance 110,000.00      
    00000004    Scott, Colette  sales   75,000.00       """Exemplary"" employee
    Dependable, trustworthy"
```
Note 

- The first record is a header line, which will be used to associate data values with names
- The fifth record has a field value that contains embedded quotes and a new line character, so it must be quoted and the embedded quotes escaped.

#### Code
```c++
    std::string in_file = "employees.txt";
    std::ifstream is(in_file);

    json_deserializer handler;
    csv_parameters params;
    params.field_delimiter = '\t'
          .assume_header = true;

    csv_reader reader(is,handler,params);
    reader.read();
    json employees = handler.get_result();

    std::cout << pretty_print(employees) << std::endl;
```

#### Output

```json
    [
        {
            "dept":"sales",
            "employee-name":"Smith, Matthew",
            "employee-no":"00000001",
            "note":"",
            "salary":"150,000.00"
        },
        {
            "dept":"sales",
            "employee-name":"Brown, Sarah",
            "employee-no":"00000002",
            "note":"",
            "salary":"89,000.00"
        },
        {
            "dept":"finance",
            "employee-name":"Oberc, Scott",
            "employee-no":"00000003",
            "note":"",
            "salary":"110,000.00"
        },
        {
            "dept":"sales",
            "employee-name":"Scott, Colette",
            "employee-no":"00000004",
            "note":"\"Exemplary\" employee\nDependable, trustworthy",
            "salary":"75,000.00"
        }
    ]
```

#### Reading the comma delimited file as an array of objects with user supplied columns names

Note 

- The first record contains a header line, but we're going to ignore that and use our own names for the fields.

#### Code
```c++
    std::string in_file = "countries.csv";
    std::ifstream is(in_file);

    json_deserializer handler;

    csv_parameters params;
    params.column_names({"Country Code","Name"})
          .header_lines(1);

    csv_reader reader(is,handler,params);
    reader.read();
    json countries = handler.get_result();

    std::cout << pretty_print(countries) << std::endl;
```

#### Output 
```json
    [
        {
            "Country Code":"ABW",
            "Name":"ARUBA"
        },
        {
            "Country Code":"ATF",
            "Name":"FRENCH SOUTHERN TERRITORIES, D.R. OF"
        },
        {
            "Country Code":"VUT",
            "Name":"VANUATU"
        },
        {
            "Country Code":"WLF",
            "Name":"WALLIS & FUTUNA ISLANDS"
        }
    ]
```
