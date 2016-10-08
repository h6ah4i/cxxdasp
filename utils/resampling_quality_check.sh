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
    echo "Usage: $0 {simple|soxr} quality"
    exit 1
fi

BENCH_TOP_DIR="$BASE_DIR/benchmark_resampler"

RESAMPLER=$1
QUALITY=$2
ORIG_WAV_DIR="$BENCH_TOP_DIR/original_sounds"
RESAMPLED_WAV_DIR="$BENCH_TOP_DIR/resampled_"$RESAMPLER"_q"$QUALITY
RESAMPLED_SPECTROGRAM_DIR=$RESAMPLED_WAV_DIR"_spectrogram"

if [ ! -d $RESAMPLED_WAV_DIR ]; then
    mkdir -p $RESAMPLED_WAV_DIR
fi

RESAMPLE_WAV="$UTILS_DIR/resample_wav.sh -b $RESAMPLER-resampler -q $QUALITY"

echo "===[ $RESAMPLER-resampler (quality=$QUALITY) ]==="

# to 44100 Hz
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_mono_8000.wav $RESAMPLED_WAV_DIR/resampled_mono_8000_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_mono_11025.wav $RESAMPLED_WAV_DIR/resampled_mono_11025_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_mono_12000.wav $RESAMPLED_WAV_DIR/resampled_mono_12000_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_mono_16000.wav $RESAMPLED_WAV_DIR/resampled_mono_16000_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_mono_22050.wav $RESAMPLED_WAV_DIR/resampled_mono_22050_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_mono_24000.wav $RESAMPLED_WAV_DIR/resampled_mono_24000_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_mono_32000.wav $RESAMPLED_WAV_DIR/resampled_mono_32000_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_mono_44100.wav $RESAMPLED_WAV_DIR/resampled_mono_44100_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_mono_48000.wav $RESAMPLED_WAV_DIR/resampled_mono_48000_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_mono_88200.wav $RESAMPLED_WAV_DIR/resampled_mono_88200_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_mono_96000.wav $RESAMPLED_WAV_DIR/resampled_mono_96000_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_mono_176400.wav $RESAMPLED_WAV_DIR/resampled_mono_176400_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_mono_192000.wav $RESAMPLED_WAV_DIR/resampled_mono_192000_44100.wav

$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_stereo_8000.wav $RESAMPLED_WAV_DIR/resampled_stereo_8000_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_stereo_11025.wav $RESAMPLED_WAV_DIR/resampled_stereo_11025_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_stereo_12000.wav $RESAMPLED_WAV_DIR/resampled_stereo_12000_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_stereo_16000.wav $RESAMPLED_WAV_DIR/resampled_stereo_16000_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_stereo_22050.wav $RESAMPLED_WAV_DIR/resampled_stereo_22050_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_stereo_24000.wav $RESAMPLED_WAV_DIR/resampled_stereo_24000_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_stereo_32000.wav $RESAMPLED_WAV_DIR/resampled_stereo_32000_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_stereo_44100.wav $RESAMPLED_WAV_DIR/resampled_stereo_44100_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_stereo_48000.wav $RESAMPLED_WAV_DIR/resampled_stereo_48000_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_stereo_88200.wav $RESAMPLED_WAV_DIR/resampled_stereo_88200_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_stereo_96000.wav $RESAMPLED_WAV_DIR/resampled_stereo_96000_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_stereo_176400.wav $RESAMPLED_WAV_DIR/resampled_stereo_176400_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_stereo_96000.wav $RESAMPLED_WAV_DIR/resampled_stereo_96000_44100.wav
$RESAMPLE_WAV -f 44100 $ORIG_WAV_DIR/sweep_stereo_192000.wav $RESAMPLED_WAV_DIR/resampled_stereo_192000_44100.wav

# to 48000 Hz
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_mono_8000.wav $RESAMPLED_WAV_DIR/resampled_mono_8000_48000.wav
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_mono_11025.wav $RESAMPLED_WAV_DIR/resampled_mono_11025_48000.wav
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_mono_12000.wav $RESAMPLED_WAV_DIR/resampled_mono_12000_48000.wav
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_mono_16000.wav $RESAMPLED_WAV_DIR/resampled_mono_16000_48000.wav
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_mono_22050.wav $RESAMPLED_WAV_DIR/resampled_mono_22050_48000.wav
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_mono_24000.wav $RESAMPLED_WAV_DIR/resampled_mono_24000_48000.wav
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_mono_32000.wav $RESAMPLED_WAV_DIR/resampled_mono_32000_48000.wav
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_mono_44100.wav $RESAMPLED_WAV_DIR/resampled_mono_44100_48000.wav
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_mono_48000.wav $RESAMPLED_WAV_DIR/resampled_mono_48000_48000.wav
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_mono_88200.wav $RESAMPLED_WAV_DIR/resampled_mono_88200_48000.wav
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_mono_96000.wav $RESAMPLED_WAV_DIR/resampled_mono_96000_48000.wav
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_mono_176400.wav $RESAMPLED_WAV_DIR/resampled_mono_176400_48000.wav
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_mono_192000.wav $RESAMPLED_WAV_DIR/resampled_mono_192000_48000.wav

$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_stereo_8000.wav $RESAMPLED_WAV_DIR/resampled_stereo_8000_48000.wav
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_stereo_11025.wav $RESAMPLED_WAV_DIR/resampled_stereo_11025_48000.wav
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_stereo_12000.wav $RESAMPLED_WAV_DIR/resampled_stereo_12000_48000.wav
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_stereo_16000.wav $RESAMPLED_WAV_DIR/resampled_stereo_16000_48000.wav
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_stereo_22050.wav $RESAMPLED_WAV_DIR/resampled_stereo_22050_48000.wav
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_stereo_24000.wav $RESAMPLED_WAV_DIR/resampled_stereo_24000_48000.wav
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_stereo_32000.wav $RESAMPLED_WAV_DIR/resampled_stereo_32000_48000.wav
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_stereo_44100.wav $RESAMPLED_WAV_DIR/resampled_stereo_44100_48000.wav
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_stereo_48000.wav $RESAMPLED_WAV_DIR/resampled_stereo_48000_48000.wav
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_stereo_88200.wav $RESAMPLED_WAV_DIR/resampled_stereo_88200_48000.wav
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_stereo_96000.wav $RESAMPLED_WAV_DIR/resampled_stereo_96000_48000.wav
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_stereo_176400.wav $RESAMPLED_WAV_DIR/resampled_stereo_176400_48000.wav
$RESAMPLE_WAV -f 48000 $ORIG_WAV_DIR/sweep_stereo_192000.wav $RESAMPLED_WAV_DIR/resampled_stereo_192000_48000.wav

echo "------------------------"

# generate spectrogram images
$UTILS_DIR/gen_spectrograms.sh $RESAMPLED_WAV_DIR $RESAMPLED_SPECTROGRAM_DIR

echo "========================="
echo