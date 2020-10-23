// SARU : Tag fastjson
// SARU : CxxLibs -L.. -lfastjson

#include "saru_cxx.h"
#include "fastjson/fastjson.h"

class TestFixture
{
  public:
    void test_string_buffer()
    {
      fastjson::ArrayEntry array_entries[2];
      unsigned char buffer[] = "xxxxxxxxxxxx"; 
      fastjson::Document doc;
      doc.string_store = buffer+1;
      doc.array_store = array_entries;

      bool ok = fastjson::parse_doc( "[\"hello\",\"world\"]", & doc );
      saru_assert(ok);

      saru_assert_equal( std::string("xhelloworldx"), std::string( reinterpret_cast<const char*>(buffer) ) );
    }

    void test_number_buffer()
    {
      fastjson::ArrayEntry array_entries[2];
      unsigned char buffer[] = "xxxxxxxxxxxx";
      fastjson::Document doc;
      doc.string_store = buffer+1;
      doc.array_store = array_entries;

      bool ok = fastjson::parse_doc( "[12345,67890]", & doc );
      saru_assert(ok);

      saru_assert_equal( std::string("x1234567890x"), std::string( reinterpret_cast<const char*>(buffer) ) );
    }

    void test_empty_array_root()
    {
      fastjson::Document doc;
      bool ok = fastjson::parse_doc( "[]", &doc );
      saru_assert( ok );
      
      //Now we should have a root array element with no children.
      saru_assert( doc.root.type == fastjson::Token::ArrayToken );
      saru_assert( doc.root.array.ptr == NULL ); 
    }

    void test_empty_dict_root()
    {
      fastjson::Document doc;
      bool ok = fastjson::parse_doc( "{}", &doc );
      saru_assert( ok );
      
      //Now we should have a root array element with no children.
      saru_assert( doc.root.type == fastjson::Token::DictToken );
      saru_assert( doc.root.dict.ptr == NULL ); 
    }

    void test_string_root()
    {
      unsigned char buffer[] = "xxxxxxxxxxxx";
      fastjson::Document doc;
      doc.string_store = buffer;
      bool ok = fastjson::parse_doc( "\"hello\"", &doc );
      saru_assert( ok );
      
      //Now we should have a root array element with no children.
      saru_assert( doc.root.type == fastjson::Token::ValueToken );
      saru_assert( doc.root.value.ptr == reinterpret_cast<char*>(buffer) ); 
      saru_assert_equal(5u, doc.root.value.size);
      saru_assert_equal( fastjson::ValueType::StringHint, doc.root.value.type_hint);
    }

    void test_number_root()
    {
      unsigned char buffer[] = "xxxxxxxxxxxx";
      fastjson::Document doc;
      doc.string_store = buffer;
      bool ok = fastjson::parse_doc( "123", &doc );
      saru_assert( ok );
      
      //Now we should have a root array element with no children.
      saru_assert( doc.root.type == fastjson::Token::ValueToken );
      saru_assert( doc.root.value.ptr == reinterpret_cast<char*>(buffer) ); 
      saru_assert_equal(3u, doc.root.value.size);
      saru_assert_equal( fastjson::ValueType::NumberHint, doc.root.value.type_hint);
    }

    void test_array_of_strings()
    {
      fastjson::ArrayEntry array_entries[2];
      unsigned char buffer[] = "xxxxxxxxxxxx";
      fastjson::Document doc;
      doc.string_store = buffer;
      doc.array_store  = array_entries;
      bool ok = fastjson::parse_doc( "[\"hello\",\"world\"]" , &doc );
      saru_assert(ok);
      
      //We should have go the root element as the array
      saru_assert( doc.root.type == fastjson::Token::ArrayToken );
      saru_assert_equal( &array_entries[0], doc.root.array.ptr );

      //Now we should have got two strings in the array
      saru_assert( array_entries[0].tok.type == fastjson::Token::ValueToken );
      saru_assert_equal( 5u, array_entries[0].tok.value.size );
      saru_assert( buffer == reinterpret_cast<unsigned char*>(array_entries[0].tok.value.ptr) ); 
      saru_assert_equal( &array_entries[1], array_entries[0].next );

      saru_assert( array_entries[1].tok.type == fastjson::Token::ValueToken );
      saru_assert_equal( 5u, array_entries[1].tok.value.size );
      saru_assert( buffer+5 == reinterpret_cast<unsigned char*>(array_entries[1].tok.value.ptr) ); 
      saru_assert_equal( (void*)NULL, array_entries[1].next );
    }

