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
if [ "$#" -lt 1 ]; then
    echo "Usage: $0 dest_wav_dir"
    exit 1
fi

DEST_DIR=$1

function gen_float_sweep_sound_files() {
    local freq=$1
    local general_opts="-V -r $freq -b 32 -e floating-point -n"
    local synth_opts_l="8 sine 1:$(($freq/2-1)) gain -6"
    local synth_opts_r="8 sine $(($freq/2-1)):1 gain -6"

    # monaural
    if [ ! -f $DEST_DIR/sweep_mono_$freq.wav ]; then
        sox -c 1 $general_opts $DEST_DIR/sweep_mono_$freq.wav synth $synth_opts_l
    fi
    # stereo
    if [ ! -f $DEST_DIR/sweep_stereo_$freq.wav ]; then
        sox -c 1 $general_opts $DEST_DIR/tmp_left_sweep_stereo_$freq.wav synth $synth_opts_l
        sox -c 1 $general_opts $DEST_DIR/tmp_right_sweep_stereo_$freq.wav synth $synth_opts_r
        sox $DEST_DIR/tmp_left_sweep_stereo_$freq.wav $DEST_DIR/tmp_right_sweep_stereo_$freq.wav -c 2 --combine merge $DEST_DIR/sweep_stereo_$freq.wav
        rm $DEST_DIR/tmp_left_sweep_stereo_$freq.wav
        rm $DEST_DIR/tmp_right_sweep_stereo_$freq.wav
    fi
}

if [ ! -d $DEST_DIR ]; then
    mkdir -p $DEST_DIR
fi

gen_float_sweep_sound_files 8000
gen_float_sweep_sound_files 16000
gen_float_sweep_sound_files 32000
gen_float_sweep_sound_files 44100
gen_float_sweep_sound_files 48000
gen_float_sweep_sound_files 88200
gen_float_sweep_sound_files 96000
