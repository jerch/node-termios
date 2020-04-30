/* termios_basic.cpp
 *
 * Copyright (C) 2017, 2020 Joerg Breitbart
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#include "termios_basic.h"
#include <errno.h>

NAN_METHOD(Isatty)
{
    Nan::HandleScope scope;
    if (info.Length() != 1 || !info[0]->IsNumber()) {
        return Nan::ThrowError("usage: termios.isatty(fd)");
    }
    int tty = isatty(Nan::To<int>(info[0]).FromJust());
    if (!tty && errno == EBADF) {
        std::string error(strerror(errno));
        return Nan::ThrowError((std::string("isatty failed - ") + error).c_str());
    }
    info.GetReturnValue().Set(Nan::New<Boolean>(tty));
}


NAN_METHOD(Ttyname)
{
    Nan::HandleScope scope;
    if (info.Length() != 1 || !info[0]->IsNumber()) {
        return Nan::ThrowError("usage: termios.ttyname(fd)");
    }
    #ifdef SOLARIS
    #define _POSIX_PTHREAD_SEMANTICS
    char buf[_POSIX_PATH_MAX] = {0};
    int res = ttyname_r(Nan::To<int>(info[0]).FromJust(), buf, _POSIX_PATH_MAX);
    #else
    char buf[TTY_NAME_MAX] = {0};
    int res = ttyname_r(Nan::To<int>(info[0]).FromJust(), buf, TTY_NAME_MAX);
    #endif
    info.GetReturnValue().Set(
        (res) ? Nan::EmptyString() : Nan::New<String>(buf).ToLocalChecked());
}


NAN_METHOD(Ptsname)
{
    Nan::HandleScope scope;
    if (info.Length() != 1 || !info[0]->IsNumber()) {
        return Nan::ThrowError("usage: termios.ptsname(fd)");
    }
    char *name = ptsname(Nan::To<int>(info[0]).FromJust());
    info.GetReturnValue().Set(
        (name) ? Nan::New<String>(name).ToLocalChecked() : Nan::EmptyString());
}


NAN_METHOD(Tcgetattr)
{
    Nan::HandleScope scope;
    if (info.Length() != 2
          || !info[0]->IsNumber()
          || !info[1]->IsObject()) {
        return Nan::ThrowError("Usage: tcgetattr(fd, buffer)");
    }
    if (!Buffer::HasInstance(info[1]) || Buffer::Length(info[1]) != sizeof(struct termios)) {
        return Nan::ThrowError("wrong buffer type");
    }
    struct termios* buf = (struct termios *) Buffer::Data(info[1]);

    int res;
    TEMP_FAILURE_RETRY(res = tcgetattr(Nan::To<int>(info[0]).FromJust(), buf));
    if (res) {
        std::string error(strerror(errno));
        return Nan::ThrowError((std::string("tcgetattr failed - ") + error).c_str());
    }
    info.GetReturnValue().SetUndefined();
}


NAN_METHOD(Tcsetattr)
{
    Nan::HandleScope scope;
    if (info.Length() != 3
          || !info[0]->IsNumber()
          || !info[1]->IsNumber()
          || !info[2]->IsObject()) {
        return Nan::ThrowError("Usage: tcsetattr(fd, action, buffer)");
    }

    if (!Buffer::HasInstance(info[2]) || Buffer::Length(info[2]) != sizeof(struct termios)) {
        return Nan::ThrowError("wrong buffer type");
    }
    struct termios *buf = (struct termios *) Buffer::Data(info[2]);

    int res;
    TEMP_FAILURE_RETRY(res = tcsetattr(Nan::To<int>(info[0]).FromJust(), Nan::To<int>(info[1]).FromJust(), buf));
    if (res) {
        std::string error(strerror(errno));
        return Nan::ThrowError((std::string("tcsetattr failed - ") + error).c_str());
    }
    info.GetReturnValue().SetUndefined();
}


NAN_METHOD(Tcsendbreak)
{
    Nan::HandleScope scope;
    if (info.Length() != 2
          || !info[0]->IsNumber()
          || !info[1]->IsNumber()) {
        return Nan::ThrowError("usage: termios.tcsendbreak(fd, duration)");
    }
    int res;
    TEMP_FAILURE_RETRY(res = tcsendbreak(Nan::To<int>(info[0]).FromJust(), Nan::To<int>(info[1]).FromJust()));
    if (res) {
        std::string error(strerror(errno));
        return Nan::ThrowError((std::string("tcsendbreak failed - ") + error).c_str());
    }
    info.GetReturnValue().SetUndefined();
}


NAN_METHOD(Tcdrain)
{
    Nan::HandleScope scope;
    if (info.Length() != 1 || !info[0]->IsNumber()) {
        return Nan::ThrowError("usage: termios.tcdrain(fd)");
    }
    int res;
    TEMP_FAILURE_RETRY(res = tcdrain(Nan::To<int>(info[0]).FromJust()));
    if (res) {
        std::string error(strerror(errno));
        return Nan::ThrowError((std::string("tcdrain failed - ") + error).c_str());
    }
    info.GetReturnValue().SetUndefined();
}


NAN_METHOD(Tcflush)
{
    Nan::HandleScope scope;
    if (info.Length() != 2
          || !info[0]->IsNumber()
          || !info[1]->IsNumber()) {
        return Nan::ThrowError("usage: termios.tcflush(fd, queue_selector)");
    }
    int res;
    TEMP_FAILURE_RETRY(res = tcflush(Nan::To<int>(info[0]).FromJust(), Nan::To<int>(info[1]).FromJust()));
    if (res) {
        std::string error(strerror(errno));
        return Nan::ThrowError((std::string("tcflush failed - ") + error).c_str());
    }
    info.GetReturnValue().SetUndefined();
}


NAN_METHOD(Tcflow)
{
    Nan::HandleScope scope;
    if (info.Length() != 2
          || !info[0]->IsNumber()
          || !info[1]->IsNumber()) {
        return Nan::ThrowError("usage: termios.tcflow(fd, action)");
    }
    int res;
    TEMP_FAILURE_RETRY(res = tcflow(Nan::To<int>(info[0]).FromJust(), Nan::To<int>(info[1]).FromJust()));
    if (res) {
        std::string error(strerror(errno));
        return Nan::ThrowError((std::string("tcflow failed - ") + error).c_str());
    }
    info.GetReturnValue().SetUndefined();
}


NAN_METHOD(Cfgetispeed)
{
    Nan::HandleScope scope;
    if (info.Length() != 1
          || !info[0]->IsObject()) {
        return Nan::ThrowError("usage: termios.cfgetispeed(buffer)");
    }
    if (!Buffer::HasInstance(info[0]) || Buffer::Length(info[0]) != sizeof(struct termios)) {
        return Nan::ThrowError("wrong buffer type");
    }
    struct termios *buf = (struct termios *) Buffer::Data(info[0]);
    info.GetReturnValue().Set(Nan::New<Number>(cfgetispeed(buf)));
}


NAN_METHOD(Cfgetospeed)
{
    Nan::HandleScope scope;
    if (info.Length() != 1
          || !info[0]->IsObject()) {
        return Nan::ThrowError("usage: termios.cfgetospeed(buffer)");
    }
    if (!Buffer::HasInstance(info[0]) || Buffer::Length(info[0]) != sizeof(struct termios)) {
        return Nan::ThrowError("wrong buffer type");
    }
    struct termios *buf = (struct termios *) Buffer::Data(info[0]);
    info.GetReturnValue().Set(Nan::New<Number>(cfgetospeed(buf)));
}


NAN_METHOD(Cfsetispeed)
{
    Nan::HandleScope scope;
    if (info.Length() != 2
          || !info[0]->IsObject()
          || !info[1]->IsNumber()) {
        return Nan::ThrowError("usage: termios.cfsetispeed(buffer, speed)");
    }
    if (!Buffer::HasInstance(info[0]) || Buffer::Length(info[0]) != sizeof(struct termios)) {
        return Nan::ThrowError("wrong buffer type");
    }
    struct termios *buf = (struct termios *) Buffer::Data(info[0]);
    if (cfsetispeed(buf, Nan::To<int>(info[1]).FromJust())) {
        std::string error(strerror(errno));
        return Nan::ThrowError((std::string("cfsetispeed failed - ") + error).c_str());
    }
    info.GetReturnValue().SetUndefined();
}


NAN_METHOD(Cfsetospeed)
{
    Nan::HandleScope scope;
    if (info.Length() != 2
          || !info[0]->IsObject()
          || !info[1]->IsNumber()) {
        return Nan::ThrowError("usage: termios.cfsetospeed(buffer, speed)");
    }
    if (!Buffer::HasInstance(info[0]) || Buffer::Length(info[0]) != sizeof(struct termios)) {
        return Nan::ThrowError("wrong buffer type");
    }
    struct termios *buf = (struct termios *) Buffer::Data(info[0]);
    if (cfsetospeed(buf, Nan::To<int>(info[1]).FromJust())) {
        std::string error(strerror(errno));
        return Nan::ThrowError((std::string("cfsetospeed failed - ") + error).c_str());
    }
    info.GetReturnValue().SetUndefined();
}
