#!/bin/bash

#
# Usage:  boiler.sh trgtName config.h
#


TRGT_NAME=$1
TRGT_NAME_UPPER=$(echo $1 |  sed -e "y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/")
CONF_HEADER=$2
DIR=$(dirname $0)


cat $DIR/plain/boiler.h \
	| sed -e "s/BOILER/${TRGT_NAME_UPPER}/g" \
	      -e "s/boiler/${TRGT_NAME}/g" \
	      -e "s/CONF_HEADER/${CONF_HEADER}/" \
	| uncrustify -c ~/.uncrustify.cfg -l C -q \
	> ${TRGT_NAME}.h

cat $DIR/plain/boiler_tests.h \
	| sed -e "s/BOILER/${TRGT_NAME_UPPER}/g" \
	      -e "s/CONF_HEADER/${CONF_HEADER}/" \
	      -e "s/boiler/${TRGT_NAME}/g" \
	| uncrustify -c ~/.uncrustify.cfg -l C -q \
	> ${TRGT_NAME}_tests.h

cat $DIR/plain/boiler.c \
	| sed -e "s/boiler/${TRGT_NAME}/g" \
	      -e "s/CONF_HEADER/${CONF_HEADER}/" \
	| uncrustify -c ~/.uncrustify.cfg -l C -q \
	> ${TRGT_NAME}.c

cat $DIR/plain/boiler_tests.c \
	| sed -e "s/boiler/${TRGT_NAME}/g" \
	      -e "s/CONF_HEADER/${CONF_HEADER}/" \
	| uncrustify -c ~/.uncrustify.cfg -l C -q \
	> ${TRGT_NAME}_tests.c

