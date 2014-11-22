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

BASE_DIR=$SCRIPT_DIR/../..
UTILS_DIR=$BASE_DIR/utils

DEST_DIR=$BASE_DIR/android/benchmark_resampler/original_sounds

if [ ! -d $DEST_DIR ]; then
    mkdir -p $DEST_DIR
    $UTILS_DIR/gen_testsounds.sh $DEST_DIR

    pushd $DEST_DIR

    # convert to .raw
    for wav_file in $(find . -type f -name '*.wav')
    do
        sox ${wav_file} ${wav_file%.wav}.raw
    done

    popd
fi