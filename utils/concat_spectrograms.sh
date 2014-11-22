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
if [ "$#" -lt 3 ]; then
    echo "Usage: $0 src1_image_dir src2_image_dir dest_image_dir"
    exit 1
fi

SRC1_DIR=$1
SRC2_DIR=$2
DEST_DIR=$3
IMAGE_MAGICK_CONVERT=convert

if [ ! -d $DEST_DIR ]; then
    mkdir -p $DEST_DIR
fi

TARGET_FILE_LIST=""

# list up files
for src1_file in $(ls $SRC1_DIR/*.png); do
    fname=$(basename $src1_file)
    src2_file=$SRC2_DIR/$fname
    dest_file=$DEST_DIR/$fname

    if [ -f $src2_file ]; then
        TARGET_FILE_LIST="$TARGET_FILE_LIST:$fname"
    fi
done

# replace ":" -> "\n"
TARGET_FILE_LIST=$(echo "$TARGET_FILE_LIST" | sed "s/:/\n/g")
# remove empty line
TARGET_FILE_LIST=$(echo "$TARGET_FILE_LIST" | sed "/^$/d")

# check GNU parallel is available
if command -v parallel >/dev/null 2>&1; then
    # echo "GNU parallel is available"
    echo "$TARGET_FILE_LIST" | parallel -a - $IMAGE_MAGICK_CONVERT $SRC1_DIR/{} $SRC2_DIR/{} +append $DEST_DIR/{}
else
    # echo "GNU parallel is not available"
    for fname in $(echo $TARGET_FILE_LIST); do
        echo "$fname"
        $IMAGE_MAGICK_CONVERT $SRC1_DIR/$fname $SRC2_DIR/$fname +append $DEST_DIR/$fname
    done
fi
