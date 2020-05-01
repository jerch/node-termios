import { assert } from 'chai';
import { native, Termios } from '.';
import * as pty from 'node-pty';
import { platform } from 'os';

/**
 * Note: Tests need stdin (0) to be a real TTY.
 */

describe('native functions', () => {
  describe('isatty', () => {
    it('true on stdin', () => {
      assert.equal(native.isatty(0), true);
    });
    it('false on non tty fd', () => {
      const fs = require('fs');
      const fd = fs.openSync('/', 'r');
      assert.equal(native.isatty(fd), false);
      fs.closeSync(fd);
    });
    it('should throw for illegal fd (except on solaris)', () => {
      if (platform() === 'sunos') {
        assert.equal(native.isatty(12345), false);
      } else {
        assert.throws(() => native.isatty(12345));
      }
    });
  });
  describe('ttyname', () => {
    it('should return path string on tty fd', () => {
      assert.notEqual(native.ttyname(0), '');
    });
    it('should return empty string on non tty fd', () => {
      const fs = require('fs');
      const fd = fs.openSync('/', 'r');
      assert.equal(native.ttyname(fd), '');
      fs.closeSync(fd);
    });
  });
  describe('ptsname', () => {
    it('should return path string on pty master fd', (done) => {
      const p = pty.spawn('sh', [], {});
      const path = native.ptsname((p as any)._fd);
      assert.notEqual(path, '');
      const fromPty: string[] = [];
      p.onData(data => fromPty.push(data));
      p.write('tty\r');  // report ttyname from slave end
      setTimeout(() => {
        p.kill();
        // ttyname on slave should return ptsname on master
        assert.equal(fromPty.join('').includes(path), true);
        done();
      }, 100);
    });
    it('should return empty string on non pty master fd', () => {
      const fs = require('fs');
      const fd = fs.openSync('/', 'r');
      assert.equal(native.ptsname(fd), '');
      fs.closeSync(fd);
      assert.equal(native.ptsname(0), '');
    });
  });
  describe('tcgetattr', () => {
    it('should load data into buffer', () => {
      const buf = Buffer.from(Array(native.EXPLAIN.size));
      native.tcgetattr(0, buf);
      assert.notDeepEqual(buf, Buffer.from(Array(native.EXPLAIN.size)));
    });
    it('should reject wrong arguments', () => {
      // illegal fd
      assert.throws(() => native.tcgetattr(-1, Buffer.from(Array(native.EXPLAIN.size))));
      const fs = require('fs');
      const fd = fs.openSync('/', 'r');
      assert.throws(() => native.tcgetattr(fd, Buffer.from(Array(native.EXPLAIN.size))));
      fs.closeSync(fd);
      // misaligned buffer
      assert.throws(() => native.tcgetattr(0, Buffer.from(Array(10))), 'wrong buffer type');
    });
  });
  describe('tcsetattr', () => {
    let oldAttrs: Buffer;
    beforeEach(() => {
      oldAttrs = Buffer.from(Array(native.EXPLAIN.size));
      native.tcgetattr(0, oldAttrs);
    });
    afterEach(() => {
      native.tcsetattr(0, native.ACTION.TCSANOW, oldAttrs);
    });
    it('read/write/read test', () => {
      // not all values are nullable, we test only for iflag and oflag
      const attr = Buffer.from(Array(native.EXPLAIN.size));
      native.tcgetattr(0, attr);
      attr.subarray(0, 8).fill(0); // --> spans iflag and oflag byte range
      native.tcsetattr(0, native.ACTION.TCSANOW, attr);
      const attrRead = Buffer.from(Array(native.EXPLAIN.size));
      native.tcgetattr(0, attrRead);
      assert.deepEqual(attrRead, attr);
      native.tcsetattr(0, native.ACTION.TCSANOW, oldAttrs);
    });
    it('should reject wrong arguments', () => {
      // illegal fd
      assert.throws(() => native.tcsetattr(-1,native.ACTION.TCSANOW, Buffer.from(Array(native.EXPLAIN.size))));
      const fs = require('fs');
      const fd = fs.openSync('/', 'r');
      assert.throws(() => native.tcsetattr(fd, native.ACTION.TCSANOW, Buffer.from(Array(native.EXPLAIN.size))));
      fs.closeSync(fd);
      // misaligned buffer
      assert.throws(() => native.tcsetattr(0, native.ACTION.TCSANOW, Buffer.from(Array(10))), 'wrong buffer type');
    });
  });
  /**
   * Note tested:
   *    tcsendbreak, tcdrain, tcflush, tcflow
   */
});

