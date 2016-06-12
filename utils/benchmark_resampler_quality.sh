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
ORIG_WAV_DIR="$BENCH_TOP_DIR/original_sounds"
ORIG_SPECTROGRAM_DIR=$ORIG_WAV_DIR"_spectrogram"

QUALITY_CHECK=$UTILS_DIR/resampling_quality_check.sh
CONCAT_SPECTROGRAMS=$UTILS_DIR/concat_spectrograms.sh

# generate sound files
$UTILS_DIR/gen_testsounds.sh $ORIG_WAV_DIR

# generate spectrogram images
$UTILS_DIR/gen_spectrograms.sh $ORIG_WAV_DIR $ORIG_SPECTROGRAM_DIR

#
# simple-resampler (using audio-resampler library)
#
$QUALITY_CHECK simple 1 2>&1 | tee "$BENCH_TOP_DIR/console_output_simple_q1.txt"
$QUALITY_CHECK simple 2 2>&1 | tee "$BENCH_TOP_DIR/console_output_simple_q2.txt"
$QUALITY_CHECK simple 3 2>&1 | tee "$BENCH_TOP_DIR/console_output_simple_q3.txt"
$QUALITY_CHECK simple 4 2>&1 | tee "$BENCH_TOP_DIR/console_output_simple_q4.txt"
$QUALITY_CHECK simple 5 2>&1 | tee "$BENCH_TOP_DIR/console_output_simple_q5.txt"

#
# soxr-resampler (using soxr library)
#
$QUALITY_CHECK soxr 1 2>&1 | tee "$BENCH_TOP_DIR/console_output_soxr_q1.txt"
$QUALITY_CHECK soxr 2 2>&1 | tee "$BENCH_TOP_DIR/console_output_soxr_q2.txt"
$QUALITY_CHECK soxr 3 2>&1 | tee "$BENCH_TOP_DIR/console_output_soxr_q3.txt"
$QUALITY_CHECK soxr 4 2>&1 | tee "$BENCH_TOP_DIR/console_output_soxr_q4.txt"
$QUALITY_CHECK soxr 5 2>&1 | tee "$BENCH_TOP_DIR/console_output_soxr_q5.txt"

#
# fresample-resampler (using fresample library)
#
$QUALITY_CHECK fresample 1 2>&1 | tee "$BENCH_TOP_DIR/console_output_fresample_q1.txt"
$QUALITY_CHECK fresample 2 2>&1 | tee "$BENCH_TOP_DIR/console_output_fresample_q2.txt"
$QUALITY_CHECK fresample 3 2>&1 | tee "$BENCH_TOP_DIR/console_output_fresample_q3.txt"
$QUALITY_CHECK fresample 4 2>&1 | tee "$BENCH_TOP_DIR/console_output_fresample_q4.txt"
$QUALITY_CHECK fresample 5 2>&1 | tee "$BENCH_TOP_DIR/console_output_fresample_q5.txt"

#
# src-resampler (using libsamplerate library)
#
$QUALITY_CHECK src 1 2>&1 | tee "$BENCH_TOP_DIR/console_output_src_q1.txt"
$QUALITY_CHECK src 2 2>&1 | tee "$BENCH_TOP_DIR/console_output_src_q2.txt"
$QUALITY_CHECK src 3 2>&1 | tee "$BENCH_TOP_DIR/console_output_src_q3.txt"
$QUALITY_CHECK src 4 2>&1 | tee "$BENCH_TOP_DIR/console_output_src_q4.txt"
$QUALITY_CHECK src 5 2>&1 | tee "$BENCH_TOP_DIR/console_output_src_q5.txt"


#
# concat spectrogram images
#
# LEFT: simple-resampler | RIGHT: {soxr-resampler | fresample-resampler | src-resampler}
#
function gen_concat_spectrums() {
    local T1=$1
    local T2=$2
    local QUALITY=$3

    echo "Generating concatenated spectrogram images ($T1 & $T2, quality = $QUALITY)"

    $CONCAT_SPECTROGRAMS \
        $BENCH_TOP_DIR"/resampled_"$T1"_q"$QUALITY"_spectrogram" \
        $BENCH_TOP_DIR"/resampled_"$T2"_q"$QUALITY"_spectrogram" \
        $BENCH_TOP_DIR"/resampled_concat_"$T1"_"$T2"_q"$QUALITY"_spectrogram"
}

gen_concat_spectrums simple soxr 1
gen_concat_spectrums simple soxr 2
gen_concat_spectrums simple soxr 3
gen_concat_spectrums simple soxr 4
gen_concat_spectrums simple soxr 5

gen_concat_spectrums simple fresample 1
gen_concat_spectrums simple fresample 2
gen_concat_spectrums simple fresample 3
gen_concat_spectrums simple fresample 4
gen_concat_spectrums simple fresample 5

gen_concat_spectrums simple src 1
gen_concat_spectrums simple src 2
gen_concat_spectrums simple src 3
gen_concat_spectrums simple src 4
gen_concat_spectrums simple src 5
