#!/bin/sh

target=$1

bitbake -c cleansstate $target || exit 1
bitbake -c clean $target || exit 1
bitbake $target
