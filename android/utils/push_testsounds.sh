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
ORIG_SOUND_DIR=$BASE_DIR/android/benchmark_resampler/original_sounds

REMOTE_BASE_PATH=/data/local/tmp

adb shell "mkdir -p $REMOTE_BASE_PATH/benchmark_resampler/original_sounds"

# convert to .raw
for raw_file in $(find $ORIG_SOUND_DIR -type f -name '*.raw')
do
    dest_path=$REMOTE_BASE_PATH/benchmark_resampler/original_sounds/$(basename $raw_file)
    echo $dest_path
    adb push $raw_file $dest_path
done
