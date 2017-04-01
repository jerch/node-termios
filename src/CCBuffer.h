/* CCBuffer.h
 *
 * Copyright (C) 2017 Joerg Breitbart
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#ifndef CCBUFFER_H
#define CCBUFFER_H

#include "node_termios.h"
#include "CTermios.h"

class CCBuffer : public Nan::ObjectWrap {
friend CTermios;
public:
    static v8::Local<v8::FunctionTemplate> init();
private:
    explicit CCBuffer(cc_t*, int);
    ~CCBuffer();

    static NAN_METHOD(New);
    static NAN_METHOD(ToBuffer);
    static inline Nan::Persistent<v8::Function> & constructor();
    static inline Nan::Persistent<v8::FunctionTemplate> & tmpl();

    static NAN_INDEX_GETTER(IndexGetter);
    static NAN_INDEX_SETTER(IndexSetter);
    static NAN_INDEX_ENUMERATOR(IndexEnumerator);
    static NAN_INDEX_DELETER(IndexDeleter);
    static NAN_INDEX_QUERY(IndexQuery);

    cc_t *value_;
    unsigned int length_;
};

#endif // CCBUFFER_H
