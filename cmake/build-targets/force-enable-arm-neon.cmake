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

add_definitions(-DCXXPH_CONFIG_RUNTIME_FEATURE_CHECK_ARM_NEON=0)

# for dependent libraries
set(CXXDASP_DEP_LIBS_CMAKE_CACHE_ARGS ${CXXDASP_DEP_LIBS_CMAKE_CACHE_ARGS} -DCMAKE_TOOLCHAIN_FILE:STRING=${CMAKE_CURRENT_LIST_FILE})
