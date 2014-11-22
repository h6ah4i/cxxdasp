#include <cxxdasp/mixer/f32_mono_sse_mixer_core_operator.hpp>

#if (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)

#include <cxxporthelper/cstdint>
#include <cxxporthelper/x86_intrinsics.hpp>

#include <cxxdasp/utils/utils.hpp>

namespace cxxdasp {
namespace mixer {

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
template <typename SrcLoadOp, typename DestStoreOp>
static inline void sse_f32_mono_mul_scale(f32_mono_sse_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
                                          const f32_mono_sse_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
                                          const f32_mono_sse_mixer_core_operator::frame_type &scale, int n)
    CXXPH_NOEXCEPT
{

    const __m128 scale_m128 = _mm_load_ps1(&scale.sample[0]);

    for (int i = 0; i < n; i += 16) {
        const __m128 s0 = SrcLoadOp::load(reinterpret_cast<const float *>(&src[i + 0]));
        const __m128 s1 = SrcLoadOp::load(reinterpret_cast<const float *>(&src[i + 4]));
        const __m128 s2 = SrcLoadOp::load(reinterpret_cast<const float *>(&src[i + 8]));
        const __m128 s3 = SrcLoadOp::load(reinterpret_cast<const float *>(&src[i + 12]));

        const __m128 r0 = _mm_mul_ps(s0, scale_m128);
        const __m128 r1 = _mm_mul_ps(s1, scale_m128);
        const __m128 r2 = _mm_mul_ps(s2, scale_m128);
        const __m128 r3 = _mm_mul_ps(s3, scale_m128);

        DestStoreOp::store(reinterpret_cast<float *>(&dest[i + 0]), r0);
        DestStoreOp::store(reinterpret_cast<float *>(&dest[i + 4]), r1);
        DestStoreOp::store(reinterpret_cast<float *>(&dest[i + 8]), r2);
        DestStoreOp::store(reinterpret_cast<float *>(&dest[i + 12]), r3);
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
template <typename SrcLoadOp, typename DestLoadOp, typename DestStoreOp>
static inline void sse_f32_mono_mac_scale(f32_mono_sse_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
                                          const f32_mono_sse_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
                                          const f32_mono_sse_mixer_core_operator::frame_type &scale, int n)
    CXXPH_NOEXCEPT
{

    const __m128 scale_m128 = _mm_load_ps1(&scale.sample[0]);

    for (int i = 0; i < n; i += 16) {
        const __m128 s0 = SrcLoadOp::load(reinterpret_cast<const float *>(&src[i + 0]));
        const __m128 s1 = SrcLoadOp::load(reinterpret_cast<const float *>(&src[i + 4]));
        const __m128 s2 = SrcLoadOp::load(reinterpret_cast<const float *>(&src[i + 8]));
        const __m128 s3 = SrcLoadOp::load(reinterpret_cast<const float *>(&src[i + 12]));

        const __m128 d0 = DestLoadOp::load(reinterpret_cast<const float *>(&dest[i + 0]));
        const __m128 d1 = DestLoadOp::load(reinterpret_cast<const float *>(&dest[i + 4]));
        const __m128 d2 = DestLoadOp::load(reinterpret_cast<const float *>(&dest[i + 8]));
        const __m128 d3 = DestLoadOp::load(reinterpret_cast<const float *>(&dest[i + 12]));

        const __m128 r0 = _mm_add_ps(d0, _mm_mul_ps(s0, scale_m128));
        const __m128 r1 = _mm_add_ps(d1, _mm_mul_ps(s1, scale_m128));
        const __m128 r2 = _mm_add_ps(d2, _mm_mul_ps(s2, scale_m128));
        const __m128 r3 = _mm_add_ps(d3, _mm_mul_ps(s3, scale_m128));

        DestStoreOp::store(reinterpret_cast<float *>(&dest[i + 0]), r0);
        DestStoreOp::store(reinterpret_cast<float *>(&dest[i + 4]), r1);
        DestStoreOp::store(reinterpret_cast<float *>(&dest[i + 8]), r2);
        DestStoreOp::store(reinterpret_cast<float *>(&dest[i + 12]), r3);
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
template <typename SrcLoadOp, typename DestStoreOp, typename TableLoadOp>
static inline void
sse_f32_mono_mul_forward_table_and_scale(f32_mono_sse_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
                                         const f32_mono_sse_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
                                         const f32_mono_sse_mixer_core_operator::table_coeff_type *CXXPH_RESTRICT table,
                                         const f32_mono_sse_mixer_core_operator::frame_type &scale, int n)
    CXXPH_NOEXCEPT
{

    const __m128 scale_m128 = _mm_load_ps1(&scale.sample[0]);

    for (int i = 0; i < n; i += 16) {
        const __m128 s0 = SrcLoadOp::load(reinterpret_cast<const float *>(&src[i + 0]));
        const __m128 s1 = SrcLoadOp::load(reinterpret_cast<const float *>(&src[i + 4]));
        const __m128 s2 = SrcLoadOp::load(reinterpret_cast<const float *>(&src[i + 8]));
        const __m128 s3 = SrcLoadOp::load(reinterpret_cast<const float *>(&src[i + 12]));

        const __m128 t0 = TableLoadOp::load(reinterpret_cast<const float *>(&table[i + 0]));
        const __m128 t1 = TableLoadOp::load(reinterpret_cast<const float *>(&table[i + 4]));
        const __m128 t2 = TableLoadOp::load(reinterpret_cast<const float *>(&table[i + 8]));
        const __m128 t3 = TableLoadOp::load(reinterpret_cast<const float *>(&table[i + 12]));

        const __m128 r0 = _mm_mul_ps(s0, _mm_mul_ps(t0, scale_m128));
        const __m128 r1 = _mm_mul_ps(s1, _mm_mul_ps(t1, scale_m128));
        const __m128 r2 = _mm_mul_ps(s2, _mm_mul_ps(t2, scale_m128));
        const __m128 r3 = _mm_mul_ps(s3, _mm_mul_ps(t3, scale_m128));

        DestStoreOp::store(reinterpret_cast<float *>(&dest[i + 0]), r0);
        DestStoreOp::store(reinterpret_cast<float *>(&dest[i + 4]), r1);
        DestStoreOp::store(reinterpret_cast<float *>(&dest[i + 8]), r2);
        DestStoreOp::store(reinterpret_cast<float *>(&dest[i + 12]), r3);
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
template <typename SrcLoadOp, typename DestLoadOp, typename DestStoreOp, typename TableLoadOp>
static inline void
sse_f32_mono_mac_forward_table_and_scale(f32_mono_sse_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
                                         const f32_mono_sse_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
                                         const f32_mono_sse_mixer_core_operator::table_coeff_type *CXXPH_RESTRICT table,
                                         const f32_mono_sse_mixer_core_operator::frame_type &scale, int n)
    CXXPH_NOEXCEPT
{

    const __m128 scale_m128 = _mm_load_ps1(&scale.sample[0]);

    for (int i = 0; i < n; i += 16) {
        const __m128 s0 = SrcLoadOp::load(reinterpret_cast<const float *>(&src[i + 0]));
        const __m128 s1 = SrcLoadOp::load(reinterpret_cast<const float *>(&src[i + 4]));
        const __m128 s2 = SrcLoadOp::load(reinterpret_cast<const float *>(&src[i + 8]));
        const __m128 s3 = SrcLoadOp::load(reinterpret_cast<const float *>(&src[i + 12]));

        const __m128 d0 = DestLoadOp::load(reinterpret_cast<const float *>(&dest[i + 0]));
        const __m128 d1 = DestLoadOp::load(reinterpret_cast<const float *>(&dest[i + 4]));
        const __m128 d2 = DestLoadOp::load(reinterpret_cast<const float *>(&dest[i + 8]));
        const __m128 d3 = DestLoadOp::load(reinterpret_cast<const float *>(&dest[i + 12]));

        const __m128 t0 = TableLoadOp::load(reinterpret_cast<const float *>(&table[i + 0]));
        const __m128 t1 = TableLoadOp::load(reinterpret_cast<const float *>(&table[i + 4]));
        const __m128 t2 = TableLoadOp::load(reinterpret_cast<const float *>(&table[i + 8]));
        const __m128 t3 = TableLoadOp::load(reinterpret_cast<const float *>(&table[i + 12]));

        const __m128 r0 = _mm_add_ps(d0, _mm_mul_ps(s0, _mm_mul_ps(t0, scale_m128)));
        const __m128 r1 = _mm_add_ps(d1, _mm_mul_ps(s1, _mm_mul_ps(t1, scale_m128)));
        const __m128 r2 = _mm_add_ps(d2, _mm_mul_ps(s2, _mm_mul_ps(t2, scale_m128)));
        const __m128 r3 = _mm_add_ps(d3, _mm_mul_ps(s3, _mm_mul_ps(t3, scale_m128)));

        DestStoreOp::store(reinterpret_cast<float *>(&dest[i + 0]), r0);
        DestStoreOp::store(reinterpret_cast<float *>(&dest[i + 4]), r1);
        DestStoreOp::store(reinterpret_cast<float *>(&dest[i + 8]), r2);
        DestStoreOp::store(reinterpret_cast<float *>(&dest[i + 12]), r3);
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
template <typename SrcLoadOp, typename DestStoreOp, typename TableLoadOp>
static inline void sse_f32_mono_mul_backward_table_and_scale(
    f32_mono_sse_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
    const f32_mono_sse_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
    const f32_mono_sse_mixer_core_operator::table_coeff_type *CXXPH_RESTRICT table,
    const f32_mono_sse_mixer_core_operator::frame_type &scale, int n) CXXPH_NOEXCEPT
{

    const __m128 scale_m128 = _mm_load_ps1(&scale.sample[0]);

    for (int i = 0; i < n; i += 16) {
        const __m128 s0 = SrcLoadOp::load(reinterpret_cast<const float *>(&src[i + 0]));
        const __m128 s1 = SrcLoadOp::load(reinterpret_cast<const float *>(&src[i + 4]));
        const __m128 s2 = SrcLoadOp::load(reinterpret_cast<const float *>(&src[i + 8]));
        const __m128 s3 = SrcLoadOp::load(reinterpret_cast<const float *>(&src[i + 12]));

        table -= 16;
        const __m128 t3 = TableLoadOp::load(reinterpret_cast<const float *>(&table[1 + 0]));
        const __m128 t2 = TableLoadOp::load(reinterpret_cast<const float *>(&table[1 + 4]));
        const __m128 t1 = TableLoadOp::load(reinterpret_cast<const float *>(&table[1 + 8]));
        const __m128 t0 = TableLoadOp::load(reinterpret_cast<const float *>(&table[1 + 12]));

        const __m128 r0 = _mm_mul_ps(s0, _mm_mul_ps(_mm_shuffle_ps(t0, t0, _MM_SHUFFLE(0, 1, 2, 3)), scale_m128));
        const __m128 r1 = _mm_mul_ps(s1, _mm_mul_ps(_mm_shuffle_ps(t1, t1, _MM_SHUFFLE(0, 1, 2, 3)), scale_m128));
        const __m128 r2 = _mm_mul_ps(s2, _mm_mul_ps(_mm_shuffle_ps(t2, t2, _MM_SHUFFLE(0, 1, 2, 3)), scale_m128));
        const __m128 r3 = _mm_mul_ps(s3, _mm_mul_ps(_mm_shuffle_ps(t3, t3, _MM_SHUFFLE(0, 1, 2, 3)), scale_m128));

        DestStoreOp::store(reinterpret_cast<float *>(&dest[i + 0]), r0);
        DestStoreOp::store(reinterpret_cast<float *>(&dest[i + 4]), r1);
        DestStoreOp::store(reinterpret_cast<float *>(&dest[i + 8]), r2);
        DestStoreOp::store(reinterpret_cast<float *>(&dest[i + 12]), r3);
    }
}
#endif

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
template <typename SrcLoadOp, typename DestLoadOp, typename DestStoreOp, typename TableLoadOp>
static inline void sse_f32_mono_mac_backward_table_and_scale(
    f32_mono_sse_mixer_core_operator::frame_type *CXXPH_RESTRICT dest,
    const f32_mono_sse_mixer_core_operator::frame_type *CXXPH_RESTRICT src,
    const f32_mono_sse_mixer_core_operator::table_coeff_type *CXXPH_RESTRICT table,
    const f32_mono_sse_mixer_core_operator::frame_type &scale, int n) CXXPH_NOEXCEPT
{

    const __m128 scale_m128 = _mm_load_ps1(&scale.sample[0]);

    for (int i = 0; i < n; i += 16) {
        const __m128 s0 = SrcLoadOp::load(reinterpret_cast<const float *>(&src[i + 0]));
        const __m128 s1 = SrcLoadOp::load(reinterpret_cast<const float *>(&src[i + 4]));
        const __m128 s2 = SrcLoadOp::load(reinterpret_cast<const float *>(&src[i + 8]));
        const __m128 s3 = SrcLoadOp::load(reinterpret_cast<const float *>(&src[i + 12]));

        const __m128 d0 = DestLoadOp::load(reinterpret_cast<const float *>(&dest[i + 0]));
        const __m128 d1 = DestLoadOp::load(reinterpret_cast<const float *>(&dest[i + 4]));
        const __m128 d2 = DestLoadOp::load(reinterpret_cast<const float *>(&dest[i + 8]));
        const __m128 d3 = DestLoadOp::load(reinterpret_cast<const float *>(&dest[i + 12]));

        table -= 16;
        const __m128 t3 = TableLoadOp::load(reinterpret_cast<const float *>(&table[1 + 0]));
        const __m128 t2 = TableLoadOp::load(reinterpret_cast<const float *>(&table[1 + 4]));
        const __m128 t1 = TableLoadOp::load(reinterpret_cast<const float *>(&table[1 + 8]));
        const __m128 t0 = TableLoadOp::load(reinterpret_cast<const float *>(&table[1 + 12]));

        const __m128 r0 =
            _mm_add_ps(d0, _mm_mul_ps(s0, _mm_mul_ps(_mm_shuffle_ps(t0, t0, _MM_SHUFFLE(0, 1, 2, 3)), scale_m128)));
        const __m128 r1 =
            _mm_add_ps(d1, _mm_mul_ps(s1, _mm_mul_ps(_mm_shuffle_ps(t1, t1, _MM_SHUFFLE(0, 1, 2, 3)), scale_m128)));
        const __m128 r2 =
            _mm_add_ps(d2, _mm_mul_ps(s2, _mm_mul_ps(_mm_shuffle_ps(t2, t2, _MM_SHUFFLE(0, 1, 2, 3)), scale_m128)));
        const __m128 r3 =
            _mm_add_ps(d3, _mm_mul_ps(s3, _mm_mul_ps(_mm_shuffle_ps(t3, t3, _MM_SHUFFLE(0, 1, 2, 3)), scale_m128)));

        DestStoreOp::store(reinterpret_cast<float *>(&dest[i + 0]), r0);
        DestStoreOp::store(reinterpret_cast<float *>(&dest[i + 4]), r1);
        DestStoreOp::store(reinterpret_cast<float *>(&dest[i + 8]), r2);
        DestStoreOp::store(reinterpret_cast<float *>(&dest[i + 12]), r3);
    }
}
#endif

void f32_mono_sse_mixer_core_operator::mul_scale(frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src,
                                                 const frame_type &scale, int n) const CXXPH_NOEXCEPT
{
    using namespace cxxporthelper::x86intrinsics;

    assert(utils::is_aligned(src, 4));
    assert(utils::is_aligned(dest, 4));
    assert(n >= 0);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
    if (!utils::is_aligned(src, 16)) {
        const uintptr_t src_ptr = reinterpret_cast<uintptr_t>(src);
        const int n1 = (std::min)((16 - static_cast<int>(src_ptr & 0x0f)) / 4, n);

        for (int i = 0; i < n1; ++i) {
            dest[i] = src[i] * scale;
        }

        src += n1;
        dest += n1;
        n -= n1;
    }

    const int n2 = (n >> 4) << 4;
    if (n2 > 0) {
        assert(utils::is_aligned(src, 16));

        if (utils::is_aligned(dest, 16)) {
            sse_f32_mono_mul_scale<sse_m128_load_aligned, sse_m128_store_aligned>(dest, src, scale, n2);
        } else {
            sse_f32_mono_mul_scale<sse_m128_load_aligned, sse_m128_store_unaligned>(dest, src, scale, n2);
        }

        src += n2;
        dest += n2;
        n -= n2;
    }
#endif

    for (int i = 0; i < n; ++i) {
        dest[i] = src[i] * scale;
    }
}

void f32_mono_sse_mixer_core_operator::mac_scale(frame_type *CXXPH_RESTRICT dest, const frame_type *CXXPH_RESTRICT src,
                                                 const frame_type &scale, int n) const CXXPH_NOEXCEPT
{
    using namespace cxxporthelper::x86intrinsics;

    assert(utils::is_aligned(src, 4));
    assert(utils::is_aligned(dest, 4));
    assert(n >= 0);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
    if (!utils::is_aligned(dest, 16)) {
        const uintptr_t dest_ptr = reinterpret_cast<uintptr_t>(dest);
        const int n1 = (std::min)((16 - static_cast<int>(dest_ptr & 0x0f)) / 4, n);

        for (int i = 0; i < n1; ++i) {
            dest[i] += src[i] * scale;
        }

        src += n1;
        dest += n1;
        n -= n1;
    }

    const int n2 = (n >> 4) << 4;
    if (n2 > 0) {
        assert(utils::is_aligned(dest, 16));

        if (utils::is_aligned(src, 16)) {
            sse_f32_mono_mac_scale<sse_m128_load_aligned, sse_m128_load_aligned, sse_m128_store_aligned>(dest, src,
                                                                                                         scale, n2);
        } else {
            sse_f32_mono_mac_scale<sse_m128_load_unaligned, sse_m128_load_aligned, sse_m128_store_aligned>(dest, src,
                                                                                                           scale, n2);
        }

        src += n2;
        dest += n2;
        n -= n2;
    }
#endif

    for (int i = 0; i < n; ++i) {
        dest[i] += src[i] * scale;
    }
}

void f32_mono_sse_mixer_core_operator::mul_forward_table_and_scale(frame_type *CXXPH_RESTRICT dest,
                                                                   const frame_type *CXXPH_RESTRICT src,
                                                                   const table_coeff_type *CXXPH_RESTRICT table,
                                                                   const frame_type &scale, int n) const CXXPH_NOEXCEPT
{
    using namespace cxxporthelper::x86intrinsics;

    assert(utils::is_aligned(src, 4));
    assert(utils::is_aligned(dest, 4));
    assert(n >= 0);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
    if (!utils::is_aligned(src, 16)) {
        const uintptr_t src_ptr = reinterpret_cast<uintptr_t>(src);
        const int n1 = (std::min)((16 - static_cast<int>(src_ptr & 0x0f)) / 4, n);

        for (int i = 0; i < n1; ++i) {
            dest[i] = src[i] * table[i] * scale;
        }

        src += n1;
        dest += n1;
        table += n1;
        n -= n1;
    }

    const int n2 = (n >> 4) << 4;
    if (n2 > 0) {
        assert(utils::is_aligned(src, 16));

        if (utils::is_aligned(dest, 16) && utils::is_aligned(table, 16)) {
            sse_f32_mono_mul_forward_table_and_scale<sse_m128_load_aligned, sse_m128_store_aligned,
                                                     sse_m128_load_aligned>(dest, src, table, scale, n2);
        } else if (utils::is_aligned(dest, 16) && !utils::is_aligned(table, 16)) {
            sse_f32_mono_mul_forward_table_and_scale<sse_m128_load_aligned, sse_m128_store_aligned,
                                                     sse_m128_load_unaligned>(dest, src, table, scale, n2);
        } else if (!utils::is_aligned(dest, 16) && utils::is_aligned(table, 16)) {
            sse_f32_mono_mul_forward_table_and_scale<sse_m128_load_aligned, sse_m128_store_unaligned,
                                                     sse_m128_load_aligned>(dest, src, table, scale, n2);
        } else {
            sse_f32_mono_mul_forward_table_and_scale<sse_m128_load_aligned, sse_m128_store_unaligned,
                                                     sse_m128_load_unaligned>(dest, src, table, scale, n2);
        }

        src += n2;
        dest += n2;
        table += n2;
        n -= n2;
    }
#endif

    for (int i = 0; i < n; ++i) {
        dest[i] = src[i] * table[i] * scale;
    }
}

void f32_mono_sse_mixer_core_operator::mac_forward_table_and_scale(frame_type *CXXPH_RESTRICT dest,
                                                                   const frame_type *CXXPH_RESTRICT src,
                                                                   const table_coeff_type *CXXPH_RESTRICT table,
                                                                   const frame_type &scale, int n) const CXXPH_NOEXCEPT
{
    using namespace cxxporthelper::x86intrinsics;

    assert(utils::is_aligned(src, 4));
    assert(utils::is_aligned(dest, 4));
    assert(n >= 0);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
    if (!utils::is_aligned(dest, 16)) {
        const uintptr_t dest_ptr = reinterpret_cast<uintptr_t>(dest);
        const int n1 = (std::min)((16 - static_cast<int>(dest_ptr & 0x0f)) / 4, n);

        for (int i = 0; i < n1; ++i) {
            dest[i] += src[i] * table[i] * scale;
        }

        src += n1;
        dest += n1;
        table += n1;
        n -= n1;
    }

    const int n2 = (n >> 4) << 4;
    if (n2 > 0) {
        assert(utils::is_aligned(dest, 16));

        if (utils::is_aligned(src, 16) && utils::is_aligned(table, 16)) {
            sse_f32_mono_mac_forward_table_and_scale<sse_m128_load_aligned, sse_m128_load_aligned,
                                                     sse_m128_store_aligned, sse_m128_load_aligned>(dest, src, table,
                                                                                                    scale, n2);
        } else if (utils::is_aligned(src, 16) && !utils::is_aligned(table, 16)) {
            sse_f32_mono_mac_forward_table_and_scale<sse_m128_load_aligned, sse_m128_load_aligned,
                                                     sse_m128_store_aligned, sse_m128_load_unaligned>(dest, src, table,
                                                                                                      scale, n2);
        } else if (!utils::is_aligned(src, 16) && utils::is_aligned(table, 16)) {
            sse_f32_mono_mac_forward_table_and_scale<sse_m128_load_unaligned, sse_m128_load_aligned,
                                                     sse_m128_store_aligned, sse_m128_load_aligned>(dest, src, table,
                                                                                                    scale, n2);
        } else {
            sse_f32_mono_mac_forward_table_and_scale<sse_m128_load_unaligned, sse_m128_load_aligned,
                                                     sse_m128_store_aligned, sse_m128_load_unaligned>(dest, src, table,
                                                                                                      scale, n2);
        }

        src += n2;
        dest += n2;
        table += n2;
        n -= n2;
    }
#endif

    for (int i = 0; i < n; ++i) {
        dest[i] += src[i] * table[i] * scale;
    }
}

void f32_mono_sse_mixer_core_operator::mul_backward_table_and_scale(frame_type *CXXPH_RESTRICT dest,
                                                                    const frame_type *CXXPH_RESTRICT src,
                                                                    const table_coeff_type *CXXPH_RESTRICT table,
                                                                    const frame_type &scale, int n) const CXXPH_NOEXCEPT
{
    using namespace cxxporthelper::x86intrinsics;

    assert(utils::is_aligned(src, 4));
    assert(utils::is_aligned(dest, 4));
    assert(n >= 0);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
    if (!utils::is_aligned(src, 16)) {
        const uintptr_t src_ptr = reinterpret_cast<uintptr_t>(src);
        const int n1 = (std::min)((16 - static_cast<int>(src_ptr & 0x0f)) / 4, n);

        for (int i = 0; i < n1; ++i) {
            dest[i] = src[i] * (*table) * scale;
            --table;
        }

        src += n1;
        dest += n1;
        // table -= n1;
        n -= n1;
    }

    const int n2 = (n >> 4) << 4;
    if (n2 > 0) {
        assert(utils::is_aligned(src, 16));

        if (utils::is_aligned(dest, 16) && utils::is_aligned((table - 7), 16)) {
            sse_f32_mono_mul_backward_table_and_scale<sse_m128_load_aligned, sse_m128_store_aligned,
                                                      sse_m128_load_aligned>(dest, src, table, scale, n2);
        } else if (utils::is_aligned(dest, 16) && !utils::is_aligned((table - 7), 16)) {
            sse_f32_mono_mul_backward_table_and_scale<sse_m128_load_aligned, sse_m128_store_aligned,
                                                      sse_m128_load_unaligned>(dest, src, table, scale, n2);
        } else if (!utils::is_aligned(dest, 16) && utils::is_aligned((table - 7), 16)) {
            sse_f32_mono_mul_backward_table_and_scale<sse_m128_load_aligned, sse_m128_store_unaligned,
                                                      sse_m128_load_aligned>(dest, src, table, scale, n2);
        } else {
            sse_f32_mono_mul_backward_table_and_scale<sse_m128_load_aligned, sse_m128_store_unaligned,
                                                      sse_m128_load_unaligned>(dest, src, table, scale, n2);
        }

        src += n2;
        dest += n2;
        table -= n2;
        n -= n2;
    }
#endif

    for (int i = 0; i < n; ++i) {
        dest[i] = src[i] * (*table) * scale;
        --table;
    }
}

void f32_mono_sse_mixer_core_operator::mac_backward_table_and_scale(frame_type *CXXPH_RESTRICT dest,
                                                                    const frame_type *CXXPH_RESTRICT src,
                                                                    const table_coeff_type *CXXPH_RESTRICT table,
                                                                    const frame_type &scale, int n) const CXXPH_NOEXCEPT
{
    using namespace cxxporthelper::x86intrinsics;

    assert(utils::is_aligned(src, 4));
    assert(utils::is_aligned(dest, 4));
    assert(n >= 0);

#if CXXPH_COMPILER_SUPPORTS_X86_SSE
    if (!utils::is_aligned(dest, 16)) {
        const uintptr_t dest_ptr = reinterpret_cast<uintptr_t>(dest);
        const int n1 = (std::min)((16 - static_cast<int>(dest_ptr & 0x0f)) / 4, n);

        for (int i = 0; i < n1; ++i) {
            dest[i] += src[i] * (*table) * scale;
            --table;
        }

        src += n1;
        dest += n1;
        // table -= n1;
        n -= n1;
    }

    const int n2 = (n >> 4) << 4;
    if (n2 > 0) {
        assert(utils::is_aligned(dest, 16));

        if (utils::is_aligned(src, 16) && utils::is_aligned((table - 7), 16)) {
            sse_f32_mono_mac_backward_table_and_scale<sse_m128_load_aligned, sse_m128_load_aligned,
                                                      sse_m128_store_aligned, sse_m128_load_aligned>(dest, src, table,
                                                                                                     scale, n2);
        } else if (utils::is_aligned(src, 16) && !utils::is_aligned((table - 7), 16)) {
            sse_f32_mono_mac_backward_table_and_scale<sse_m128_load_aligned, sse_m128_load_aligned,
                                                      sse_m128_store_aligned, sse_m128_load_unaligned>(dest, src, table,
                                                                                                       scale, n2);
        } else if (!utils::is_aligned(src, 16) && utils::is_aligned((table - 7), 16)) {
            sse_f32_mono_mac_backward_table_and_scale<sse_m128_load_unaligned, sse_m128_load_aligned,
                                                      sse_m128_store_aligned, sse_m128_load_aligned>(dest, src, table,
                                                                                                     scale, n2);
        } else {
            sse_f32_mono_mac_backward_table_and_scale<sse_m128_load_unaligned, sse_m128_load_aligned,
                                                      sse_m128_store_aligned, sse_m128_load_unaligned>(dest, src, table,
                                                                                                       scale, n2);
        }

        src += n2;
        dest += n2;
        table -= n2;
        n -= n2;
    }
#endif

    for (int i = 0; i < n; ++i) {
        dest[i] += src[i] * (*table) * scale;
        --table;
    }
}

} // namespace mixer
} // namespace cxxdasp

#endif // (CXXPH_TARGET_ARCH == CXXPH_ARCH_I386) || (CXXPH_TARGET_ARCH == CXXPH_ARCH_X86_64)
