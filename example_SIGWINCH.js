const { native: { tcgetwinsize } } = require('.');

process.stdin.resume();

process.on('SIGWINCH', () => {
  console.log('TTY size updated:', tcgetwinsize(0));
});

console.log('TTY size initial:', tcgetwinsize(0));
console.log('Resize the terminal window...');
