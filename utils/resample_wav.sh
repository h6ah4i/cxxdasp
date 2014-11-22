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

function print_usage() {
    echo "Usage:"
    echo "    $0 [-f dest_freq] [-b backend] [-q quality] src_file dest_file"
    echo
    echo "Example:"
    echo "    $0 -f 44100 -b simple-resampler -q1 input.wav output.wav"
}

# parse arguments

DEST_FREQ=44100
BACKEND="simple-resampler"
QUALITY=1

while getopts ":f:b:q:" o; do
    case "$o" in
        f)
            DEST_FREQ="$OPTARG"
            ;;
        b)
            BACKEND="$OPTARG"
            ;;
        q)
            QUALITY="$OPTARG"
            ;;
        [?])
            print_usage
            exit 1
            ;;
    esac
done
shift $(($OPTIND-1))

# check arguments
if [ "$#" -lt 2 ]; then
    print_usage
    exit 1
fi

SRC_FILE=$1
DEST_FILE=$2

# resolve resampler app path
RESAMPLER=$($UTILS_DIR/find_resampler.sh $BACKEND)
if [ $RESAMPLER == "" ]; then
    echo "resampler app is not found"
    exit 1
fi

#
# main process
#
SRC_FREQ=$(sox --i -r $SRC_FILE)
NUM_CHANNELS=$(sox --i -c $SRC_FILE)

TMP_SRC_FILE=$SCRIPT_DIR/tmp_src.raw
TMP_DEST_FILE=$SCRIPT_DIR/tmp_dest.raw

# src.wav -> src.raw
sox $SRC_FILE $TMP_SRC_FILE

# src.raw -> resampled.raw
$RESAMPLER $TMP_SRC_FILE $TMP_DEST_FILE $NUM_CHANNELS $SRC_FREQ $DEST_FREQ $QUALITY

# resampled.raw -> resampled.wav
sox -t raw -c $NUM_CHANNELS -r $DEST_FREQ -b 32 -e floating-point $TMP_DEST_FILE $DEST_FILE

rm -f $TMP_SRC_FILE $TMP_DEST_FILE