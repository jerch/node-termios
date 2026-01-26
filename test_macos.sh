#!/usr/bin/env bash

# Re‑exec under a fresh pseudo‑tty once
if [ -z "$FORCED_TTY" ]; then
  export FORCED_TTY=1
  # -q: quiet, -t 0: disable timing, -F: flush after each write (BSD script)
  exec script -q -t 0 /dev/null /bin/bash "$0" "$@"
fi

# --- real script starts here ---
echo "stdin is a TTY: $(tty)"
npm test
