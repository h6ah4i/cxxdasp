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

BENCH_TOP_DIR="$BASE_DIR/benchmark_resampler"

SPEED_CHECK=$UTILS_DIR/resampling_speed_check.sh
CONCAT_SPECTROGRAMS=$UTILS_DIR/concat_spectrograms.sh

#
# parameters
#
SOUND_DURATION=10         # sec.
NUM_ITERATE=30      # 30 times tries / measureing

QUALITY_LIST=(1 2 3 4 5)
SRC_FREQ_LIST=(8000 11025 12000 16000 22050 24000 32000 44100 48000 88200 96000 176400 192000)
DEST_FREQ_LIST=(44100 48000)
NUM_CH_LIST=(1 2)


function div_float() {
    if hash bc 2>/dev/null; then
        echo "$1 / $2" | bc -l
    else
        awk -v A=$1 -v B=$2 'BEGIN { print A/B }'
    fi
}

function measure_compare() {
    local target1=$1
    local target2=$2
    local src_freq=$3
    local dest_freq=$4
    local num_ch=$5
    local quality=$6
    local t_1=$($SPEED_CHECK $target1 $src_freq $dest_freq $num_ch $quality $SOUND_DURATION $NUM_ITERATE)
    local t_2=$($SPEED_CHECK $target2 $src_freq $dest_freq $num_ch $quality $SOUND_DURATION $NUM_ITERATE)
    local speed_ratio=$(div_float $t_2 $t_1)

    speed_ratio=$(printf "%.1f" $speed_ratio)
    speed_ratio_fresample=$(printf "%.1f" $speed_ratio_fresample)

    echo "SRC=$src_freq, DEST=$dest_freq, CH=$num_ch, Q=$quality"
    echo "    $target1: $t_1 [us]"
    echo "    $target2: $t_2 [us]"
    echo "    speed ratio: $speed_ratio"
    echo
}

function measure_all_combinations() {
    local target1=$1
    local target2=$2

    for dest_freq in ${DEST_FREQ_LIST[@]}; do
        for quality in ${QUALITY_LIST[@]}; do
            for src_freq in ${SRC_FREQ_LIST[@]}; do
                for num_ch in ${NUM_CH_LIST[@]}; do
                    measure_compare $target1 $target2 $src_freq $dest_freq $num_ch $quality
                done
            done
        done
    done
}


measure_all_combinations simple soxr 2>&1 | tee "$BENCH_TOP_DIR/console_output_speed_soxr.txt"
measure_all_combinations simple fresample 2>&1 | tee "$BENCH_TOP_DIR/console_output_speed_fresample.txt"
measure_all_combinations simple src 2>&1 | tee "$BENCH_TOP_DIR/console_output_speed_src.txt"
