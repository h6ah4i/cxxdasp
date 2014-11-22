//
//    Copyright (C) 2014 Haruki Hasegawa
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//

#include <cxxdasp/filter/biquad/f32_stereo_neon_biquad_tdf2_core_operator.hpp>

#if CXXPH_COMPILER_SUPPORTS_ARM_NEON

namespace cxxdasp {
namespace filter {
namespace impl {

#if CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM

static void perform_phase1(const float32_t *CXXPH_RESTRICT src, float32_t *CXXPH_RESTRICT dest, unsigned int n,
                           const float32_t *CXXPH_RESTRICT b_, const float32_t *CXXPH_RESTRICT ia_,
                           float32_t *CXXPH_RESTRICT s_) CXXPH_NOEXCEPT;

static void perform_phase1(float32_t *CXXPH_RESTRICT src_dest, unsigned int n, const float32_t *CXXPH_RESTRICT b_,
                           const float32_t *CXXPH_RESTRICT ia_, float32_t *CXXPH_RESTRICT s_) CXXPH_NOEXCEPT
{
    // NOTE: it's safe to pass the same buffer for the source and destination.
    perform_phase1(src_dest, src_dest, n, b_, ia_, s_);
}

static void perform_phase1(const float32_t *CXXPH_RESTRICT src, float32_t *CXXPH_RESTRICT dest, unsigned int n,
                           const float32_t *CXXPH_RESTRICT b_, const float32_t *CXXPH_RESTRICT ia_,
                           float32_t *CXXPH_RESTRICT s_) CXXPH_NOEXCEPT
{

    int cnt = n >> 2;
    asm volatile("pld [%[src], #64]\n\t"
                 "pld [%[dest], #64]\n\t"
                 "vld1.32 {d4, d5}, [%[b]]\n\t"
                 "vld1.32 {d6}, [%[ia]]\n\t"
                 "vld1.32 {d24, d25}, [%[s]]\n"
                 "1:\n\t"
                 "vld1.32 {d26, d27}, [%[src]]!\n\t"
                 "\n\t"
                 "@ === 0A ===\n\t"
                 "vmul.f32 d0, d26, d4[0]\n\t"
                 "vmul.f32 d18, d26, d4[1]\n\t"
                 "vmul.f32 d19, d26, d5[0]\n\t"
                 "@ === 1A ===\n\t"
                 "vmul.f32 d1, d27, d4[0]\n\t"
                 "vmul.f32 d20, d27, d4[1]\n\t"
                 "vmul.f32 d21, d27, d5[0]\n\t"
                 "@ === 0B ===\n\t"
                 "vadd.f32 d0, d0, d24\n\t"
                 "vadd.f32 d18, d18, d25\n\t"
                 "\n\t"
                 "vld1.32 {d28, d29}, [%[src]]!\n\t"
                 "\n\t"
                 "@ === 0C ===\n\t"
                 "vmla.f32 d18, d0, d6[0]\n\t"
                 "vmla.f32 d19, d0, d6[1]\n\t"
                 "@ === 2A ===\n\t"
                 "vmul.f32 d2, d28, d4[0]\n\t"
                 "vmul.f32 d22, d28, d4[1]\n\t"
                 "vmul.f32 d23, d28, d5[0]\n\t"
                 "@ === 1B ===\n\t"
                 "vadd.f32 d1, d1, d18\n\t"
                 "vadd.f32 d20, d20, d19\n\t"
                 "@ === 1C ===\n\t"
                 "vmla.f32 d20, d1, d6[0]\n\t"
                 "vmla.f32 d21, d1, d6[1]\n\t"
                 "@ === 3A ===\n\t"
                 "vmul.f32 d3, d29, d4[0]\n\t"
                 "vmul.f32 d24, d29, d4[1]\n\t"
                 "vmul.f32 d25, d29, d5[0]\n\t"
                 "@ === 2B ===\n\t"
                 "vadd.f32 d2, d2, d20\n\t"
                 "vadd.f32 d22, d22, d21\n\t"
                 "@ === 2C ===\n\t"
                 "vmla.f32 d22, d2, d6[0]\n\t"
                 "vmla.f32 d23, d2, d6[1]\n\t"
                 "\n\t"
                 "pld [%[dest], #128]\n\t"
                 "vst1.32 {d0, d1}, [%[dest]]!\n\t"
                 "pld [%[src], #64]\n\t"
                 "subs %[cnt], %[cnt], #1\n\t"
                 "\n\t"
                 "@ === 3B ===\n\t"
                 "vadd.f32 d3, d3, d22\n\t"
                 "vadd.f32 d24, d24, d23\n\t"
                 "@ === 3C ===\n\t"
                 "vmla.f32 d24, d3, d6[0]\n\t"
                 "vmla.f32 d25, d3, d6[1]\n\t"
                 "\n\t"
                 "vst1.32 {d2, d3}, [%[dest]]!\n\t"
                 "\n\t"
                 "bne 1b\n\t"
                 "\n\t"
                 "vst1.32 {d24, d25}, [%[s]]\n\t"
                 ""
                 : [src] "+r"(src), [dest] "+r"(dest), [cnt] "+r"(cnt)
                 : [b] "r"(b_), [ia] "r"(ia_), [s] "r"(s_)
                 : "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d16", "d17", "d18", "d19", "d20", "d21", "d22", "d23",
                   "d24", "d25", "d26", "d27", "d28", "d29");
}

#elif CXXPH_TARGET_ARCH == CXXPH_ARCH_ARM64

static inline float32x2x2_t vld1_f32x2x2(const float32_t *s)
{
    const float32x4_t x = vld1q_f32(s);
    float32x2x2_t r;
    r.val[0] = vget_low_f32(x);
    r.val[1] = vget_high_f32(x);
    return r;
}

static inline void vst1_f32x2x2(float32_t *d, float32x2x2_t &x) { vst1q_f32(d, vcombine_f32(x.val[0], x.val[1])); }

static inline float32x2x4_t vld1_f32x2x4(const float32_t *s)
{
    const float32x4_t x0 = vld1q_f32(&s[0]);
    const float32x4_t x1 = vld1q_f32(&s[4]);
    float32x2x4_t r;
    r.val[0] = vget_low_f32(x0);
    r.val[1] = vget_high_f32(x0);
    r.val[2] = vget_low_f32(x1);
    r.val[3] = vget_high_f32(x1);
    return r;
}

static inline void vst1_f32x2x4(float32_t *d, float32x2x4_t &x)
{
    vst1q_f32(&d[0], vcombine_f32(x.val[0], x.val[1]));
    vst1q_f32(&d[4], vcombine_f32(x.val[2], x.val[3]));
}

static void perform_phase1(float32_t *CXXPH_RESTRICT src_dest, unsigned int n, const float32_t *CXXPH_RESTRICT b_,
                           const float32_t *CXXPH_RESTRICT ia_, float32_t *CXXPH_RESTRICT s_) CXXPH_NOEXCEPT
{

    const float32x4_t b = vld1q_f32(&b_[0]);
    const float32x2_t ia = vld1_f32(&ia_[0]);

    float32x2x2_t s = vld1_f32x2x2(s_);
    float32x2x2_t t, u, v, w;

    for (int i = 0; i < (n << 1); i += 8) {
        float32x2x4_t x;
        float32x2x4_t y;

        x = vld1_f32x2x4(&src_dest[i]);
        __builtin_prefetch(&src_dest[i + 8], 0, 1);

        // 0A
        y.val[0] = vmul_lane_f32(x.val[0], vget_low_f32(b), 0);
        t.val[0] = vmul_lane_f32(x.val[0], vget_low_f32(b), 1);
        t.val[1] = vmul_lane_f32(x.val[0], vget_high_f32(b), 0);

        // 1A
        y.val[1] = vmul_lane_f32(x.val[1], vget_low_f32(b), 0);
        u.val[0] = vmul_lane_f32(x.val[1], vget_low_f32(b), 1);
        u.val[1] = vmul_lane_f32(x.val[1], vget_high_f32(b), 0);

        // 0B
        y.val[0] = vadd_f32(y.val[0], s.val[0]);
        t.val[0] = vadd_f32(t.val[0], s.val[1]);

        // 0C
        t.val[0] = vmla_lane_f32(t.val[0], y.val[0], ia, 0);
        t.val[1] = vmla_lane_f32(t.val[1], y.val[0], ia, 1);

        // 2A
        y.val[2] = vmul_lane_f32(x.val[2], vget_low_f32(b), 0);
        v.val[0] = vmul_lane_f32(x.val[2], vget_low_f32(b), 1);
        v.val[1] = vmul_lane_f32(x.val[2], vget_high_f32(b), 0);

        // 1B
        y.val[1] = vadd_f32(y.val[1], t.val[0]);
        u.val[0] = vadd_f32(u.val[0], t.val[1]);

        // 1C
        u.val[0] = vmla_lane_f32(u.val[0], y.val[1], ia, 0);
        u.val[1] = vmla_lane_f32(u.val[1], y.val[1], ia, 1);

        // 3A
        y.val[3] = vmul_lane_f32(x.val[3], vget_low_f32(b), 0);
        w.val[0] = vmul_lane_f32(x.val[3], vget_low_f32(b), 1);
        w.val[1] = vmul_lane_f32(x.val[3], vget_high_f32(b), 0);

        // 2B
        y.val[2] = vadd_f32(y.val[2], u.val[0]);
        v.val[0] = vadd_f32(v.val[0], u.val[1]);

        // 2C
        v.val[0] = vmla_lane_f32(v.val[0], y.val[2], ia, 0);
        v.val[1] = vmla_lane_f32(v.val[1], y.val[2], ia, 1);

        // 3B
        y.val[3] = vadd_f32(y.val[3], v.val[0]);
        w.val[0] = vadd_f32(w.val[0], v.val[1]);

        // 3C
        w.val[0] = vmla_lane_f32(w.val[0], y.val[3], ia, 0);
        w.val[1] = vmla_lane_f32(w.val[1], y.val[3], ia, 1);

        s = w;

        vst1_f32x2x4(&src_dest[i], y);
    }

    vst1_f32x2x2(&s_[0], s);
}

static void perform_phase1(const float32_t *CXXPH_RESTRICT src, float32_t *CXXPH_RESTRICT dest, unsigned int n,
                           const float32_t *CXXPH_RESTRICT b_, const float32_t *CXXPH_RESTRICT ia_,
                           float32_t *CXXPH_RESTRICT s_) CXXPH_NOEXCEPT
{

    const float32x4_t b = vld1q_f32(&b_[0]);
    const float32x2_t ia = vld1_f32(&ia_[0]);

    float32x2x2_t s = vld1_f32x2x2(s_);
    float32x2x2_t t, u, v, w;

    for (int i = 0; i < (n << 1); i += 8) {
        float32x2x4_t x;
        float32x2x4_t y;

        x = vld1_f32x2x4(&src[i]);
        __builtin_prefetch(&src[i + 8], 0, 1);

        // 0A
        y.val[0] = vmul_lane_f32(x.val[0], vget_low_f32(b), 0);
        t.val[0] = vmul_lane_f32(x.val[0], vget_low_f32(b), 1);
        t.val[1] = vmul_lane_f32(x.val[0], vget_high_f32(b), 0);

        // 1A
        y.val[1] = vmul_lane_f32(x.val[1], vget_low_f32(b), 0);
        u.val[0] = vmul_lane_f32(x.val[1], vget_low_f32(b), 1);
        u.val[1] = vmul_lane_f32(x.val[1], vget_high_f32(b), 0);

        // 0B
        y.val[0] = vadd_f32(y.val[0], s.val[0]);
        t.val[0] = vadd_f32(t.val[0], s.val[1]);

        // 0C
        t.val[0] = vmla_lane_f32(t.val[0], y.val[0], ia, 0);
        t.val[1] = vmla_lane_f32(t.val[1], y.val[0], ia, 1);

        // 2A
        y.val[2] = vmul_lane_f32(x.val[2], vget_low_f32(b), 0);
        v.val[0] = vmul_lane_f32(x.val[2], vget_low_f32(b), 1);
        v.val[1] = vmul_lane_f32(x.val[2], vget_high_f32(b), 0);

        // 1B
        y.val[1] = vadd_f32(y.val[1], t.val[0]);
        u.val[0] = vadd_f32(u.val[0], t.val[1]);

        // 1C
        u.val[0] = vmla_lane_f32(u.val[0], y.val[1], ia, 0);
        u.val[1] = vmla_lane_f32(u.val[1], y.val[1], ia, 1);

        // 3A
        y.val[3] = vmul_lane_f32(x.val[3], vget_low_f32(b), 0);
        w.val[0] = vmul_lane_f32(x.val[3], vget_low_f32(b), 1);
        w.val[1] = vmul_lane_f32(x.val[3], vget_high_f32(b), 0);

        // 2B
        y.val[2] = vadd_f32(y.val[2], u.val[0]);
        v.val[0] = vadd_f32(v.val[0], u.val[1]);

        // 2C
        v.val[0] = vmla_lane_f32(v.val[0], y.val[2], ia, 0);
        v.val[1] = vmla_lane_f32(v.val[1], y.val[2], ia, 1);

        // 3B
        y.val[3] = vadd_f32(y.val[3], v.val[0]);
        w.val[0] = vadd_f32(w.val[0], v.val[1]);

        // 3C
        w.val[0] = vmla_lane_f32(w.val[0], y.val[3], ia, 0);
        w.val[1] = vmla_lane_f32(w.val[1], y.val[3], ia, 1);

        s = w;

        vst1_f32x2x4(&dest[i], y);
        __builtin_prefetch(&dest[i + 8], 1, 1);
    }

    vst1_f32x2x2(&s_[0], s);
}

#endif

static void perform_phase2(float32_t *CXXPH_RESTRICT src_dest, unsigned int n, const float32_t *CXXPH_RESTRICT b_,
                           const float32_t *CXXPH_RESTRICT ia_, float32_t *CXXPH_RESTRICT s_) CXXPH_NOEXCEPT
{

    const float32x4_t b = vld1q_f32(&b_[0]);
    const float32x2_t ia = vld1_f32(&ia_[0]);

    float32x2_t s1 = vld1_f32(&s_[2 * 0]);
    float32x2_t s2 = vld1_f32(&s_[2 * 1]);
    float32x2_t x0, y0;

    for (int i = 0; i < n; ++i) {
        x0 = vld1_f32(&src_dest[2 * i]);

        y0 = vmla_lane_f32(s1, x0, vget_low_f32(b), 0);
        s1 = vmla_lane_f32(s2, x0, vget_low_f32(b), 1);
        s2 = vmul_lane_f32(x0, vget_high_f32(b), 0);
        s1 = vmla_lane_f32(s1, y0, ia, 0);
        s2 = vmla_lane_f32(s2, y0, ia, 1);

        vst1_f32(&src_dest[2 * i], y0);
    }

    vst1_f32(&s_[2 * 0], s1);
    vst1_f32(&s_[2 * 1], s2);
}

static void perform_phase2(const float32_t *CXXPH_RESTRICT src, float32_t *CXXPH_RESTRICT dest, unsigned int n,
                           const float32_t *CXXPH_RESTRICT b_, const float32_t *CXXPH_RESTRICT ia_,
                           float32_t *CXXPH_RESTRICT s_) CXXPH_NOEXCEPT
{

    const float32x4_t b = vld1q_f32(&b_[0]);
    const float32x2_t ia = vld1_f32(&ia_[0]);

    float32x2_t s1 = vld1_f32(&s_[2 * 0]);
    float32x2_t s2 = vld1_f32(&s_[2 * 1]);
    float32x2_t x0, y0;

    for (int i = 0; i < n; ++i) {
        x0 = vld1_f32(&src[2 * i]);

        y0 = vmla_lane_f32(s1, x0, vget_low_f32(b), 0);
        s1 = vmla_lane_f32(s2, x0, vget_low_f32(b), 1);
        s2 = vmul_lane_f32(x0, vget_high_f32(b), 0);
        s1 = vmla_lane_f32(s1, y0, ia, 0);
        s2 = vmla_lane_f32(s2, y0, ia, 1);

        vst1_f32(&dest[2 * i], y0);
    }

    vst1_f32(&s_[2 * 0], s1);
    vst1_f32(&s_[2 * 1], s2);
}

void f32_stereo_neon_biquad_transposed_direct_form_2_core_operator_impl::perform(
    float32_t *CXXPH_RESTRICT src_dest, unsigned int n, const float32_t *CXXPH_RESTRICT b_,
    const float32_t *CXXPH_RESTRICT ia_, float32_t *CXXPH_RESTRICT s_) CXXPH_NOEXCEPT
{

    const int n1 = (n >> 2) << 2; // (n / 4) * 4;
    const int n2 = (n & 0x3);     // n % 4

    if (n1 != 0) {
        perform_phase1(&src_dest[2 * 0], n1, b_, ia_, s_);
    }

    if (n2 != 0) {
        perform_phase2(&src_dest[2 * n1], n2, b_, ia_, s_);
    }
}

void f32_stereo_neon_biquad_transposed_direct_form_2_core_operator_impl::perform(
    const float32_t *CXXPH_RESTRICT src, float32_t *CXXPH_RESTRICT dest, unsigned int n,
    const float32_t *CXXPH_RESTRICT b_, const float32_t *CXXPH_RESTRICT ia_, float32_t *CXXPH_RESTRICT s_)
    CXXPH_NOEXCEPT
{

    const int n1 = (n >> 2) << 2; // (n / 4) * 4;
    const int n2 = (n & 0x3);     // n % 4

    if (n1 != 0) {
        perform_phase1(&src[2 * 0], &dest[2 * 0], n1, b_, ia_, s_);
    }

    if (n2 != 0) {
        perform_phase2(&src[2 * n1], &dest[2 * n1], n2, b_, ia_, s_);
    }
}

} // namespace impl
} // namespace filter
} // namespace cxxdasp

#endif // CXXPH_COMPILER_SUPPORTS_ARM_NEON
