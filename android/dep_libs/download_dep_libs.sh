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

pushd $SCRIPT_DIR

# remove temporary cmake build directory
rm -rf ./build

# create temporary cmake build directory
mkdir ./build
pushd build

cmake -G "Unix Makefiles" ../cmake_download_dep_libs

# NOTE:
# If crash occurs on Windows x64 with GNU make 3.81, 
# please download newer binary from here:
# http://www.equation.com/servlet/equation.cmd?fa=make
make

popd # build

# remove temporary cmake build directory
rm -rf ./build

popd # $SCRIPT_DIR
