#!/usr/bin/env bash

if [ -z "$FORCED_TTY" ] && [ -t 0 ]; then
  # re‑exec ourselves under script(1), using a pseudo‑tty
  FORCED_TTY=1 script -q /dev/null "$0" "$@"
  exit $?
fi

# --- real script starts here ---
echo "stdin is a TTY: $(tty)"
