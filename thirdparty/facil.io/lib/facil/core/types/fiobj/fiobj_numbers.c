/*
Copyright: Boaz Segev, 2017-2018
License: MIT
*/

#include "fiobj_numbers.h"
#include "fiobject.h"

#include <assert.h>
#include <errno.h>
#include <math.h>

/* *****************************************************************************
Numbers Type
***************************************************************************** */

typedef struct {
  fiobj_object_header_s head;
  intptr_t i;
} fiobj_num_s;

typedef struct {
  fiobj_object_header_s head;
  double f;
} fiobj_float_s;

#define obj2num(o) ((fiobj_num_s *)FIOBJ2PTR(o))
#define obj2float(o) ((fiobj_float_s *)FIOBJ2PTR(o))

/* *****************************************************************************
Numbers VTable
***************************************************************************** */

static __thread char num_buffer[512];

static intptr_t fio_i2i(const FIOBJ o) { return obj2num(o)->i; }
static intptr_t fio_f2i(const FIOBJ o) {
  return (intptr_t)floorl(obj2float(o)->f);
}
static double fio_i2f(const FIOBJ o) { return (double)obj2num(o)->i; }
static double fio_f2f(const FIOBJ o) { return obj2float(o)->f; }

static size_t fio_itrue(const FIOBJ o) { return (obj2num(o)->i != 0); }
static size_t fio_ftrue(const FIOBJ o) { return (obj2float(o)->f != 0); }

static fio_cstr_s fio_i2str(const FIOBJ o) {
  return (fio_cstr_s){
      .buffer = num_buffer, .len = fio_ltoa(num_buffer, obj2num(o)->i, 10),
  };
}
static fio_cstr_s fio_f2str(const FIOBJ o) {
  if (isnan(obj2float(o)->f))
    return (fio_cstr_s){.buffer = "NaN", .len = 3};
  else if (isinf(obj2float(o)->f)) {
    if (obj2float(o)->f > 0)
      return (fio_cstr_s){.buffer = "Infinity", .len = 8};
    else
      return (fio_cstr_s){.buffer = "-Infinity", .len = 9};
  }
  return (fio_cstr_s){
      .buffer = num_buffer, .len = fio_ftoa(num_buffer, obj2float(o)->f, 10),
  };
}

static size_t fiobj_i_is_eq(const FIOBJ self, const FIOBJ other) {
  return obj2num(self)->i == obj2num(other)->i;
}
static size_t fiobj_f_is_eq(const FIOBJ self, const FIOBJ other) {
  return obj2float(self)->f == obj2float(other)->f;
}

void fiobject___simple_dealloc(FIOBJ o, void (*task)(FIOBJ, void *), void *arg);
uintptr_t fiobject___noop_count(FIOBJ o);

const fiobj_object_vtable_s FIOBJECT_VTABLE_NUMBER = {
    .class_name = "Number",
    .to_i = fio_i2i,
    .to_f = fio_i2f,
    .to_str = fio_i2str,
    .is_true = fio_itrue,
    .is_eq = fiobj_i_is_eq,
    .count = fiobject___noop_count,
    .dealloc = fiobject___simple_dealloc,
};

const fiobj_object_vtable_s FIOBJECT_VTABLE_FLOAT = {
    .class_name = "Float",
    .to_i = fio_f2i,
    .to_f = fio_f2f,
    .is_true = fio_ftrue,
    .to_str = fio_f2str,
    .is_eq = fiobj_f_is_eq,
    .count = fiobject___noop_count,
    .dealloc = fiobject___simple_dealloc,
};

/* *****************************************************************************
Number API
***************************************************************************** */

/** Creates a Number object. Remember to use `fiobj_free`. */
FIOBJ fiobj_num_new_bignum(intptr_t num) {
  fiobj_num_s *o = malloc(sizeof(*o));
  if (!o) {
    perror("ERROR: fiobj number couldn't allocate memory");
    exit(errno);
  }
  *o = (fiobj_num_s){
      .head =
          {
              .type = FIOBJ_T_NUMBER, .ref = 1,
          },
      .i = num,
  };
  return (FIOBJ)o;
}

