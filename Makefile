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

#
# Targets
#
.PHONY: doc run-benchmark gen-filter-coeffs

all:
	@echo "This library uses CMake. Please refer to the README file."

doc:
	doxygen ./utils/Doxyfile

run-resampler-benchmark:
	./utils/benchmark_resampler.sh

codestyle:
	find \
		./include ./source ./example ./test \
		-name "*.cpp" -or -name "*.hpp" \
		| xargs -i clang-format -i {}