describe('Termios', () => {
  it('ctor from valid tty fd', () => {
    const t = new Termios(0);
    assert.notEqual(t.c_iflag, 0);
    assert.notEqual(t.c_oflag, 0);
    assert.notEqual(t.c_cflag, 0);
    assert.notEqual(t.c_lflag, 0);
  });
  it('ctor from other Termios object', () => {
    const t1 = new Termios(0);
    const t2 = new Termios(t1);
    assert.equal(t2.c_iflag, t1.c_iflag);
    assert.equal(t2.c_oflag, t1.c_oflag);
    assert.equal(t2.c_cflag, t1.c_cflag);
    assert.equal(t2.c_lflag, t1.c_lflag);
    assert.deepEqual(t2.c_cc, t1.c_cc);
    assert.deepEqual((t2 as any)._data, (t1 as any)._data);
  });
  it('ctor from explicit null should create empty', () => {
    const t = new Termios(null);
    assert.equal(t.c_iflag, 0);
    assert.equal(t.c_oflag, 0);
    assert.equal(t.c_cflag, 0);
    assert.equal(t.c_lflag, 0);
    assert.deepEqual(t.c_cc, Buffer.from(Array(native.EXPLAIN.members.c_cc.width)));
    assert.deepEqual((t as any)._data, Buffer.from(Array(native.EXPLAIN.size)));
  });
  it('ctor from undefined should pull ttydefaults.h', () => {
    // Note - ttydefaults.h not supported on solaris
    const UNSUPPORTED = ['sunos', 'aix'];
    const t = new Termios();
    if (UNSUPPORTED.indexOf(platform()) !== -1) {
      assert.equal(t.c_iflag, 0);
      assert.equal(t.c_oflag, 0);
      assert.equal(t.c_cflag, 0);
      assert.equal(t.c_lflag, 0);
      assert.deepEqual(t.c_cc, Buffer.from(Array(native.EXPLAIN.members.c_cc.width)));
    } else {
      assert.notEqual(t.c_iflag, 0);
      assert.notEqual(t.c_oflag, 0);
      assert.notEqual(t.c_cflag, 0);
      assert.notEqual(t.c_lflag, 0);
      // test typical default control codes from ttydefaults.h
      const ctrl = (c: string) => c.charCodeAt(0) & 31;
      assert.equal(t.c_cc[native.CC.VEOF], ctrl('d'));
      assert.equal(t.c_cc[native.CC.VINTR], ctrl('c'));
      assert.equal(t.c_cc[native.CC.VSTART], ctrl('q'));
      assert.equal(t.c_cc[native.CC.VSTOP], ctrl('s'));
      assert.equal(t.c_cc[native.CC.VSUSP], ctrl('z'));
      assert.equal(t.c_cc[native.CC.VREPRINT], ctrl('r'));
    }
  });
  it('ctor rejects invalid arguments', () => {
    // invalid fd
    assert.throws(() => new Termios(-1));
    assert.throws(() => new Termios(12345));
  });
  it('flags getter/setter', () => {
    const t = new Termios(0);
    // these assumptions are abit flakey and might fail on some CIs
    assert.notEqual(t.c_iflag, 0);
    assert.notEqual(t.c_oflag, 0);
    assert.notEqual(t.c_lflag, 0);

    // toggle settings
    t.c_iflag &= ~(native.IFLAGS.BRKINT | native.IFLAGS.ICRNL);
    assert.equal(t.c_iflag & native.IFLAGS.BRKINT, 0);
    assert.equal(t.c_iflag & native.IFLAGS.ICRNL, 0);
    t.c_iflag |= native.IFLAGS.BRKINT | native.IFLAGS.ICRNL;
    assert.notEqual(t.c_iflag & native.IFLAGS.BRKINT, 0);
    assert.notEqual(t.c_iflag & native.IFLAGS.ICRNL, 0);

    t.c_oflag &= ~(native.OFLAGS.OPOST | native.OFLAGS.ONLCR);
    assert.equal(t.c_oflag & native.OFLAGS.OPOST, 0);
    assert.equal(t.c_oflag & native.OFLAGS.ONLCR, 0);
    t.c_oflag |= native.OFLAGS.OPOST | native.OFLAGS.ONLCR;
    assert.notEqual(t.c_oflag & native.OFLAGS.OPOST, 0);
    assert.notEqual(t.c_oflag & native.OFLAGS.ONLCR, 0);

    t.c_lflag &= ~(native.LFLAGS.ECHO | native.LFLAGS.ICANON);
    assert.equal(t.c_lflag & native.LFLAGS.ECHO, 0);
    assert.equal(t.c_lflag & native.LFLAGS.ICANON, 0);
    t.c_lflag |= native.LFLAGS.ECHO | native.LFLAGS.ICANON;
    assert.notEqual(t.c_lflag & native.LFLAGS.ECHO, 0);
    assert.notEqual(t.c_lflag & native.LFLAGS.ICANON, 0);
  });
  it('speed methods', () => {
    // note that nowadays both speeds are normally entangled,
    // thus setting one also applies to the other one
    const t = new Termios(0);
    const iSpeed = t.getInputSpeed();
    assert.notEqual(iSpeed, native.BAUD.B75);
    const oSpeed = t.getOutputSpeed();
    assert.notEqual(oSpeed, native.BAUD.B75);
    t.setInputSpeed(native.BAUD.B75);
    assert.equal(t.getInputSpeed(), native.BAUD.B75);
    t.setOutputSpeed(native.BAUD.B75);
    assert.equal(t.getOutputSpeed(), native.BAUD.B75);
  });
});