    void test_complex_dict()
    {
        fastjson::ArrayEntry array_entries[4];
        fastjson::DictEntry  dict_entries[2];
        unsigned char buffer[] = "xxxxxxx";
        fastjson::Document doc;
        doc.string_store = buffer;
        doc.array_store = array_entries;
        doc.dict_store  = dict_entries;

        bool ok = fastjson::parse_doc( "{\"f\":[\"g\",\"h\"],\"i\":[\"j\",\"k\"]}" , &doc );
        saru_assert(ok);

        //We should have go the root element as the array
        saru_assert_equal( fastjson::Token::DictToken, doc.root.type );
        saru_assert_equal( &dict_entries[0], doc.root.dict.ptr );

        //The first entry in the dictionary
        saru_assert_equal( &dict_entries[1],            dict_entries[0].next );
        //  First Key
        saru_assert_equal( fastjson::Token::ValueToken, dict_entries[0].key_tok.type );
        saru_assert_equal( fastjson::ValueType::StringHint,  dict_entries[0].key_tok.value.type_hint );
        saru_assert_equal( 1u,                          dict_entries[0].key_tok.value.size );
        saru_assert_equal( (void*)buffer,               (void*)dict_entries[0].key_tok.value.ptr );
        saru_assert_equal( 'f',                         buffer[0] );
        //  First Value
        saru_assert_equal( fastjson::Token::ArrayToken, dict_entries[0].value_tok.type );
        saru_assert_equal( array_entries,               dict_entries[0].value_tok.array.ptr );
        
        //The second entry in the dictionary
        saru_assert_equal( (void*)NULL,                 dict_entries[1].next);
        //  Second Key
        saru_assert_equal( fastjson::Token::ValueToken, dict_entries[1].key_tok.type );
        saru_assert_equal( fastjson::ValueType::StringHint,  dict_entries[1].key_tok.value.type_hint );
        saru_assert_equal( 1u,                          dict_entries[1].key_tok.value.size );
        saru_assert_equal( (void*)(buffer+3),           (void*)dict_entries[1].key_tok.value.ptr );
        saru_assert_equal( 'i',                         buffer[3] );

        //  Second Value
        saru_assert_equal( fastjson::Token::ArrayToken, dict_entries[1].value_tok.type );
        saru_assert_equal( array_entries+2,             dict_entries[1].value_tok.array.ptr );

        //Check the array entries.
        saru_assert_equal( array_entries+1,                 array_entries[0].next );
        saru_assert_equal( fastjson::Token::ValueToken,     array_entries[0].tok.type );
        saru_assert_equal( fastjson::ValueType::StringHint, array_entries[0].tok.value.type_hint );
        saru_assert_equal( 1u,                              array_entries[0].tok.value.size );
        saru_assert_equal( (void*)(buffer+1),        (void*)array_entries[0].tok.value.ptr );
        saru_assert_equal( 'g',                             buffer[1] );

        saru_assert_equal( (void*)0,                        array_entries[1].next );
        saru_assert_equal( fastjson::Token::ValueToken,     array_entries[1].tok.type );
        saru_assert_equal( fastjson::ValueType::StringHint, array_entries[1].tok.value.type_hint );
        saru_assert_equal( 1u,                              array_entries[1].tok.value.size );
        saru_assert_equal( (void*)(buffer+2),        (void*)array_entries[1].tok.value.ptr );
        saru_assert_equal( 'h',                             buffer[2] );

        saru_assert_equal( array_entries+3,                 array_entries[2].next );
        saru_assert_equal( fastjson::Token::ValueToken,     array_entries[2].tok.type );
        saru_assert_equal( fastjson::ValueType::StringHint, array_entries[2].tok.value.type_hint );
        saru_assert_equal( 1u,                              array_entries[2].tok.value.size );
        saru_assert_equal( (void*)(buffer+4),        (void*)array_entries[2].tok.value.ptr );
        saru_assert_equal( 'j',                             buffer[4] );

        saru_assert_equal( (void*)0,                        array_entries[3].next );
        saru_assert_equal( fastjson::Token::ValueToken,     array_entries[3].tok.type );
        saru_assert_equal( fastjson::ValueType::StringHint, array_entries[3].tok.value.type_hint );
        saru_assert_equal( 1u,                              array_entries[3].tok.value.size );
        saru_assert_equal( (void*)(buffer+5),        (void*)array_entries[3].tok.value.ptr );
        saru_assert_equal( 'k',                             buffer[5] );

    }
  

