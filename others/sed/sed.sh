#!/bin/bash

#cd rename
#FILES=`ls`
##echo $FILES
#
#for I in ${FILES}; do
#	FILE=${I}
#	TEST_NAME=$(echo ${FILE} | awk -F 'ts_' '{ print $2 }')
#	echo ${TEST_NAME}
#	cp ${FILE} plugin_${TEST_NAME}_phase2
#done
#`cd ..`

#FILES=`ls ./rename/`
#
#for I in ${FILES}; do
#	FILE=${I}
#	TEST_NAME=$(echo ${FILE} | awk -F 'ts_' '{ print $2 }')
#	echo ${TEST_NAME} >> custom.prf
#done

#FILES=`ls ~/workspace/lynis-ts/plugins/plugin_*`
#
#for I in ${FILES}; do
#	FILE=${I}
#	sed -i '2 r head.txt' ${FILE}
#done

#FILES=`ls ./rename/`
#
#for I in ${FILES}; do
#	FILE=${I}
#	echo $FILE
#	TEST_NAME=$(echo ${FILE} | awk -F '_' '{ print $2 }')
#	OLD="# PLUGIN_NAME=binaries"
#	NEW="# PLUGIN_NAME=${TEST_NAME}"
#	echo $NEW
#	sed -i  "s/$OLD/$NEW/" ~/workspace/lynis-ts/plugins/${FILE}
#done

FILES=`ls ./rename/`

for I in ${FILES}; do
	cat tail.txt >> ~/workspace/lynis-ts/plugins/${I}
done
