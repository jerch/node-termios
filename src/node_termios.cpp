/* node_termios.cpp
 *
 * Copyright (C) 2017, 2020 Joerg Breitbart
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#include "node_termios.h"
#include "termios_basic.h"


void populate_symbol_maps(
    Local<Object> all,
    Local<Object> iflags,
    Local<Object> oflags,
    Local<Object> cflags,
    Local<Object> lflags,
    Local<Object> cc,
    Local<Object> jsactions,
    Local<Object> jsflushs,
    Local<Object> jsflows,
    Local<Object> jsbaudrates)
{
    // no platform switches here, simply test for existance of questionable symbols
    // _SAFE32 macro is used as a compile guard to spot symbol changes beyond 32 bit

    // c_iflag
    #if    _SAFE32_8(IGNBRK,BRKINT,IGNPAR,PARMRK,INPCK,ISTRIP,INLCR,ICRNL) \
        && _SAFE32_8(IUCLC,IXON,IXANY,IXOFF,IMAXBEL,IUTF8,0,0)

    TERMIOS_EXPORT(c_iflag, iflags, IGNBRK);
    TERMIOS_EXPORT(c_iflag, iflags, BRKINT);
    TERMIOS_EXPORT(c_iflag, iflags, IGNPAR);
    TERMIOS_EXPORT(c_iflag, iflags, PARMRK);
    TERMIOS_EXPORT(c_iflag, iflags, INPCK);
    TERMIOS_EXPORT(c_iflag, iflags, ISTRIP);
    TERMIOS_EXPORT(c_iflag, iflags, INLCR);
    TERMIOS_EXPORT(c_iflag, iflags, IGNCR);
    TERMIOS_EXPORT(c_iflag, iflags, ICRNL);
    #ifdef IUCLC
    TERMIOS_EXPORT(c_iflag, iflags, IUCLC);
    #endif
    TERMIOS_EXPORT(c_iflag, iflags, IXON);
    TERMIOS_EXPORT(c_iflag, iflags, IXANY);
    TERMIOS_EXPORT(c_iflag, iflags, IXOFF);
    TERMIOS_EXPORT(c_iflag, iflags, IMAXBEL);
    #ifdef IUTF8
    TERMIOS_EXPORT(c_iflag, iflags, IUTF8);
    #endif

    #else
    #error "iflag not in 32 bit range"
    #endif

    // c_oflag
    #if    _SAFE32_8(OPOST,OLCUC,ONLCR,OCRNL,ONOCR,ONLRET,OFILL,OFDEL) \
        && _SAFE32_8(NLDLY,CRDLY,TABDLY,BSDLY,VTDLY,FFDLY,TAB0,TAB3)   \
        && _SAFE32_1(ONOEOT)

    TERMIOS_EXPORT(c_oflag, oflags, OPOST);
    #ifdef OLCUC
    TERMIOS_EXPORT(c_oflag, oflags, OLCUC);
    #endif
    TERMIOS_EXPORT(c_oflag, oflags, ONLCR);
    TERMIOS_EXPORT(c_oflag, oflags, OCRNL);
    TERMIOS_EXPORT(c_oflag, oflags, ONOCR);
    TERMIOS_EXPORT(c_oflag, oflags, ONLRET);
    #ifdef OFILL
    TERMIOS_EXPORT(c_oflag, oflags, OFILL);
    #endif
    #ifdef OFDEL
    TERMIOS_EXPORT(c_oflag, oflags, OFDEL);
    #endif
    #ifdef NLDLY
    TERMIOS_EXPORT(c_oflag, oflags, NLDLY);
    #endif
    #ifdef CRDLY
    TERMIOS_EXPORT(c_oflag, oflags, CRDLY);
    #endif
    #ifdef TABDLY
    TERMIOS_EXPORT(c_oflag, oflags, TABDLY);
    #endif
    #ifdef BSDLY
    TERMIOS_EXPORT(c_oflag, oflags, BSDLY);
    #endif
    #ifdef VTDLY
    TERMIOS_EXPORT(c_oflag, oflags, VTDLY);
    #endif
    #ifdef FFDLY
    TERMIOS_EXPORT(c_oflag, oflags, FFDLY);
    #endif
    #ifdef TAB0
    TERMIOS_EXPORT(c_oflag, oflags, TAB0);
    #endif
    #ifdef TAB3
    TERMIOS_EXPORT(c_oflag, oflags, TAB3);
    #endif
    #ifdef ONOEOT
    TERMIOS_EXPORT(c_oflag, oflags, ONOEOT);
    #endif

    #else
    #error "oflag not in 32 bit range"
    #endif

    // c_cflag
    #if    _SAFE32_8(CBAUD,CBAUDEX,CSIZE,CS5,CS6,CS7,CS8,CSTOPB)           \
        && _SAFE32_8(CREAD,PARENB,PARODD,HUPCL,CLOCAL,LOBLK,CIBAUD,CMSPAR) \
        && _SAFE32_4(CRTSCTS,CCTS_OFLOW,CRTS_IFLOW,MDMBUF)

    #ifdef CBAUD
    TERMIOS_EXPORT(c_cflag, cflags, CBAUD);
    #endif
    #ifdef CBAUDEX
    TERMIOS_EXPORT(c_cflag, cflags, CBAUDEX);
    #endif
    TERMIOS_EXPORT(c_cflag, cflags, CSIZE);
    TERMIOS_EXPORT(c_cflag, cflags, CS5);
    TERMIOS_EXPORT(c_cflag, cflags, CS6);
    TERMIOS_EXPORT(c_cflag, cflags, CS7);
    TERMIOS_EXPORT(c_cflag, cflags, CS8);
    TERMIOS_EXPORT(c_cflag, cflags, CSTOPB);
    TERMIOS_EXPORT(c_cflag, cflags, CREAD);
    TERMIOS_EXPORT(c_cflag, cflags, PARENB);
    TERMIOS_EXPORT(c_cflag, cflags, PARODD);
    TERMIOS_EXPORT(c_cflag, cflags, HUPCL);
    TERMIOS_EXPORT(c_cflag, cflags, CLOCAL);
    #ifdef LOBLK
    TERMIOS_EXPORT(c_cflag, cflags, LOBLK);
    #endif
    #ifdef CIBAUD
    TERMIOS_EXPORT(c_cflag, cflags, CIBAUD);
    #endif
    #ifdef CMSPAR
    TERMIOS_EXPORT(c_cflag, cflags, CMSPAR);
    #endif
    TERMIOS_EXPORT(c_cflag, cflags, CRTSCTS);
    #ifdef CCTS_OFLOW
    TERMIOS_EXPORT(c_cflag, cflags, CCTS_OFLOW);
    #endif
    #ifdef CRTS_IFLOW
    TERMIOS_EXPORT(c_cflag, cflags, CRTS_IFLOW);
    #endif
    #ifdef MDMBUF
    TERMIOS_EXPORT(c_cflag, cflags, MDMBUF);
    #endif

    #else
    #error "cflag not in 32 bit range"
    #endif

    // c_lflag
    #if    _SAFE32_8(ISIG,ICANON,XCASE,ECHO,ECHOE,ECHOK,ECHONL,ECHOCTL)         \
        && _SAFE32_8(ECHOPRT,ECHOKE,DEFECHO,FLUSHO,NOFLSH,TOSTOP,PENDIN,IEXTEN) \
        && _SAFE32_4(ALTWERASE,EXTPROC,NOKERNINFO,0)

    TERMIOS_EXPORT(c_lflag, lflags, ISIG);
    TERMIOS_EXPORT(c_lflag, lflags, ICANON);
    #ifdef XCASE
    TERMIOS_EXPORT(c_lflag, lflags, XCASE);
    #endif
    TERMIOS_EXPORT(c_lflag, lflags, ECHO);
    TERMIOS_EXPORT(c_lflag, lflags, ECHOE);
    #ifdef ECHOK
    TERMIOS_EXPORT(c_lflag, lflags, ECHOK);
    #endif
    TERMIOS_EXPORT(c_lflag, lflags, ECHONL);
    TERMIOS_EXPORT(c_lflag, lflags, ECHOCTL);
    TERMIOS_EXPORT(c_lflag, lflags, ECHOPRT);
    TERMIOS_EXPORT(c_lflag, lflags, ECHOKE);
    #ifdef DEFECHO
    TERMIOS_EXPORT(c_lflag, lflags, DEFECHO);
    #endif
    TERMIOS_EXPORT(c_lflag, lflags, FLUSHO);
    TERMIOS_EXPORT(c_lflag, lflags, NOFLSH);
    TERMIOS_EXPORT(c_lflag, lflags, TOSTOP);
    TERMIOS_EXPORT(c_lflag, lflags, PENDIN);
    TERMIOS_EXPORT(c_lflag, lflags, IEXTEN);
    #ifdef ALTWERASE
    TERMIOS_EXPORT(c_lflag, lflags, ALTWERASE);
    #endif
    #ifdef EXTPROC
    TERMIOS_EXPORT(c_lflag, lflags, EXTPROC);
    #endif
    #ifdef NOKERNINFO
    TERMIOS_EXPORT(c_lflag, lflags, NOKERNINFO);
    #endif

    #else
    #error "lflag not in 32 bit range"
    #endif

    // c_cc
    TERMIOS_EXPORT(c_cc, cc, VDISCARD);
    #ifdef VDSUSP
    TERMIOS_EXPORT(c_cc, cc, VDSUSP);
    #endif
    TERMIOS_EXPORT(c_cc, cc, VEOF);
    TERMIOS_EXPORT(c_cc, cc, VEOL);
    TERMIOS_EXPORT(c_cc, cc, VEOL2);
    TERMIOS_EXPORT(c_cc, cc, VERASE);
    TERMIOS_EXPORT(c_cc, cc, VINTR);
    TERMIOS_EXPORT(c_cc, cc, VKILL);
    TERMIOS_EXPORT(c_cc, cc, VLNEXT);
    TERMIOS_EXPORT(c_cc, cc, VMIN);
    TERMIOS_EXPORT(c_cc, cc, VQUIT);
    TERMIOS_EXPORT(c_cc, cc, VREPRINT);
    TERMIOS_EXPORT(c_cc, cc, VSTART);
    #ifdef VSTATUS
    TERMIOS_EXPORT(c_cc, cc, VSTATUS);
    #endif
    TERMIOS_EXPORT(c_cc, cc, VSTOP);
    TERMIOS_EXPORT(c_cc, cc, VSUSP);
    #ifdef VSWTCH
    TERMIOS_EXPORT(c_cc, cc, VSWTCH);
    #endif
    TERMIOS_EXPORT(c_cc, cc, VTIME);
    TERMIOS_EXPORT(c_cc, cc, VWERASE);

    // optional_actions for tcsetattr
    TERMIOS_EXPORT(actions, jsactions, TCSANOW);
    TERMIOS_EXPORT(actions, jsactions, TCSADRAIN);
    TERMIOS_EXPORT(actions, jsactions, TCSAFLUSH);
    #ifdef TCSASOFT
    TERMIOS_EXPORT(actions, jsactions, TCSASOFT);
    #endif

    // tcflush queue_selectors
    TERMIOS_EXPORT(flushs, jsflushs, TCIFLUSH);
    TERMIOS_EXPORT(flushs, jsflushs, TCOFLUSH);
    TERMIOS_EXPORT(flushs, jsflushs, TCIOFLUSH);

    // tcflow actions
    TERMIOS_EXPORT(flows, jsflows, TCOOFF);
    TERMIOS_EXPORT(flows, jsflows, TCOON);
    TERMIOS_EXPORT(flows, jsflows, TCIOFF);
    TERMIOS_EXPORT(flows, jsflows, TCION);

    // baud rates
    TERMIOS_EXPORT(baudrates, jsbaudrates, B0);
    TERMIOS_EXPORT(baudrates, jsbaudrates, B50);
    TERMIOS_EXPORT(baudrates, jsbaudrates, B75);
    TERMIOS_EXPORT(baudrates, jsbaudrates, B110);
    TERMIOS_EXPORT(baudrates, jsbaudrates, B134);
    TERMIOS_EXPORT(baudrates, jsbaudrates, B150);
    TERMIOS_EXPORT(baudrates, jsbaudrates, B200);
    TERMIOS_EXPORT(baudrates, jsbaudrates, B300);
    TERMIOS_EXPORT(baudrates, jsbaudrates, B600);
    TERMIOS_EXPORT(baudrates, jsbaudrates, B1200);
    TERMIOS_EXPORT(baudrates, jsbaudrates, B1800);
    TERMIOS_EXPORT(baudrates, jsbaudrates, B2400);
    TERMIOS_EXPORT(baudrates, jsbaudrates, B4800);
    TERMIOS_EXPORT(baudrates, jsbaudrates, B9600);
    TERMIOS_EXPORT(baudrates, jsbaudrates, B19200);
    TERMIOS_EXPORT(baudrates, jsbaudrates, B38400);
    #ifdef B7200
    TERMIOS_EXPORT(baudrates, jsbaudrates, B7200);
    #endif
    #ifdef B14400
    TERMIOS_EXPORT(baudrates, jsbaudrates, B14400);
    #endif
    #ifdef B28800
    TERMIOS_EXPORT(baudrates, jsbaudrates, B28800);
    #endif
    TERMIOS_EXPORT(baudrates, jsbaudrates, B57600);
    #ifdef B76800
    TERMIOS_EXPORT(baudrates, jsbaudrates, B76800);
    #endif
    TERMIOS_EXPORT(baudrates, jsbaudrates, B115200);
    TERMIOS_EXPORT(baudrates, jsbaudrates, B230400);
    #ifdef B460800
    TERMIOS_EXPORT(baudrates, jsbaudrates, B460800);
    #endif
    #ifdef B500000
    TERMIOS_EXPORT(baudrates, jsbaudrates, B500000);
    #endif
    #ifdef B576000
    TERMIOS_EXPORT(baudrates, jsbaudrates, B576000);
    #endif
    #ifdef B921600
    TERMIOS_EXPORT(baudrates, jsbaudrates, B921600);
    #endif
    #ifdef B1000000
    TERMIOS_EXPORT(baudrates, jsbaudrates, B1000000);
    #endif
    #ifdef B1152000
    TERMIOS_EXPORT(baudrates, jsbaudrates, B1152000);
    #endif
    #ifdef B1500000
    TERMIOS_EXPORT(baudrates, jsbaudrates, B1500000);
    #endif
    #ifdef B2000000
    TERMIOS_EXPORT(baudrates, jsbaudrates, B2000000);
    #endif
    #ifdef B2500000
    TERMIOS_EXPORT(baudrates, jsbaudrates, B2500000);
    #endif
    #ifdef B3000000
    TERMIOS_EXPORT(baudrates, jsbaudrates, B3000000);
    #endif
    #ifdef B3500000
    TERMIOS_EXPORT(baudrates, jsbaudrates, B3500000);
    #endif
    #ifdef B4000000
    TERMIOS_EXPORT(baudrates, jsbaudrates, B4000000);
    #endif
    #ifdef EXTA
    TERMIOS_EXPORT(baudrates, jsbaudrates, EXTA);
    #endif
    #ifdef EXTB
    TERMIOS_EXPORT(baudrates, jsbaudrates, EXTB);
    #endif
}


NAN_MODULE_INIT(init) {
    Nan::HandleScope scope;

    // symbols
    // The symbols are grouped together by responsibility.
    // Additonally all known symbols can be found in `ALL_SYMBOLS`.
    Local<Object> all = Nan::New<Object>();
    Local<Object> iflags = Nan::New<Object>();
    Local<Object> oflags = Nan::New<Object>();
    Local<Object> cflags = Nan::New<Object>();
    Local<Object> lflags = Nan::New<Object>();
    Local<Object> cc = Nan::New<Object>();
    Local<Object> jsactions = Nan::New<Object>();
    Local<Object> jsflushs = Nan::New<Object>();
    Local<Object> jsflows = Nan::New<Object>();
    Local<Object> jsbaudrates = Nan::New<Object>();
    populate_symbol_maps(
        all, iflags, oflags, cflags, lflags,
        cc, jsactions, jsflushs, jsflows, jsbaudrates);
    MODULE_EXPORT("ALL_SYMBOLS", all);
    MODULE_EXPORT("IFLAGS", iflags);
    MODULE_EXPORT("OFLAGS", oflags);
    MODULE_EXPORT("CFLAGS", cflags);
    MODULE_EXPORT("LFLAGS", lflags);
    MODULE_EXPORT("CC", cc);
    MODULE_EXPORT("ACTION", jsactions);
    MODULE_EXPORT("FLUSH", jsflushs);
    MODULE_EXPORT("FLOW", jsflows);
    MODULE_EXPORT("BAUD", jsbaudrates);

    // helper functions - useful functions related to ttys
    MODULE_EXPORT("isatty", Nan::GetFunction(Nan::New<FunctionTemplate>(Isatty)).ToLocalChecked());
    MODULE_EXPORT("ttyname", Nan::GetFunction(Nan::New<FunctionTemplate>(Ttyname)).ToLocalChecked());
    MODULE_EXPORT("ptsname", Nan::GetFunction(Nan::New<FunctionTemplate>(Ptsname)).ToLocalChecked());
    MODULE_EXPORT("load_ttydefaults", Nan::GetFunction(Nan::New<FunctionTemplate>(Load_ttydefaults)).ToLocalChecked());

    // termios functions
    MODULE_EXPORT("tcgetattr", Nan::GetFunction(Nan::New<FunctionTemplate>(Tcgetattr)).ToLocalChecked());
    MODULE_EXPORT("tcsetattr", Nan::GetFunction(Nan::New<FunctionTemplate>(Tcsetattr)).ToLocalChecked());
    MODULE_EXPORT("tcsendbreak", Nan::GetFunction(Nan::New<FunctionTemplate>(Tcsendbreak)).ToLocalChecked());
    MODULE_EXPORT("tcdrain", Nan::GetFunction(Nan::New<FunctionTemplate>(Tcdrain)).ToLocalChecked());
    MODULE_EXPORT("tcflush", Nan::GetFunction(Nan::New<FunctionTemplate>(Tcflush)).ToLocalChecked());
    MODULE_EXPORT("tcflow", Nan::GetFunction(Nan::New<FunctionTemplate>(Tcflow)).ToLocalChecked());
    MODULE_EXPORT("cfgetispeed", Nan::GetFunction(Nan::New<FunctionTemplate>(Cfgetispeed)).ToLocalChecked());
    MODULE_EXPORT("cfgetospeed", Nan::GetFunction(Nan::New<FunctionTemplate>(Cfgetospeed)).ToLocalChecked());
    MODULE_EXPORT("cfsetispeed", Nan::GetFunction(Nan::New<FunctionTemplate>(Cfsetispeed)).ToLocalChecked());
    MODULE_EXPORT("cfsetospeed", Nan::GetFunction(Nan::New<FunctionTemplate>(Cfsetospeed)).ToLocalChecked());

    // explain termios structure
    // EXPLAIN_MEMBERS --> {symbol: {offset: 0, width: 4}}
    Local<Object> members = Nan::New<Object>();
    EXPLAIN_MEMBER(members, struct termios, c_iflag);
    EXPLAIN_MEMBER(members, struct termios, c_oflag);
    EXPLAIN_MEMBER(members, struct termios, c_cflag);
    EXPLAIN_MEMBER(members, struct termios, c_lflag);
    EXPLAIN_MEMBER_ARRAY(members, struct termios, c_cc, cc_t);

    Local<Object> termios_explain = Nan::New<Object>();
    Nan::Set(termios_explain, Nan::New<String>("size").ToLocalChecked(), Nan::New<Number>(sizeof(struct termios)));
    Nan::Set(termios_explain, Nan::New<String>("members").ToLocalChecked(), members);
    MODULE_EXPORT("EXPLAIN", termios_explain);
}

#ifdef NAN_MODULE_WORKER_ENABLED
    NAN_MODULE_WORKER_ENABLED(termios, init)
#else
    NODE_MODULE(termios, init)
#endif