describe('terminal write/read test', () => {
  // cannot be tested on solaris (pty master does not support termios semantics)
  if (platform() === 'sunos') return;
  it('disable ECHO', (done) => {
    const p = pty.spawn('cat', [], {});
    const t = new Termios((p as any)._fd);
    const fromPty: string[] = [];
    p.onData(data => {
      fromPty.push(data);
      if (data.includes('#again_readable#')) {
        p.kill();
        assert.equal(fromPty.join('').includes('#readable##again_readable#'), true);
        done();
      }
    });
    p.write('#readable#');
    setTimeout(() => {
      // this must not show up on the terminal
      t.c_lflag &= ~native.LFLAGS.ECHO;
      t.writeTo((p as any)._fd, native.ACTION.TCSADRAIN);
      p.write('*TOP_SECRET*');
    }, 50);
    setTimeout(() => {
      t.c_lflag |= native.LFLAGS.ECHO;
      t.writeTo((p as any)._fd, native.ACTION.TCSADRAIN);
      p.write('#again_readable#');
    }, 100);
  });
});

describe('worker support', () => {
  it('multiple workers calling into native code', (done) => {
    const { Worker } = require('worker_threads');
    const AMOUNT = 5;
    const workers: Worker[] = [];
    const messages: string[] = [];
    for (let i = 0; i < AMOUNT; ++i) {
      const worker = new Worker(`
        const { Termios } = require('.');
        const { parentPort } = require('worker_threads');
        parentPort.once('message', msg => parentPort.postMessage(JSON.stringify(new Termios(0))));`
        , {eval: true});
      worker.once('message', (msg: string) => {
        messages.push(msg);
        if (messages.length === AMOUNT) {
          // all should be equal
          const first = messages[0];
          for (msg of messages) {
            assert.equal(msg, first);
          }
          done();
        }
      });
      workers.push(worker);
    }
    // trigger `new Termios(0)` on all workers
    workers.forEach(w => w.postMessage('ready'));
  });
});
