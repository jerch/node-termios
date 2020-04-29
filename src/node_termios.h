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

#endif // NODE_TERMIOS_H