/** Mutates a Big Number object's value. Effects every object's reference! */
// void fiobj_num_set(FIOBJ target, intptr_t num) {
//   assert(FIOBJ_TYPE_IS(target, FIOBJ_T_NUMBER) &&
//   FIOBJ_IS_ALLOCATED(target)); obj2num(target)->i = num;
// }

/** Creates a temporary Number object. This ignores `fiobj_free`. */
FIOBJ fiobj_num_tmp(intptr_t num) {
  static __thread fiobj_num_s ret;
  ret = (fiobj_num_s){
      .head = {.type = FIOBJ_T_NUMBER, .ref = ((~(uint32_t)0) >> 4)}, .i = num,
  };
  return (FIOBJ)&ret;
}

/* *****************************************************************************
Float API
***************************************************************************** */

/** Creates a Float object. Remember to use `fiobj_free`.  */
FIOBJ fiobj_float_new(double num) {
  fiobj_float_s *o = malloc(sizeof(*o));
  if (!o) {
    perror("ERROR: fiobj float couldn't allocate memory");
    exit(errno);
  }
  *o = (fiobj_float_s){
      .head =
          {
              .type = FIOBJ_T_FLOAT, .ref = 1,
          },
      .f = num,
  };
  return (FIOBJ)o;
}

/** Mutates a Float object's value. Effects every object's reference!  */
void fiobj_float_set(FIOBJ obj, double num) {
  assert(FIOBJ_TYPE_IS(obj, FIOBJ_T_FLOAT));
  obj2float(obj)->f = num;
}

/** Creates a temporary Number object. This ignores `fiobj_free`. */
FIOBJ fiobj_float_tmp(double num) {
  static __thread fiobj_float_s ret;
  ret = (fiobj_float_s){
      .head =
          {
              .type = FIOBJ_T_FLOAT, .ref = ((~(uint32_t)0) >> 4),
          },
      .f = num,
  };
  return (FIOBJ)&ret;
}

/* *****************************************************************************
Number and Float Helpers
***************************************************************************** */
static const char hex_notation[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

/**
 * A helper function that converts between String data to a signed int64_t.
 *
 * Numbers are assumed to be in base 10. `0x##` (or `x##`) and `0b##` (or
 * `b##`) are recognized as base 16 and base 2 (binary MSB first)
 * respectively.
 */
intptr_t fio_atol(char **pstr) {
  /* use strtol ?*/
  char *str = *pstr;
  uintptr_t result = 0;
  uint8_t invert = 0;
  while (str[0] == '-') {
    invert ^= 1;
    str++;
  }
  if (str[0] == 'B' || str[0] == 'b' ||
      (str[0] == '0' && (str[1] == 'b' || str[1] == 'B'))) {
    /* base 2 */
    if (str[0] == '0')
      str++;
    str++;
    while (str[0] == '0' || str[0] == '1') {
      result = (result << 1) | (str[0] == '1');
      str++;
    }
  } else if (str[0] == 'x' || str[0] == 'X' ||
             (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))) {
    /* base 16 */
    uint8_t tmp;
    if (str[0] == '0')
      str++;
    str++;
    while (1) {
      if (str[0] >= '0' && str[0] <= '9')
        tmp = str[0] - '0';
      else if (str[0] >= 'A' && str[0] <= 'F')
        tmp = str[0] - ('A' - 10);
      else if (str[0] >= 'a' && str[0] <= 'f')
        tmp = str[0] - ('a' - 10);
      else
        goto finish;
      result = (result << 4) | tmp;
      str++;
    }
  } else {
    /* base 10 */
    const char *end = str;
    while (end[0] >= '0' && end[0] <= '9' && (uintptr_t)(end - str) < 22)
      end++;
    if ((uintptr_t)(end - str) > 21) /* too large for a number */
      return 0;

    while (str < end) {
      result = (result * 10) + (str[0] - '0');
      str++;
    }
  }
finish:
  if (invert)
    result = 0 - result;
  *pstr = str;
  return (intptr_t)result;
}

/** A helper function that convers between String data to a signed double. */
double fio_atof(char **pstr) { return strtold(*pstr, pstr); }

