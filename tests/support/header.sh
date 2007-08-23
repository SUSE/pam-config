#!/bin/sh
function init_pamdir {
  # START cleanup
  rm -f etc/pam.d/*
  cp etc/* etc/pam.d/ 2>/dev/null
  # END cleanup
}
function check_for_confdir {
  if [ "x$CONFDIR" == "x" ]; then
    echo "CONFDIR not set"
    exit 1
  fi
}
init_pamdir
check_for_confdir
PAMCONFIG="../src/pam-config --confdir $CONFDIR"
