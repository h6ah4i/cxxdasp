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
    echo "Usage: $0 {simple-resampler | soxr-resampler | fresample-resampler}"
    exit 1
fi

BASENAME=$1

# resolve resampler app path
RESAMPLER_PATHS=(\
    "$BASE_DIR/build/cmake/example/$BASENAME/$BASENAME" \
    "$BASE_DIR/build/cmake/example/$BASENAME/Release/$BASENAME.exe" \
    "$BASE_DIR/build/cmake/example/$BASENAME/Debug/$BASENAME.exe" \
    )

RESAMPLER=""

for path in "${RESAMPLER_PATHS[@]}"; do
    if [ -x $path ]; then
        RESAMPLER=$path
        break
    fi
done

if [ "$RESAMPLER" == "" ]; then
    exit 1
fi

echo $RESAMPLER

exit 0
