#!/bin/sh

if [ ! $# = 1 ]; then
    echo "No param, exit !"
fi

name=$1

data=$(cat $name | awk -F "-2018" '{ print $2}' | awk -F . '{ print $1 }')
echo $data
