/* termios_basic.cpp
 *
 * Copyright (C) 2017, 2020 Joerg Breitbart
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#include "termios_basic.h"
#include <errno.h>
#include <unistd.h>
#include <string.h>


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
    #endif
    char buf[CUSTOM_MAX_TTY_PATH] = {0};
    int res = ttyname_r(Nan::To<int>(info[0]).FromJust(), buf, CUSTOM_MAX_TTY_PATH);
    info.GetReturnValue().Set(
        (res) ? Nan::EmptyString() : Nan::New<String>(buf).ToLocalChecked());
}


/**
 * ptsname_r shims for various platforms without native implementation.
 */
#ifdef __APPLE__
#include <sys/ioctl.h>
#include <sys/stat.h>
int ptsname_r_darwin(int fd, char *buf, size_t buflen) {
    if (!isatty(fd)) {
        errno = ENOTTY;
        return ENOTTY;
    }
    char name[128] = "";  // hardcoded to 128 bytes for ioctl
    struct stat stat_buf;
    int error = ioctl(fd, TIOCPTYGNAME, name);
    if (!error) {
        if (stat(name, &stat_buf) == 0) {
            size_t length = strlen(name) + 1;
            if (length > buflen) {
                errno = ERANGE;
                return ERANGE;
            }
            strcpy(buf, name);
            return 0;
        }
    }
    errno = EINVAL;
    return EINVAL;
}
#endif

#ifdef __FreeBSD__
#include <sys/param.h>
#include <sys/ioctl.h>
#include <paths.h>
int ptsname_r_freebsd(int fd, char *buf, size_t buflen) {
    if(ioctl(fd, TIOCPTMASTER)) {
        errno = ENOTTY;
        return ENOTTY;
    }
    char name[sizeof(_PATH_DEV) + SPECNAMELEN] = _PATH_DEV;
    if (fdevname_r(fd, name + sizeof(_PATH_DEV) - 1, sizeof(name) - sizeof(_PATH_DEV) - 1) != NULL) {
        size_t length = strlen(name) + 1;
        if (length > buflen) {
            errno = ERANGE;
            return ERANGE;
        }
        strcpy(buf, name);
        return 0;
    }
    errno = EINVAL;
    return EINVAL;
}
#endif


NAN_METHOD(Ptsname)
{
    Nan::HandleScope scope;
    if (info.Length() != 1 || !info[0]->IsNumber()) {
        return Nan::ThrowError("usage: termios.ptsname(fd)");
    }
    #ifdef SOLARIS
        // solaris claims to have thread-safe ptsname
        char *buf = ptsname(Nan::To<int>(info[0]).FromJust());
        int res = (buf) ? 0 : 1;
    #else
        char buf[CUSTOM_MAX_TTY_PATH] = "";
        #ifdef __APPLE__
        int res = ptsname_r_darwin(Nan::To<int>(info[0]).FromJust(), buf, CUSTOM_MAX_TTY_PATH);
        #elif defined __FreeBSD__
        int res = ptsname_r_freebsd(Nan::To<int>(info[0]).FromJust(), buf, CUSTOM_MAX_TTY_PATH);
        #else
        int res = ptsname_r(Nan::To<int>(info[0]).FromJust(), buf, CUSTOM_MAX_TTY_PATH);
        #endif
    #endif
    info.GetReturnValue().Set(
        (res) ? Nan::EmptyString() : Nan::New<String>(buf).ToLocalChecked());
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

#if !defined(__sun) && !defined(__hpux) && !defined(_AIX)
#define LOAD_TTY_DEFAULTS 1
#include <sys/ttydefaults.h>
#endif

NAN_METHOD(Load_ttydefaults)
{
    Nan::HandleScope scope;
    if (info.Length() != 1
          || !info[0]->IsObject()) {
        return Nan::ThrowError("Usage: load_ttydefaults(buffer)");
    }
    #ifdef LOAD_TTY_DEFAULTS
    if (!Buffer::HasInstance(info[0]) || Buffer::Length(info[0]) != sizeof(struct termios)) {
        return Nan::ThrowError("wrong buffer type");
    }
    struct termios* buf = (struct termios *) Buffer::Data(info[0]);
    // load default flags
    buf->c_iflag = TTYDEF_IFLAG;
    buf->c_oflag = TTYDEF_OFLAG;
    buf->c_cflag = TTYDEF_CFLAG;
    buf->c_lflag = TTYDEF_LFLAG;
    // load default cc
    buf->c_cc[VEOF] = CEOF;
    buf->c_cc[VEOL] = CEOL;
    buf->c_cc[VERASE] = CERASE;
    buf->c_cc[VINTR] = CINTR;
    #ifdef VSTATUS
    buf->c_cc[VSTATUS] = CSTATUS;
    #endif
    buf->c_cc[VKILL] = CKILL;
    buf->c_cc[VMIN] = CMIN;
    buf->c_cc[VQUIT] = CQUIT;
    buf->c_cc[VSUSP] = CSUSP;
    buf->c_cc[VTIME] = CTIME;
    #ifdef VDSUSP
    buf->c_cc[VDSUSP] = CDSUSP;
    #endif
    buf->c_cc[VSTART] = CSTART;
    buf->c_cc[VSTOP] = CSTOP;
    buf->c_cc[VLNEXT] = CLNEXT;
    buf->c_cc[VDISCARD] = CDISCARD;
    buf->c_cc[VWERASE] = CWERASE;
    buf->c_cc[VREPRINT] = CREPRINT;
    // set default speeds
    cfsetispeed(buf, TTYDEF_SPEED);
    cfsetospeed(buf, TTYDEF_SPEED);
    info.GetReturnValue().Set(Nan::True());
    #else
    info.GetReturnValue().Set(Nan::False());
    #endif
}
