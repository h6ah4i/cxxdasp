cxxdasp
===============

"cxxdasp" : Digital Audio Signal Processing library for C++


Features
---

- Specialized for some audio processing
    - Sample format conversion
    - Resampling (Sample rate conversion)
    - IIR filter (Biquad filter and Linear Trapezoidal Integrated State Variable Filter)
    - One dimensional FFT (requires corresponding backend FFT libraries)
- Small and simple C++ template based library
- SIMD optimized (SSE, NEON)
- Android NDK fully supported


Supported platforms
---

This library requires **C++11 compatible compiler**.

I have tested on these platforms listed below:

- Linux (Ubuntu 14.4 x86_64)
    - gcc 4.8
    - clang 3.4
- QEMU-Linux (arm, aarch64)
    - gcc 4.8
- OSX 10.9 (x86_64)
    - clang 3.4
- Windows (x86_64)
    - Visual Studio 2013
- Android (armeabi, armeabi-v7, armeabi-v7-hard, arm64-v8a, x86, x86_64, mips (*1), mips64(*1))
    - android-ndk-r9d (gcc 4.8, clang) (*2)
    - android-ndk-r10c (gcc 4.9, clang) (*2)

(*1) No special optimization is provided. Just can be compiled and not tested.
(*2) llvm-libc++ or gnustl is requred to build test applications.
   (Because gmock requires complete STL implementation. However cxxdasp library is compatible with stlport, llvm-libc++ and gnustl.)


Latest version
---

- Version 0.5.0  (November 23, 2014)



Documentation
---
See [Wiki](../../wiki).


License
---

See [`LICENSE`](LICENSE) for full of the license text.

    Copyright (C) 2014 Haruki Hasegawa

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.


### Notes

You also have to check dependent library/software licenses.


Dependent libraries
---

### Single-prefision FFT backend

- [PFFFT : a pretty fast FFT](https://bitbucket.org/jpommier/pffft)
    - [PFFFT (h6a h4i forked version)](https://bitbucket.org/h6a_h4i/pffft)
- [FFTS : The Fastest Fourier Transform in the South](https://github.com/anthonix/ffts)
- [Kiss FFT : "Keep It Simple, Stupid." FFT library](http://sourceforge.net/projects/kissfft/)
- [Ne10 : An Open Optimized Software Library Project for the ARM Architecture](http://projectne10.github.io/Ne10/)
- [FFTW : C subroutine library for computing the discrete Fourier transform](http://www.fftw.org/)

### Double-precision FFT backend

- [General Purpose FFT (Fast Fourier/Cosine/Sine Transform) Package](http://www.kurims.kyoto-u.ac.jp/~ooura/fft.html)
- [FFTW : C subroutine library for computing the discrete Fourier transform](http://www.fftw.org/)

### Misc.

- [Google Mock : Google C++ Mocking Framework](https://code.google.com/p/googlemock/)
- [The SoX Resampler library : High quality, one-dimensional sample-rate conversion library](http://sourceforge.net/projects/soxr/)
- [libfresample : Fast, free sample rate conversion](https://github.com/depp/libfresample)
- [android-ndk-profiler](https://code.google.com/p/android-ndk-profiler/)


## Dependent or Related softwares

- [CMake : The cross-platform, open-source build system](http://www.cmake.org/)
- [Scilab : Open source, cross-platform numerical computational package](http://www.scilab.org/)
- [sox : Sound eXchange](http://sox.sourceforge.net/)
- [ImageMagick](http://www.imagemagick.org/)
- [Audacity : Free Audio Editor and Recorder](http://audacity.sourceforge.net/)
- [Android NDK](https://developer.android.com/tools/sdk/ndk/index.html)
- [Doxygen : Generate documentation from source code](http://www.stack.nl/~dimitri/doxygen/)
- [Artistic Style : A Free, Fast and Small Automatic Formatter](http://astyle.sourceforge.net/astyle.html)
- [QEMU : Open source processor emulator](http://wiki.qemu.org/Main_Page)
- [Docker : Docker - An open platform for distributed applications for developers and sysadmins.](https://www.docker.com/)
