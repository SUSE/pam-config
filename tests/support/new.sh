#!/bin/sh
function Usage {
  echo $1
  echo -e "Usage $0 -<c|s> <name> [<module> [<service>]]"
  exit 1
}

function copy_and_filter {
  local SKELETON=$1
  local NAME=$2
  local MOD=$3
  local SRV=$4
  local TRGT_PREF=$5
  if [ -z ${TRGT_PREF} ]
  then
    TRGT_PREF=.
  fi
  local TARGET=${TRGT_PREF}/test-$NAME.single
  if [ -e $TARGET ]
  then
   return 1
  fi 
  sed -e "s;@@NAME@@;$NAME;g" \
      -e "s;@@MOD@@;$MOD;g" \
      -e "s;@@SRV@@;$SRV;g" \
      $SKELETON > $TARGET
  chmod +x $TARGET
}

SPRT_PREF=support
TRGT_PREF=testcases

if [ "x-s" == "x$1" ]
then
  SKELETON=${SPRT_PREF}/skeleton-service
else
  if [ "x-c" == "x$1" ]
  then
    SKELETON=${SPRT_PREF}/skeleton-common
  else
    Usage "You must specify module type! -<c|s>"
  fi
fi

if [ -z $2 ]
then
  Usage "No testcase name given."
fi
NAME=$2

MOD=foo
if [ ! -z $3 ]
then
  MOD=$3
fi

SRV=bar
if [ ! -z $4 ]
then
  SRV=$4
fi

copy_and_filter $SKELETON $NAME $MOD $SRV $TRGT_PREF
if [ 0 != $? ]
then
  Usage "Testcase already exists!"
fi
