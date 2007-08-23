#!/bin/sh
OUT_PREF=single.out
if [ -z $1 ]
then
  echo "Usage $0 <testcase>"
  exit 1
fi
if [ ! -e "$1" ]
then
  echo "Testcase $1 not found"
  exit 1
fi
# strip suffix '.single'
TC_NAME=`basename ${1%.single}`
PREF=`dirname ${1}`
CONFDIR=`pwd`/etc ${PREF}/${TC_NAME}.single > ${OUT_PREF}/$TC_NAME.out 2> ${OUT_PREF}/$TC_NAME.err
