// update.cpp

#include <ulib/utility/dir_walk.h>

#undef  PACKAGE
#define PACKAGE "update"
#undef  ARGS
#define ARGS ""

#define U_OPTIONS \
"purpose 'update index document files generated by index...'" \
"option c config     1 'path of configuration file' ''\n" \
"option a add        1 'list of files to add to index' ''\n" \
"option s substitute 1 'list of files to substitute in index' ''\n" \
"option d delete     1 'path of files to delete from index' ''\n"

#define U_CDB_CLASS URDB
#define U_RDB_OPEN_WORDS 0
#define U_RDB_OPEN_NAMES 0

#include "IR.h"

class Application : public IR {
public:

   ~Application()
      {
      U_TRACE(5, "Application::~Application()")
      }

   static void parse(void* name)
      {
      U_TRACE(5, "Application::parse(%p)", name)

      UPosting::filename->_assign((UStringRep*)name);

      IR::parse();
      }

   static void buildFilenameListFrom(UVector<UString>& vec, const UString& arg)
      {
      U_TRACE(5, "Application::buildFilenameListFrom(%p,%.*S)", &vec, U_STRING_TO_TRACE(arg))

      uint32_t pos;
      UTokenizer t(arg);
      UString dir, filename, filter;

      while (t.next(filename, ','))
         {
         if (filename.find_first_of("?*", 0, 2) == U_NOT_FOUND) vec.push(filename);
         else
            {
            pos = filename.find_last_of('/');

            U_INTERNAL_DUMP("pos = %u", pos)

            if (pos == U_NOT_FOUND)
               {
               UDirWalk dirwalk(0, U_STRING_TO_PARAM(filename));

               (void) dirwalk.walk(vec);
               }
            else
               {
               dir    = filename.substr(0U, pos);
               filter = filename.substr(pos + 1);

               UDirWalk dirwalk(dir, U_STRING_TO_PARAM(filter));

               (void) dirwalk.walk(vec);
               }
            }
         }
      }

   void run(int argc, char* argv[], char* env[])
      {
      U_TRACE(5, "Application::run(%d,%p,%p)", argc, argv, env)

      IR::run(argc, argv, env);

      if (UPosting::dir_content_as_doc) U_ERROR("sorry, not implemented");

      // manage options

      if (UApplication::isOptions())
         {
         opt_file_to_add = opt['a'];
         opt_file_to_sub = opt['s'];
         opt_file_to_del = opt['d'];
         }

      if (IR::openCDB(true))
         {
         // load all filenames in argument

         if (opt_file_to_add) buildFilenameListFrom(file_to_add, opt_file_to_add);
         if (opt_file_to_sub) buildFilenameListFrom(file_to_sub, opt_file_to_sub);
         if (opt_file_to_del) buildFilenameListFrom(file_to_del, opt_file_to_del);

         // process all filenames in argument

         IR::setBadWords();

         if (opt_file_to_add)
            {
            operation = 0; // add

            file_to_add.callForAllEntry(parse);
            }

         if (opt_file_to_sub)
            {
            operation = 1; // sub

            file_to_sub.callForAllEntry(parse);
            }

         if (opt_file_to_del)
            {
            operation = 2; // del

            file_to_del.callForAllEntry(parse);
            }

         // register to database (RDB)

         IR::closeCDB(true);
         IR::deleteDB(true);
         }
      }

private:
   UVector<UString> file_to_add, file_to_sub, file_to_del;
   UString opt_file_to_add, opt_file_to_sub, opt_file_to_del;
};

U_MAIN
