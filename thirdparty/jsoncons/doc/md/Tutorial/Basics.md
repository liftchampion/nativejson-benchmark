```c++
    #include "jsoncons/json.hpp"

    using jsoncons::json;
    using jsoncons::pretty_print;
```
## Constructing a json value

### From a string
```c++
    std::string input = R"(
    {
        "first_name":"Jane",
        "last_name":"Roe",
        "events_attended":10
    }
    )";

    json val = json::parse(input);

    std::cout << val << std::endl;
```
The output is
```json
    {"events_attended":10,"first_name":"Jane","last_name":"Roe"}
```
Note that on output, object members are sorted alphabetically by name.

### From a file

The input file is
```json
    {
        // Members
        "members" : [
            {
                "first_name":"Jane",
                "last_name":"Roe",
                "events_attended":10,
                "accept_waiver_of_liability" : true
            },
            {
                 "first_name":"John",
                 "last_name":"Doe",
                 "events_attended":2,
                 "accept_waiver_of_liability" : true
             }
        ]
    }
```
`members.json`
```c++
    json val = json::parse_file("members.json");

    std::cout << pretty_print(val) << std::endl;
```
The output is
```json
    {
        "members":
        [
            {
                "accept_waiver_of_liability":true,
                "events_attended":10,
                "first_name":"Jane",
                "last_name":"Roe"
            },
            {
                "accept_waiver_of_liability":true,
                "events_attended":2,
                "first_name":"John",
                "last_name":"Doe"
            }
        ]
    }
```
Note that the parser accepts and ignores the single line comment that starts with the marker `//`.

### With code
```c++
    // A boolean value
    json flag(true);

    // A numeric value
    json number(10.5);

    // An object value with four members
    json obj;
    obj["first_name"] = "Jane";
    obj["last_name"] = "Roe";
    obj["events_attended"] = 10;
    obj["accept_waiver_of_liability"] = true;

    // An array value with four elements
    json arr = json::array();
    arr.add(json::null_type());
    arr.add(flag);
    arr.add(number);
    arr.add(obj);

    std::cout << pretty_print(arr) << std::endl;
```
The output is
```json
    [null,true,10.5,
        {
            "accept_waiver_of_liability":true,
            "events_attended":10,
            "first_name":"Jane",
            "last_name":"Roe"
        }
    ]
```
### From containers such as std::vector or std::list
```c++
    std::vector<int> vec;
    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);

    json val1(vec.begin(), vec.end());
    std::cout << val1 << std::endl;

    std::list<double> list;
    list.push_back(10.5);
    list.push_back(20.5);
    list.push_back(30.5);

    json val2(list.begin(), list.end());
    std::cout << val2 << std::endl;
```
The output is 
```json
    [10,20,30]
    [10.5,20.5,30.5]
```
## Accessing object members and array elements

### Accessing object members
```c++
    json obj;
    obj["first_name"] = "Jane";
    obj["last_name"] = "Roe";
    obj["events_attended"] = 10;
    obj["accept_waiver_of_liability"] = true;

    std::string first_name = obj["first_name"].as<std::string>();
    std::string last_name = obj.at("last_name").as<std::string>();
    int events_attended = obj["events_attended"].as<int>();
    bool accept_waiver_of_liability = obj["accept_waiver_of_liability"].as<bool>();

    std::cout << first_name << " " << last_name << ", " << events_attended 
              << ", " << accept_waiver_of_liability << std::endl;
```
The output is
```
    Jane Roe, 10, 1
```
### Missing values and defaults
```c++
    json obj;
    obj["first_name"] = "Jane";
    obj["last_name"] = "Roe";

    try
    {
        std::string experience = obj["outdoor_experience"].as<std::string>();
    }
    catch (const json_exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    std::string experience = obj.count("outdoor_experience") > 0 ? obj["outdoor_experience"].as<std::string>() : "";

    bool first_aid_certification = obj.get("first_aid_certification",false).as<bool>();

    std::cout << "experience=" << experience << ", first_aid_certification=" << first_aid_certification << std::endl;
```
The output is
```
    Member outdoor_experience not found.
    experience=, first_aid_certification=0
```
### Iterating over the members of an object
```c++
    json obj;
    obj["first_name"] = "Jane";
    obj["last_name"] = "Roe";
    obj["events_attended"] = 10;
    obj["accept_waiver_of_liability"] = true;

    for (auto it = obj.members().begin(); it != obj.members().end(); ++it)
    {
        std::cout << "name=" << it->name() << ", value=" << it->value().as<std::string>() << std::endl;
    }
```
The output is
```
    name=accept_waiver_of_liability, value=true
    name=events_attended, value=10
    name=first_name, value=Jane
    name=last_name, value=Roe
```
### Iterating over the elements of an array
```c++
    json arr = json::array();
    arr.add("Montreal");
    arr.add("Toronto");
    arr.add("Ottawa");
    arr.add("Vancouver");

    for (auto it = arr.elements().begin(); it != arr.elements().end(); ++it)
    {
        std::cout << it->as<std::string>() << std::endl;
    }
```
The output is
```
    Montreal
    Toronto
    Ottawa
    Vancouver
```
