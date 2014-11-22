#!/bin/bash
#
#    Copyright (C) 2014 Haruki Hasegawa
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#
set -e

# Obtain paths
pushd $(dirname "${0}") > /dev/null
SCRIPT_DIR=$(pwd -L)
popd > /dev/null

BASE_DIR=$SCRIPT_DIR/..
UTILS_DIR=$BASE_DIR/utils

# check arguments
if [ "$#" -lt 7 ]; then
    echo "Usage: $0 {simple|soxr} src_freq dest_freq ch quality duration num_iterate"
    exit 1
fi


BACKEND=$1"-resampler"
SRC_FREQ=$2
DEST_FREQ=$3
NUM_CH=$4
QUALITY=$5
DURATION=$6
NUM_ITERATE=$7

TMP_SRC_FILE=$SCRIPT_DIR/tmp_speed_check_src.raw
TMP_DEST_FILE=$SCRIPT_DIR/tmp_speed_check_dest.raw

# resolve resampler app path
RESAMPLER=$($UTILS_DIR/find_resampler.sh $BACKEND)
if [ $RESAMPLER == "" ]; then
    echo "resampler app is not found"
    exit 1
fi

# generate source file
sox -r $SRC_FREQ -c $NUM_CH -b 32 -e floating-point -n $TMP_SRC_FILE synth $DURATION sine 1000 gain -6

ELAPSED_TIME_LIST=""
cnt=0
while [ $cnt -lt $NUM_ITERATE ]; do
    RESAMPLER_STDOUT=$($RESAMPLER $TMP_SRC_FILE $TMP_DEST_FILE $NUM_CH $SRC_FREQ $DEST_FREQ $QUALITY)
    ELAPSED_US=$(echo $RESAMPLER_STDOUT | sed 's/^.*\ \([0-9]*\) \[us\]$/\1/g')

    if [ $cnt -eq 0 ]; then
        ELAPSED_TIME_LIST=$ELAPSED_US
    else
        ELAPSED_TIME_LIST=$ELAPSED_TIME_LIST$'\n'$ELAPSED_US
    fi
    cnt=$((cnt+1))
done


# sort
if [ $(which sort | grep system32 -c) == "1" ]; then
    # Windows
    # GNU sort is available...
    ELAPSED_TIME_LIST="$ELAPSED_TIME_LIST"
else
    # ignore half of the slower results
    ELAPSED_TIME_LIST=$(echo "$ELAPSED_TIME_LIST" | sort -n)
    ELAPSED_TIME_LIST=$(echo "$ELAPSED_TIME_LIST" | head -n $((NUM_ITERATE / 2)))
fi

# calculate average
AVERAGE=$(echo "$ELAPSED_TIME_LIST" | awk '{ total += $1; count++ } END { printf "%.0f", (total/count) }')

# print result  (unit: us)
echo "$AVERAGE"

# clean up
rm -f $TMP_SRC_FILE
rm -f $TMP_DEST_FILE