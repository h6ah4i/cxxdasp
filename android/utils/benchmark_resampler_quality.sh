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

BASE_DIR=$SCRIPT_DIR/../..
UTILS_DIR=$BASE_DIR/utils

BENCH_TOP_DIR="$BASE_DIR/android/benchmark_resampler"
ORIG_WAV_DIR="$BENCH_TOP_DIR/original_sounds"
ORIG_SPECTROGRAM_DIR=$ORIG_WAV_DIR"_spectrogram"

QUALITY_CHECK=$SCRIPT_DIR/resampling_quality_check.sh
CONCAT_SPECTROGRAMS=$UTILS_DIR/concat_spectrograms.sh

# generate sound files
$UTILS_DIR/gen_testsounds.sh $ORIG_WAV_DIR

# generate spectrogram images
$UTILS_DIR/gen_spectrograms.sh $ORIG_WAV_DIR $ORIG_SPECTROGRAM_DIR

#
# simple-resampler (using audio-resampler library)
#
$QUALITY_CHECK simple 1 2>&1 | tee "$BENCH_TOP_DIR/console_output_simple_q1.txt"
$QUALITY_CHECK simple 2 2>&1 | tee "$BENCH_TOP_DIR/console_output_simple_q2.txt"  # synonym to Q4
$QUALITY_CHECK simple 3 2>&1 | tee "$BENCH_TOP_DIR/console_output_simple_q3.txt"  # synonym to Q4
$QUALITY_CHECK simple 4 2>&1 | tee "$BENCH_TOP_DIR/console_output_simple_q4.txt"
$QUALITY_CHECK simple 5 2>&1 | tee "$BENCH_TOP_DIR/console_output_simple_q5.txt"

#
# soxr-resampler (using soxr library)
#
#$QUALITY_CHECK soxr 1 2>&1 | tee "$BENCH_TOP_DIR/console_output_soxr_q1.txt"
#$QUALITY_CHECK soxr 2 2>&1 | tee "$BENCH_TOP_DIR/console_output_soxr_q2.txt"
#$QUALITY_CHECK soxr 3 2>&1 | tee "$BENCH_TOP_DIR/console_output_soxr_q3.txt"
#$QUALITY_CHECK soxr 4 2>&1 | tee "$BENCH_TOP_DIR/console_output_soxr_q4.txt"
#$QUALITY_CHECK soxr 5 2>&1 | tee "$BENCH_TOP_DIR/console_output_soxr_q5.txt"

#
# concat spectrogram images
#
# LEFT: simple-resampler | RIGHT: soxr-resampler
#
#function gen_concat_spectrums() {
#    QUALITY=$1
#
#    echo "Generating concatenated spectrogram images (quality = $QUALITY)"
#
#    $CONCAT_SPECTROGRAMS \
#        $BENCH_TOP_DIR"/resampled_simple_q"$QUALITY"_spectrogram" \
#        $BENCH_TOP_DIR"/resampled_soxr_q"$QUALITY"_spectrogram" \
#        $BENCH_TOP_DIR"/resampled_concat_q"$QUALITY"_spectrogram"
#}

#gen_concat_spectrums 1
#gen_concat_spectrums 2
#gen_concat_spectrums 3
#gen_concat_spectrums 4
#gen_concat_spectrums 5