/* *****************************************************************************
Numbers to Strings
***************************************************************************** */

/**
 * A helper function that convers between a signed int64_t to a string.
 *
 * No overflow guard is provided, make sure there's at least 66 bytes
 * available (for base 2).
 *
 * Supports base 2, base 10 and base 16. An unsupported base will silently
 * default to base 10. Prefixes aren't added (i.e., no "0x" or "0b" at the
 * beginning of the string).
 *
 * Returns the number of bytes actually written (excluding the NUL
 * terminator).
 */
size_t fio_ltoa(char *dest, int64_t num, uint8_t base) {
  if (!num) {
    *(dest++) = '0';
    *(dest++) = 0;
    return 1;
  }

  size_t len = 0;

  if (base == 2) {
    uint64_t n = num; /* avoid bit shifting inconsistencies with signed bit */
    uint8_t i = 0;    /* counting bits */

    while ((i < 64) && (n & 0x8000000000000000) == 0) {
      n = n << 1;
      i++;
    }
    /* make sure the Binary representation doesn't appear signed. */
    if (i) {
      dest[len++] = '0';
    }
    /* write to dest. */
    while (i < 64) {
      dest[len++] = ((n & 0x8000000000000000) ? '1' : '0');
      n = n << 1;
      i++;
    }
    dest[len] = 0;
    return len;

  } else if (base == 16) {
    uint64_t n = num; /* avoid bit shifting inconsistencies with signed bit */
    uint8_t i = 0;    /* counting bytes */
    while (i < 8 && (n & 0xFF00000000000000) == 0) {
      n = n << 8;
      i++;
    }
    /* make sure the Hex representation doesn't appear signed. */
    if (i && (n & 0x8000000000000000)) {
      dest[len++] = '0';
      dest[len++] = '0';
    }
    /* write the damn thing */
    while (i < 8) {
      uint8_t tmp = (n & 0xF000000000000000) >> 60;
      dest[len++] = hex_notation[tmp];
      tmp = (n & 0x0F00000000000000) >> 56;
      dest[len++] = hex_notation[tmp];
      i++;
      n = n << 8;
    }
    dest[len] = 0;
    return len;
  }

  /* fallback to base 10 */
  uint64_t rem = 0;
  uint64_t factor = 1;
  if (num < 0) {
    dest[len++] = '-';
    num = 0 - num;
  }

  while (num / factor)
    factor *= 10;

  while (factor > 1) {
    factor = factor / 10;
    rem = (rem * 10);
    dest[len++] = '0' + ((num / factor) - rem);
    rem += ((num / factor) - rem);
  }
  dest[len] = 0;
  return len;
}

/**
 * A helper function that convers between a double to a string.
 *
 * No overflow guard is provided, make sure there's at least 130 bytes
 * available (for base 2).
 *
 * Supports base 2, base 10 and base 16. An unsupported base will silently
 * default to base 10. Prefixes aren't added (i.e., no "0x" or "0b" at the
 * beginning of the string).
 *
 * Returns the number of bytes actually written (excluding the NUL
 * terminator).
 */
size_t fio_ftoa(char *dest, double num, uint8_t base) {
  if (base == 2 || base == 16) {
    /* handle the binary / Hex representation the same as if it were an
     * int64_t
     */
    int64_t *i = (void *)&num;
    return fio_ltoa(dest, *i, base);
  }

  size_t written = sprintf(dest, "%g", num);
  uint8_t need_zero = 1;
  char *start = dest;
  while (*start) {
    if (*start == ',') // locale issues?
      *start = '.';
    if (*start == '.' || *start == 'e') {
      need_zero = 0;
      break;
    }
    start++;
  }
  if (need_zero) {
    dest[written++] = '.';
    dest[written++] = '0';
  }
  return written;
}

static __thread char num_buffer[512];

fio_cstr_s fio_ltocstr(long i) {
  return (fio_cstr_s){.buffer = num_buffer, .len = fio_ltoa(num_buffer, i, 10)};
}
fio_cstr_s fio_ftocstr(double f) {
  return (fio_cstr_s){.buffer = num_buffer, .len = fio_ftoa(num_buffer, f, 10)};
}

