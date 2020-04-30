const { Termios, native } = require('.');

process.stdout.write(`Demo of raw mode grabbing arrow keys to move
the cursor around and place colored input.

CTRL-D to exit.`);

// termios settings placeholders
let initial = null;
let altered = null;

// initial cursor position
let cursor = null;

// ensure to restore everything on exit
function exitHandler() {
  // cursor restore
  if (cursor) {
    process.stdout.write(`\x1b[${cursor.col};${cursor.row}H\x1b[0m`);
  }
  // SGR restore
  process.stdout.write('\x1b[0m');
  // termios restore
  initial.writeTo(0);
  console.log();
}

// data read handler, triggers on every input in raw mode
function readHandler(data) {
  const s_data = data.toString();
  // expect a cursor position report at the beginning
  if (!cursor && s_data.endsWith('R')) {
    parsed = s_data.match(/[\x1b]\[(\d+);(\d+)R/);
    if (parsed) {
      cursor = {col: parsed[1], row: parsed[2]};
    }
    // some funky colors
    process.stdout.write('\x1b[31;43m');
    return;
  }
  // exit rule CTRL-D
  if (s_data === '\x04') {
    process.exit();
  }
  if (s_data === '\x03') {
    process.stdout.write('\x1b[37;41;1mCTRL-C disabled raw mode...\x1b[31;43m');
  }
  // cursor key and colored input handling
  switch (s_data) {
    case '\x1b\x5b\x41':  process.stdout.write('\x1b[A'); break;  // up    --> CUU
    case '\x1b\x5b\x42':  process.stdout.write('\x1b[B'); break;  // down  --> CUD
    case '\x1b\x5b\x43':  process.stdout.write('\x1b[C'); break;  // right --> CUF
    case '\x1b\x5b\x44':  process.stdout.write('\x1b[D'); break;  // left  --> CUB
    default:
      process.stdout.write(s_data);
  }
}

function main() {
  // do nothing, if stdin is not a TTY
  if (!native.isatty(0)) {
    return;
  }
  // grab termios data
  initial = new Termios(0);
  altered = new Termios(initial);
  // attach handler to restore on exit
  process.on('exit', exitHandler);
  // attach readHandler with program logic
  process.stdin.on('data', readHandler);
  // enter raw mode
  altered.setraw();
  altered.writeTo(0, native.ACTION.TCSANOW);
  // get cursor position report
  process.stdout.write('\x1b[6n');
}

if (require.main === module) {
  main();
}
