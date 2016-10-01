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

# check arguments
if [ "$#" -lt 2 ]; then
    echo "Usage: $0 {simple} quality"
    exit 1
fi

RESAMPLER=$1
QUALITY=$2

BASE_DIR=$SCRIPT_DIR/../..
UTILS_DIR=$BASE_DIR/utils

LOCAL_BENCH_TOP_DIR="$BASE_DIR/android/benchmark_resampler"
LOCAL_RESAMPLED_RAW_DIR="$LOCAL_BENCH_TOP_DIR/tmp"
LOCAL_RESAMPLED_WAV_DIR="$LOCAL_BENCH_TOP_DIR/resampled_simple_q"$QUALITY
LOCAL_RESAMPLED_SPECTROGRAM_DIR=$LOCAL_RESAMPLED_WAV_DIR"_spectrogram"

REMOTE_BASE_PATH=/data/local/tmp

REMOTE_BENCH_TOP_DIR="$REMOTE_BASE_PATH/benchmark_resampler"
REMOTE_ORIG_RAW_DIR="$REMOTE_BENCH_TOP_DIR/original_sounds"
REMOTE_RESAMPLED_RAW_DIR="$REMOTE_BENCH_TOP_DIR/resampled_simple_q"$QUALITY


r_mkdir="adb shell mkdir"
r_simple_resampler="adb shell $REMOTE_BASE_PATH/$RESAMPLER-resampler"



echo "===[ $RESAMPLER-resampler (quality=$QUALITY) ]==="


#
# Run remote simple-resampler app
#

$r_mkdir $REMOTE_RESAMPLED_RAW_DIR


# to 44100 Hz
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_8000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_8000_44100.raw 1 8000 44100 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_11025.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_11025_44100.raw 1 11025 44100 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_12000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_12000_44100.raw 1 12000 44100 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_16000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_16000_44100.raw 1 16000 44100 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_22050.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_22050_44100.raw 1 22050 44100 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_24000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_24000_44100.raw 1 24000 44100 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_32000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_32000_44100.raw 1 32000 44100 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_44100.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_44100_44100.raw 1 44100 44100 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_48000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_48000_44100.raw 1 48000 44100 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_88200.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_88200_44100.raw 1 88200 44100 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_96000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_96000_44100.raw 1 96000 44100 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_176400.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_176400_44100.raw 1 176400 44100 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_192000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_192000_44100.raw 1 192000 44100 $QUALITY

$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_8000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_8000_44100.raw 2 8000 44100 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_11025.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_11025_44100.raw 2 11025 44100 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_12000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_12000_44100.raw 2 12000 44100 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_16000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_16000_44100.raw 2 16000 44100 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_22050.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_22050_44100.raw 2 22050 44100 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_24000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_24000_44100.raw 2 24000 44100 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_32000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_32000_44100.raw 2 32000 44100 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_44100.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_44100_44100.raw 2 44100 44100 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_48000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_48000_44100.raw 2 48000 44100 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_88200.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_88200_44100.raw 2 88200 44100 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_96000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_96000_44100.raw 2 96000 44100 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_176400.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_176400_44100.raw 2 176400 44100 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_192000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_192000_44100.raw 2 192000 44100 $QUALITY

# to 48000 Hz
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_8000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_8000_48000.raw 1 8000 48000 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_11025.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_11025_48000.raw 1 11025 48000 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_12000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_12000_48000.raw 1 12000 48000 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_16000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_16000_48000.raw 1 16000 48000 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_22050.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_22050_48000.raw 1 22050 48000 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_24000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_24000_48000.raw 1 24000 48000 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_32000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_32000_48000.raw 1 32000 48000 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_44100.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_44100_48000.raw 1 44100 48000 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_48000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_48000_48000.raw 1 48000 48000 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_88200.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_88200_48000.raw 1 88200 48000 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_96000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_96000_48000.raw 1 96000 48000 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_176400.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_176400_48000.raw 1 176400 48000 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_mono_192000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_mono_192000_48000.raw 1 192000 48000 $QUALITY

$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_8000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_8000_48000.raw 2 8000 48000 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_11025.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_11025_48000.raw 2 11025 48000 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_12000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_12000_48000.raw 2 12000 48000 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_16000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_16000_48000.raw 2 16000 48000 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_22050.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_22050_48000.raw 2 22050 48000 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_24000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_24000_48000.raw 2 24000 48000 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_32000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_32000_48000.raw 2 32000 48000 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_44100.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_44100_48000.raw 2 44100 48000 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_48000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_48000_48000.raw 2 48000 48000 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_88200.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_88200_48000.raw 2 88200 48000 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_96000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_96000_48000.raw 2 96000 48000 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_176400.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_176400_48000.raw 2 176400 48000 $QUALITY
$r_simple_resampler $REMOTE_ORIG_RAW_DIR/sweep_stereo_192000.raw $REMOTE_RESAMPLED_RAW_DIR/resampled_stereo_192000_48000.raw 2 192000 48000 $QUALITY

#
# pull remote files
#
if [ -d $LOCAL_RESAMPLED_RAW_DIR ]; then
    rm -r $LOCAL_RESAMPLED_RAW_DIR
fi
mkdir -p $LOCAL_RESAMPLED_RAW_DIR

adb pull $REMOTE_RESAMPLED_RAW_DIR $LOCAL_RESAMPLED_RAW_DIR


#
# convert to wav file
#
mkdir -p $LOCAL_RESAMPLED_WAV_DIR

for raw_file in $(find $LOCAL_RESAMPLED_RAW_DIR -type f -name '*.raw')
do
    file_name=$(basename ${raw_file})
    wav_file="$LOCAL_RESAMPLED_WAV_DIR/$(basename -s .raw $file_name).wav"
    channels=$(echo $file_name | sed "s/^resampled_\([a-z]*\)_\([0-9]*\)_\([0-9]*\).raw$/\1/g")
    src_freq=$(echo $file_name | sed "s/^resampled_\([a-z]*\)_\([0-9]*\)_\([0-9]*\).raw$/\2/g")
    dest_freq=$(echo $file_name | sed "s/^resampled_\([a-z]*\)_\([0-9]*\)_\([0-9]*\).raw$/\3/g")

    if [ $channels == "mono" ]; then
        num_channels=1
    elif [ $channels == "stereo" ]; then
        num_channels=2
    else
        exit 1
    fi

    echo "$wav_file"
    sox -t raw -c $num_channels -r $dest_freq -b 32 -e floating-point --endian little $raw_file $wav_file
done

echo "------------------------"

# generate spectrogram images
$UTILS_DIR/gen_spectrograms.sh $LOCAL_RESAMPLED_WAV_DIR $LOCAL_RESAMPLED_SPECTROGRAM_DIR

echo "========================="
echo