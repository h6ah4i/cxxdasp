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

function calc_stage1_fft_filter_coeffs() {
    local DEST_DIR=$BASE_DIR/generated_source/resampler/smart/coeffs
    local SCE_FILE=$UTILS_DIR/design_smart_resampler_filter.sce

    local INPUT_FREQ=$1
    local OUTPUT_FREQ=$2
    local DEST_FILE=$3

    if [ ! -d $DEST_DIR ]; then
        mkdir -p $DEST_DIR
    fi

    local p=""
    p+="PLOT=0;"
    p+="INPUT_FREQ=$INPUT_FREQ;"
    p+="OUTPUT_FREQ=$OUTPUT_FREQ;"
    p+="DEST_FILE_STAGE_1='$DEST_DIR/$DEST_FILE';"

    echo "Generating Stage 1 (FFT) FIR coefficients: INPUT=$INPUT_FREQ, OUTPUT=$OUTPUT_FREQ, FILE=$DEST_FILE"
    scilab-cli -e \
        "$p;predef('all');exec(\"$SCE_FILE\");exit;" \
        > /dev/null
}

function calc_stage2_filter_coeffs() {
    local DEST_DIR=$BASE_DIR/generated_source/resampler/smart/coeffs
    local SCE_FILE=$UTILS_DIR/design_smart_resampler_filter.sce

    local INPUT_FREQ=$1
    local OUTPUT_FREQ=$2
    local DEST_FILE=$3
    local HIGH_QUALITY=$4   # only effects stage2

    if [ ! -d $DEST_DIR ]; then
        mkdir -p $DEST_DIR
    fi

    local p=""
    p+="PLOT=0;"
    p+="INPUT_FREQ=$INPUT_FREQ;"
    p+="OUTPUT_FREQ=$OUTPUT_FREQ;"
    p+="HIGH_QUALITY=$HIGH_QUALITY;"
    p+="DEST_FILE_STAGE_2='$DEST_DIR/$DEST_FILE';"

    echo "Generating Stage 2 FIR coefficients: INPUT=$INPUT_FREQ, OUTPUT=$OUTPUT_FREQ, HIGH_QUALITY=$HIGH_QUALITY, FILE=$DEST_FILE"
    scilab-cli -e \
        "$p;predef('all');exec(\"$SCE_FILE\");exit;" \
        > /dev/null
}

function calc_stage1_halfband_filter_coeffs() {
    local DEST_DIR=$BASE_DIR/generated_source/resampler/smart/coeffs
    local SCE_FILE=$UTILS_DIR/design_half_band_filter.sce

    local FORDER=$1
    local ALPHA=$2
    local DEST_FILE=$3

    if [ ! -d $DEST_DIR ]; then
        mkdir -p $DEST_DIR
    fi

    local p=""
    p+="PLOT=0;"
    p+="FORDER=$FORDER;"
    p+="ALPHA=$ALPHA;"
    p+="MOD_DEST_FILE='$DEST_DIR/$DEST_FILE';"

    echo "Generating Stage 1 (Halfband) FIR coefficients: FORDER=$FORDER, ALPHA=$ALPHA, FILE=$DEST_FILE"
    scilab-cli -e \
        "$p;predef('all');exec(\"$SCE_FILE\");exit;" \
        > /dev/null
}

#
# STAGE 1 (FFT)
#

# (INPUT_FREQ < OUTPUT_FREQ)
#
# 8000, 11025, 12000, 16000, 22050, 24000, 32000        -> 44100
# 8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100 -> 48000
calc_stage1_fft_filter_coeffs 1 1 "stage1_fft_nyq.inc"

# ((INPUT_FREQ/2) == OUTPUT_FREQ)
#
# 88200 -> 44100
# 96000 -> 48000
calc_stage1_fft_filter_coeffs 2 1 "stage1_fft_half_nyq.inc"

# ((INPUT_FREQ/4) == OUTPUT_FREQ)
#
# 176400 -> 44100
# 192000 -> 48000
calc_stage1_fft_filter_coeffs 4 1 "stage1_fft_quarter_nyq.inc"

# (INPUT_FREQ > OUTPUT_FREQ)
#

# 48000 -> 44100
calc_stage1_fft_filter_coeffs 48000 44100 "stage1_fft_48000_44100.inc"

# 88200 -> 48000
calc_stage1_fft_filter_coeffs 88200 48000 "stage1_fft_88200_48000.inc"

# 96000 -> 44100
calc_stage1_fft_filter_coeffs 96000 44100 "stage1_fft_96000_44100.inc"

# 176400 -> 48000
calc_stage1_fft_filter_coeffs 176400 48000 "stage1_fft_176400_48000.inc"

# 192000 -> 44100
calc_stage1_fft_filter_coeffs 192000 44100 "stage1_fft_192000_44100.inc"

#
# STAGE 1 (Halbband)
#
calc_stage1_halfband_filter_coeffs 33 8.0 "stage1_halfband_lq.inc"
calc_stage1_halfband_filter_coeffs 129 14.0 "stage1_halfband_mq.inc"


#
# STAGE 2
# 

# 8000  x2  {M = 441, L = 160}
# 12000 x2  {M = 441, L = 240}
# 16000 x2  {M = 441, L = 320}
# 24000 x2  {M = 441, L = 480}
# 32000 x2  {M = 441, L = 640}
# 48000 x2  {M = 441, L = 960}
# 96000 x2  {M = 441, L = 1920}
calc_stage2_filter_coeffs 32000 44100 "stage2_lq_m441.inc" 0
calc_stage2_filter_coeffs 32000 44100 "stage2_hq_m441.inc" 1

# 11025 x2 -> 44100 {M = 2, L = 1}
# 12000 x2 -> 48000 {M = 2, L = 1}
calc_stage2_filter_coeffs 12000 48000 "stage2_lq_m2.inc" 0
calc_stage2_filter_coeffs 12000 48000 "stage2_hq_m2.inc" 1

# 8000  x2  {M = 3, L = 1}
# 16000 x2  {M = 3, L = 2}
# 32000 x2  {M = 3, L = 4}
calc_stage2_filter_coeffs 8000 48000 "stage2_lq_m3.inc" 0
calc_stage2_filter_coeffs 8000 48000 "stage2_hq_m3.inc" 1

# 44100 x2  {M = 80, L = 147}
# 88200 x2  {M = 80, L = 294}
calc_stage2_filter_coeffs 44100 48000 "stage2_lq_m80.inc" 0
calc_stage2_filter_coeffs 44100 48000 "stage2_hq_m80.inc" 1

# 11025 x2  {M = 320, L = 147}
# 22050 x2  {M = 320, L = 294}
calc_stage2_filter_coeffs 11025 48000 "stage2_lq_m320.inc" 0
calc_stage2_filter_coeffs 11025 48000 "stage2_hq_m320.inc" 1
