#!/bin/sh
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

# ignore error
set +e

sox -c 1 -r 44100 -b 64 -e floating-point --endian little -traw f64_to_f64_mono.raw f64_to_f64_mono.wav
sox -c 1 -r 44100 -b 64 -e floating-point --endian little -traw f32_to_f64_mono.raw f32_to_f64_mono.wav
sox -c 1 -r 44100 -b 64 -e floating-point --endian little -traw s32_to_f64_mono.raw s32_to_f64_mono.wav
sox -c 1 -r 44100 -b 64 -e floating-point --endian little -traw s16_to_f64_mono.raw s16_to_f64_mono.wav

sox -c 2 -r 44100 -b 64 -e floating-point --endian little -traw f64_to_f64_stereo.raw f64_to_f64_stereo.wav
sox -c 2 -r 44100 -b 64 -e floating-point --endian little -traw f32_to_f64_stereo.raw f32_to_f64_stereo.wav
sox -c 2 -r 44100 -b 64 -e floating-point --endian little -traw s32_to_f64_stereo.raw s32_to_f64_stereo.wav
sox -c 2 -r 44100 -b 64 -e floating-point --endian little -traw s16_to_f64_stereo.raw s16_to_f64_stereo.wav

sox -c 1 -r 44100 -b 32 -e floating-point --endian little -traw f64_to_f32_mono.raw f64_to_f32_mono.wav
sox -c 1 -r 44100 -b 32 -e floating-point --endian little -traw f32_to_f32_mono.raw f32_to_f32_mono.wav
sox -c 1 -r 44100 -b 32 -e floating-point --endian little -traw s32_to_f32_mono.raw s32_to_f32_mono.wav
sox -c 1 -r 44100 -b 32 -e floating-point --endian little -traw s16_to_f32_mono.raw s16_to_f32_mono.wav

sox -c 2 -r 44100 -b 32 -e floating-point --endian little -traw f64_to_f32_stereo.raw f64_to_f32_stereo.wav
sox -c 2 -r 44100 -b 32 -e floating-point --endian little -traw f32_to_f32_stereo.raw f32_to_f32_stereo.wav
sox -c 2 -r 44100 -b 32 -e floating-point --endian little -traw s32_to_f32_stereo.raw s32_to_f32_stereo.wav
sox -c 2 -r 44100 -b 32 -e floating-point --endian little -traw s16_to_f32_stereo.raw s16_to_f32_stereo.wav

sox -c 1 -r 44100 -b 32 -e signed-integer --endian little -traw f64_to_s32_mono.raw f64_to_s32_mono.wav
sox -c 1 -r 44100 -b 32 -e signed-integer --endian little -traw f32_to_s32_mono.raw f32_to_s32_mono.wav
sox -c 1 -r 44100 -b 32 -e signed-integer --endian little -traw s32_to_s32_mono.raw s32_to_s32_mono.wav
sox -c 1 -r 44100 -b 32 -e signed-integer --endian little -traw s16_to_s32_mono.raw s16_to_s32_mono.wav

sox -c 2 -r 44100 -b 32 -e signed-integer --endian little -traw f64_to_s32_stereo.raw f64_to_s32_stereo.wav
sox -c 2 -r 44100 -b 32 -e signed-integer --endian little -traw f32_to_s32_stereo.raw f32_to_s32_stereo.wav
sox -c 2 -r 44100 -b 32 -e signed-integer --endian little -traw s32_to_s32_stereo.raw s32_to_s32_stereo.wav
sox -c 2 -r 44100 -b 32 -e signed-integer --endian little -traw s16_to_s32_stereo.raw s16_to_s32_stereo.wav

sox -c 1 -r 44100 -b 16 -e signed-integer --endian little -traw f64_to_s16_mono.raw f64_to_s16_mono.wav
sox -c 1 -r 44100 -b 16 -e signed-integer --endian little -traw f32_to_s16_mono.raw f32_to_s16_mono.wav
sox -c 1 -r 44100 -b 16 -e signed-integer --endian little -traw s32_to_s16_mono.raw s32_to_s16_mono.wav
sox -c 1 -r 44100 -b 16 -e signed-integer --endian little -traw s16_to_s16_mono.raw s16_to_s16_mono.wav

sox -c 2 -r 44100 -b 16 -e signed-integer --endian little -traw f64_to_s16_stereo.raw f64_to_s16_stereo.wav
sox -c 2 -r 44100 -b 16 -e signed-integer --endian little -traw f32_to_s16_stereo.raw f32_to_s16_stereo.wav
sox -c 2 -r 44100 -b 16 -e signed-integer --endian little -traw s32_to_s16_stereo.raw s32_to_s16_stereo.wav
sox -c 2 -r 44100 -b 16 -e signed-integer --endian little -traw s16_to_s16_stereo.raw s16_to_s16_stereo.wav

# SSE
sox -c 1 -r 44100 -b 32 -e floating-point --endian little -traw s16_to_f32_mono_sse.raw s16_to_f32_mono_sse.wav
sox -c 2 -r 44100 -b 32 -e floating-point --endian little -traw s16_to_f32_stereo_sse.raw s16_to_f32_stereo_sse.wav
sox -c 2 -r 44100 -b 32 -e floating-point --endian little -traw s16_mono_to_f32_stereo_sse.raw s16_mono_to_f32_stereo_sse.wav

sox -c 1 -r 44100 -b 16 -e signed-integer --endian little -traw f32_to_s16_mono_sse.raw f32_to_s16_mono_sse.wav
sox -c 2 -r 44100 -b 16 -e signed-integer --endian little -traw f32_to_s16_stereo_sse.raw f32_to_s16_stereo_sse.wav

# NEON
sox -c 1 -r 44100 -b 32 -e floating-point --endian little -traw s16_to_f32_mono_neon.raw s16_to_f32_mono_neon.wav
sox -c 2 -r 44100 -b 32 -e floating-point --endian little -traw s16_to_f32_stereo_neon.raw s16_to_f32_stereo_neon.wav
sox -c 2 -r 44100 -b 32 -e floating-point --endian little -traw s16_mono_to_f32_stereo_neon.raw s16_mono_to_f32_stereo_neon.wav

sox -c 1 -r 44100 -b 16 -e signed-integer --endian little -traw f32_to_s16_mono_neon.raw f32_to_s16_mono_neon.wav
sox -c 2 -r 44100 -b 16 -e signed-integer --endian little -traw f32_to_s16_stereo_neon.raw f32_to_s16_stereo_neon.wav

# NEON (fast version)
sox -c 1 -r 44100 -b 32 -e floating-point --endian little -traw s16_to_f32_mono_neon_fast.raw s16_to_f32_mono_neon_fast.wav
sox -c 2 -r 44100 -b 32 -e floating-point --endian little -traw s16_to_f32_stereo_neon_fast.raw s16_to_f32_stereo_neon_fast.wav
sox -c 2 -r 44100 -b 32 -e floating-point --endian little -traw s16_mono_to_f32_stereo_neon_fast.raw s16_mono_to_f32_stereo_neon_fast.wav

sox -c 1 -r 44100 -b 16 -e signed-integer --endian little -traw f32_to_s16_mono_neon_fast.raw f32_to_s16_mono_neon_fast.wav
sox -c 2 -r 44100 -b 16 -e signed-integer --endian little -traw f32_to_s16_stereo_neon_fast.raw f32_to_s16_stereo_neon_fast.wav