/* *****************************************************************************
Tests
***************************************************************************** */

#if DEBUG
void fiobj_test_numbers(void) {
#define NUMTEST_ASSERT(cond, ...)                                              \
  if (!(cond)) {                                                               \
    fprintf(stderr, __VA_ARGS__);                                              \
    fprintf(stderr, "Testing failed.\n");                                      \
    exit(-1);                                                                  \
  }
  FIOBJ i = fiobj_num_new(8);
  fprintf(stderr, "=== Testing Numbers\n");
  NUMTEST_ASSERT(FIOBJ_TYPE_IS(i, FIOBJ_T_NUMBER),
                 "* FIOBJ_TYPE_IS failed to return true.");
  NUMTEST_ASSERT((FIOBJ_TYPE(i) == FIOBJ_T_NUMBER),
                 "* FIOBJ_TYPE failed to return type.");
  NUMTEST_ASSERT(!FIOBJ_TYPE_IS(i, FIOBJ_T_NULL),
                 "* FIOBJ_TYPE_IS failed to return false.");
  NUMTEST_ASSERT((i & FIOBJECT_NUMBER_FLAG),
                 "* Number 8 was dynamically allocated?! %p\n", (void *)i);
  NUMTEST_ASSERT((fiobj_obj2num(i) == 8), "* Number 8 was not returned! %p\n",
                 (void *)i);
  fiobj_free(i);
  i = fiobj_num_new(-1);
  NUMTEST_ASSERT((i & FIOBJECT_NUMBER_FLAG),
                 "* Number -1 was dynamically allocated?! %p\n", (void *)i);
  NUMTEST_ASSERT((fiobj_obj2num(i) == -1), "* Number -1 was not returned! %p\n",
                 (void *)i);
  fiobj_free(i);
  i = fiobj_num_new(INTPTR_MAX);
  NUMTEST_ASSERT((i & FIOBJECT_NUMBER_FLAG) == 0,
                 "* INTPTR_MAX was statically allocated?! %p\n", (void *)i);
  NUMTEST_ASSERT((fiobj_obj2num(i) == INTPTR_MAX),
                 "* INTPTR_MAX was not returned! %p\n", (void *)i);
  NUMTEST_ASSERT(
      FIOBJ_TYPE_IS(i, FIOBJ_T_NUMBER),
      "* FIOBJ_TYPE_IS failed to return true for dynamic allocation.");
  NUMTEST_ASSERT((FIOBJ_TYPE(i) == FIOBJ_T_NUMBER),
                 "* FIOBJ_TYPE failed to return type for dynamic allocation.");
  fiobj_free(i);
  i = fiobj_num_new(INTPTR_MIN);
  NUMTEST_ASSERT((i & FIOBJECT_NUMBER_FLAG) == 0,
                 "* INTPTR_MIN was statically allocated?! %p\n", (void *)i);
  NUMTEST_ASSERT((fiobj_obj2num(i) == INTPTR_MIN),
                 "* INTPTR_MIN was not returned! %p\n", (void *)i);
  fiobj_free(i);
  fprintf(stderr, "* passed.\n");
  fprintf(stderr, "=== Testing Floats\n");
  i = fiobj_float_new(1.0);
  NUMTEST_ASSERT(((i & FIOBJECT_NUMBER_FLAG) == 0),
                 "* float 1 was statically allocated?! %p\n", (void *)i);
  NUMTEST_ASSERT((fiobj_obj2float(i) == 1.0),
                 "* Float 1.0 was not returned! %p\n", (void *)i);
  fiobj_free(i);
  i = fiobj_float_new(-1.0);
  NUMTEST_ASSERT((i & FIOBJECT_NUMBER_FLAG) == 0,
                 "* Float -1 was statically allocated?! %p\n", (void *)i);
  NUMTEST_ASSERT((fiobj_obj2float(i) == -1.0),
                 "* Float -1 was not returned! %p\n", (void *)i);
  fiobj_free(i);
  fprintf(stderr, "* passed.\n");
}
#endif
