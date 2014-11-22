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

# Obtain paths
pushd $(dirname "${0}") > /dev/null
SCRIPT_DIR=$(pwd -L)
popd > /dev/null

BASE_DIR=$SCRIPT_DIR/..
UTILS_DIR=$BASE_DIR/utils


# check arguments
if [ "$#" -lt 2 ]; then
    echo "Usage: $0 src_wav_dir dest_image_dir"
    exit 1
fi

SRC_WAV_DIR=$1
DEST_DIR=$2
SPECTROGRAM_OPTIONS="-x 800 -y 513 -z 180 -w hann"

if [ ! -d $DEST_DIR ]; then
    mkdir -p $DEST_DIR
fi

# original files
for wav_file in $(ls $SRC_WAV_DIR/*.wav); do
    png_file=$(echo $(basename $wav_file) | sed 's/\.wav$/\.png/g')
    echo "Generating $png_file"
    sox $wav_file -n spectrogram $SPECTROGRAM_OPTIONS -o $DEST_DIR/$png_file
done