    void complex2()
    {
      fastjson::ArrayEntry array_entries[2];
      fastjson::DictEntry  dict_entries[4];
      unsigned char buffer[] = "xxxxxxxx"; //Eight characters
      fastjson::Document doc;
      doc.string_store = buffer;
      doc.array_store = array_entries;
      doc.dict_store  = dict_entries;
      bool ok = fastjson::parse_doc( "{\"f\":[\"g\",\"h\"],\"i\":{\"j\":\"k\"},\"l\":\"m\"}" , &doc );
      saru_assert(ok);
    }

    void naked_literal_true()
    {
      fastjson::Document doc;
      bool ok = fastjson::parse_doc( "true" , &doc );
      saru_assert(ok);

      saru_assert_equal( fastjson::Token::LiteralTrueToken, doc.root.type );
    }

    void naked_literal_false()
    {
      fastjson::Document doc;
      bool ok = fastjson::parse_doc( "false" , &doc );
      saru_assert(ok);

      saru_assert_equal( fastjson::Token::LiteralFalseToken, doc.root.type );
    }

    void naked_literal_null()
    {
      fastjson::Document doc;
      bool ok = fastjson::parse_doc( "null" , &doc );
      saru_assert(ok);

      saru_assert_equal( fastjson::Token::LiteralNullToken, doc.root.type );
    }

    void test_unicode_1byte()
    {
      unsigned char buffer[] = "xxxxxxx";
      fastjson::Document doc;
      doc.string_store = buffer+1;
      bool ok = fastjson::parse_doc( "\"\\u007F\"" , &doc );
      saru_assert_equal( fastjson::Token::ValueToken, doc.root.type );
      saru_assert_equal( 1u, doc.root.value.size );
      saru_assert_equal( (void*)(buffer+1), doc.root.value.ptr );
      saru_assert_equal( 'x', *buffer );
      saru_assert_equal( 0x7F, *(buffer+1) );
      saru_assert_equal( 'x', *(buffer+2) );
    }

    void test_unicode_2byte()
    {
      unsigned char buffer[] = "xxxxxxx";
      fastjson::Document doc;
      doc.string_store = buffer+1;
      bool ok = fastjson::parse_doc( "\"\\u00A2\"" , &doc );
      saru_assert_equal( fastjson::Token::ValueToken, doc.root.type );
      saru_assert_equal( 2u, doc.root.value.size );
      saru_assert_equal( (void*)(buffer+1), doc.root.value.ptr );
      saru_assert_equal( 'x', *buffer );
      saru_assert_equal( 0xC2, *(buffer+1) );
      saru_assert_equal( 0xA2, *(buffer+2) );
      saru_assert_equal( 'x', *(buffer+3) );
    }

    void test_unicode_3byte()
    {
      unsigned char buffer[] = "xxxxxxx";
      fastjson::Document doc;
      doc.string_store = buffer+1;
      bool ok = fastjson::parse_doc( "\"z\\u20ACz\"" , &doc );
      saru_assert_equal( fastjson::Token::ValueToken, doc.root.type );
      saru_assert_equal( 5u, doc.root.value.size );
      saru_assert_equal( (void*)(buffer+1), doc.root.value.ptr );
      saru_assert_equal( 'x', *buffer );
      saru_assert_equal( 'z', *(buffer+1) );
      saru_assert_equal( 0xE2, *(buffer+2) );
      saru_assert_equal( 0x82, *(buffer+3) );
      saru_assert_equal( 0xAC, *(buffer+4) );
      saru_assert_equal( 'z', *(buffer+5) );
      saru_assert_equal( 'x', *(buffer+6) );
    }
};

int main()
{
  saru::TestLogger logger;
  SARU_TEST( TestFixture::test_string_buffer, logger);
  SARU_TEST( TestFixture::test_number_buffer, logger);
  SARU_TEST( TestFixture::test_empty_array_root, logger);
  SARU_TEST( TestFixture::test_empty_dict_root, logger);
  SARU_TEST( TestFixture::test_string_root, logger);
  SARU_TEST( TestFixture::test_number_root, logger);
  SARU_TEST( TestFixture::test_array_of_strings, logger);
  SARU_TEST( TestFixture::test_complex_dict, logger );
  SARU_TEST( TestFixture::complex2, logger );
  SARU_TEST( TestFixture::naked_literal_true, logger );
  SARU_TEST( TestFixture::naked_literal_false, logger );
  SARU_TEST( TestFixture::naked_literal_null, logger );
  SARU_TEST( TestFixture::test_unicode_1byte, logger );
  SARU_TEST( TestFixture::test_unicode_2byte, logger );
  SARU_TEST( TestFixture::test_unicode_3byte, logger );

  logger.printSummary();

  return logger.allOK()?0:1;
}

