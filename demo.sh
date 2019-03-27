#!/bin/bash

BINDIR=`pwd`/build/demo
SRCDIR=`pwd`/demo
KEYOPT=-g

while getopts s OPT; do
    case $OPT in
	"s" ) KEYOPT="" ;;
    esac
done

PREFFILE=${SRCDIR}/client/preference10.txt
INDATAFILE=${SRCDIR}/pprs/inputdata10.txt

xterm -T "client" -e "/bin/bash -c 'cd ${BINDIR}/client && ./client -i ${PREFFILE}; exec /bin/bash -i'"&
xterm -T "PPRS"   -e "/bin/bash -c 'cd ${BINDIR}/pprs   && ./pprs -i ${INDATAFILE}; exec /bin/bash -i'"&
xterm -T "PSP"    -e "/bin/bash -c 'cd ${BINDIR}/psp    && ./psp ${KEYOPT};         exec /bin/bash -i'"&
