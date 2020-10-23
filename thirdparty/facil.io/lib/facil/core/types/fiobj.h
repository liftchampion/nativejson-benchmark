/*
Copyright: Boaz Segev, 2017-2018
License: MIT
*/
#ifndef H_FIOBJ_H
#define H_FIOBJ_H

#include "fiobj_ary.h"
#include "fiobj_data.h"
#include "fiobj_hash.h"
#include "fiobj_json.h"
#include "fiobj_numbers.h"
#include "fiobj_str.h"
#include "fiobject.h"

#include "fio_siphash.h"

#if DEBUG
FIO_INLINE void fiobj_test(void) {
  fiobj_test_string();
  fiobj_test_numbers();
  fiobj_test_array();
  fiobj_test_hash();
  fiobj_test_core();
  fiobj_data_test();
  fiobj_test_json();
}
#else
FIO_INLINE void fiobj_test(void) {
  fprintf(stderr, "ERROR: tesing functions only defined with DEBUG=1\n");
  exit(-1);
}
#endif
#undef FIO_INLINE
#endif
