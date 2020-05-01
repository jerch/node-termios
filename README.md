## Access termios settings from nodejs.

The module exports a `Termios` class, that encapsulates termios struct data:

### Properties
    
- `c_iflag`: Integer representing the input mode flags.
- `c_oflag`: Integer representing the output mode flags.
- `c_cflag`: Integer representing the control mode flags.
- `c_lflag`: Integer representing the local mode flags.
- `c_cc`: Buffer representing the control code settings.

### Methods
    
- `constructor(from?: number | ITermios | null)`  
  Create new `Termios` object. `from` can be a valid file descriptor (number),
  another `Termios` object (copy constructor) or `null` (all data zeroed out).
  Omitting `from` will try to load default values from `ttydefaults.h` (not supported by all platforms).
- `loadFrom(fd: number): void`  
  Load termios data from file descriptor `fd`.
- `writeTo(fd: number, action?: number): void`  
  Write termios data to file descriptor `fd`. `action` should be one of `native.ACTION`
  (default: `native.ACTION.TCSAFLUSH`).
- `getInputSpeed(): number`  
  Return input channel baud rate setting as in `native.BAUD`.
- `getOutputSpeed(): number`  
  Return output channel baud rate setting as in `native.BAUD`.
- `setInputSpeed(speed: number): void`  
  Set input channel baud rate. `speed` should be one of the baudrates in `native.BAUD`.
- `setOutputSpeed(speed: number): void`  
  Set output channel baud rate. `speed` should be one of the baudrates in `native.BAUD`.
- `setSpeed(speed: number): void`  
  Set input and output channel baud rate. `speed` should be one of the baudrates in `native.BAUD`.
- `setraw(): void`  
  Convenient method to set termios data to raw mode (values taken from Python).
- `setcbreak(): void`  
  Convenient method to set termios data to cbreak mode (values taken from Python).
- `setcooked(): void`  
  Convenient method to set termios data back to cooked mode.

The module further exports known symbols defined by the
underlying termios.h (platform dependent) and low level functions under `native`:

- `ALL_SYMBOLS`: All known symbols.
- `IFLAGS`: Input mode symbols.
- `OFLAGS`: Output mode symbols.
- `CFLAGS`: Character mode symbols.
- `LFLAGS`: Local mode symbols.
- `CC`: Valid symbols defined for control character settings.
- `ACTION`: Actions defined for `tcsetattr`.
- `FLUSH`: Symbols for `tcflush`.
- `FLOW`: Symbols for `tcflow`.
- `BAUD`: Defined baudrates of the platform.
- `EXPLAIN`: `struct termios` member alignments and sizes.

### Low level functions

The low level function are direct mappings of the C functions,
where `fd` should be a valid TTY file descriptor
and `buffer` is a nodejs buffer of termios struct size (see `native.EXPLAIN.size`).
Additional enum like arguments are mapped in `native` (see listing above).

Also see `termios` manpage for further details.

- `isatty(fd: number): boolean`  
  Test if file descriptor `fd` is a tty. Might throw if `fd` is not valid.
- `ttyname(fd: number): string`  
  Return tty file name for `fd`, or empty string (invalid file descriptor).
- `ptsname(fd: number): string`  
  Return pts file name for file descriptor `fd`, or empty string (invalid file descriptor).
  Note that this only works on a master end of a PTY. For slave end use `ttyname`.
- `tcgetattr(fd: number, buffer: Buffer): void`  
  Load termios data for file descriptor `fd` in `buffer`. The given buffer must have a length
  of `native.EXPLAIN.size`.
- `tcsetattr(fd: number, action: number, buffer: Buffer): void`  
  Write termios data held in `buffer` to file descriptor `fd`. `action` should be one of `termios.ACTION`.
  The given buffer must have a length of `native.EXPLAIN.size`.
- `tcsendbreak(fd: number, duration: number): void`  
- `tcdrain(fd: number): void`  
- `tcflush(fd: number, queue_selector: number): void`
- `tcflow(fd: number, flowaction: number)`
- `cfgetispeed(buffer: Buffer): void`
- `cfgetospeed(buffer: Buffer)`
- `cfsetispeed(buffer: Buffer, speed: number): void`
- `cfsetospeed(buffer: Buffer, speed: number): void`


### Examples

The example demostrates how to switch off/on echoing on STDIN:
```javascript
const { Termios, native: { LFLAGS } } = require('node-termios');

// load termios data from STDIN
var t = new Termios(0);
// disable ECHO (not yet written)
t.c_lflag &= ~LFLAGS.ECHO;
// write back to STDIN
t.writeTo(0);
// now interactive input does not show up anymore
...

// switch on again when done
t.c_lflag |= LFLAGS.ECHO;
t.writeTo(0);
```

A typical usage pattern seen in C is to store the settings,
do some needed customizations like entering raw mode
and restoring old settings afterwards:
```javascript
const { Termios } = require('node-termios');

// save current settings to restore later on
const initial = new Termios(some_tty_fd);
// create copy and enter raw mode
const altered = new Termios(initial);
altered.setraw();
altered.writeTo(some_tty_fd);
// raw mode, thus every single data event shows up
...
// restore old settings
initial.writeTo(some_tty_fd);
```

Note that in the interactive nodejs shell the standard file descriptors are already
customized by the nodejs env and should not be handled like in the example above,
or the terminal might break with the shell expectations.

For a silly yet slightly more advanced example, see [example.js](./example.js).
