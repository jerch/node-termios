/* node_termios.h
 *
 * Copyright (C) 2017, 2020 Joerg Breitbart
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#ifndef NODE_TERMIOS_H
#define NODE_TERMIOS_H

#include "nan.h"
#include <termios.h>


using namespace node;
using namespace v8;

// solaris define
#if defined(sun) || defined(__sun)
  #if defined(__SVR4) || defined(__svr4__)
    #define SOLARIS 1
  # else
    #error "Old SunOS is not supported."
  # endif
#endif

// path length for ttyname / ptsname
#define CUSTOM_MAX_TTY_PATH 256

// macro for symbol export
#define TERMIOS_EXPORT(o, js_obj, sym)                                        \
Nan::Set(all, Nan::New<String>(#sym).ToLocalChecked(),Nan::New<Number>(sym)); \
Nan::Set(js_obj, Nan::New<String>(#sym).ToLocalChecked(),Nan::New<Number>(sym))


// macro for module export
#define MODULE_EXPORT(name, symbol)                                           \
Nan::Set(target, Nan::New<String>(name).ToLocalChecked(), symbol)

#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(exp)            \
  ({                                       \
    int _rc;                               \
    do {                                   \
      _rc = (exp);                         \
    } while (_rc == -1 && errno == EINTR); \
    _rc;                                   \
  })
#endif

// macros to explain structure
#define member_size(type, member) sizeof(((type *)0)->member)

#define EXPLAIN_MEMBER(T, S, M)                                                                  \
Local<Object> t##M = Nan::New<Object>();                                                         \
Nan::Set(t##M, Nan::New<String>("offset").ToLocalChecked(), Nan::New<Number>(offsetof(S, M)));   \
Nan::Set(t##M, Nan::New<String>("width").ToLocalChecked(), Nan::New<Number>(member_size(S, M))); \
Nan::Set(T, Nan::New<String>(#M).ToLocalChecked(), t##M)

#define EXPLAIN_MEMBER_ARRAY(T, S, M, MT)                                                        \
Local<Object> t##M = Nan::New<Object>();                                                         \
Nan::Set(t##M, Nan::New<String>("offset").ToLocalChecked(), Nan::New<Number>(offsetof(S, M)));   \
Nan::Set(t##M, Nan::New<String>("elem_size").ToLocalChecked(), Nan::New<Number>(sizeof(MT)));    \
Nan::Set(t##M, Nan::New<String>("width").ToLocalChecked(), Nan::New<Number>(member_size(S, M))); \
Nan::Set(T, Nan::New<String>(#M).ToLocalChecked(), t##M)

// macro to spot symbol overflow in 32bit
#define _SAFE32_1(X1) 0 <= X1 && X1 <= 0xFFFFFFFF
#define _SAFE32_2(X1, X2) _SAFE32_1(X1) && _SAFE32_1(X2)
#define _SAFE32_4(X1, X2, X3, X4) _SAFE32_2(X1, X2) && _SAFE32_2(X3, X4)
#define _SAFE32_8(X1, X2, X3, X4, X5, X6, X7, X8) _SAFE32_4(X1, X2, X3, X4) && _SAFE32_4(X5, X6, X7, X8)

#endif // NODE_TERMIOS_H
