#include "../test.h"

class StrlenParseResult : public ParseResultBase {
public:
    StrlenParseResult() : length() {}

    size_t length;
};

class StrlenStringResult : public StringResultBase {
public:
    StrlenStringResult() : s() {}

    virtual const char* c_str() const { return s; }
    
    char *s;
};

class StrlenTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "strlen (C)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif
	
#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        StrlenParseResult* pr = new StrlenParseResult;
        pr->length = ::strlen(json);
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        static char buf[65536] = {};
        (void)parseResult;

        StrlenStringResult* sr = new StrlenStringResult;
    	sr->s = buf;
        return sr;
    }
#endif
};

REGISTER_TEST(StrlenTest);
