#include <cxxdasp/window/window_functions.hpp>

#include <algorithm>
#include <cassert>

#include <cxxporthelper/cmath>
#include <cxxporthelper/compiler.hpp>
#include <cxxdasp/utils/fast_sincos_generator.hpp>

#if CXXPH_COMPILER_SUPPORTS_CONSTEXPR
#define COMPILE_TIME_CONST constexpr
#else
#define COMPILE_TIME_CONST const
#endif

namespace cxxdasp {
namespace window {

static void mirror_copy(float *CXXPH_RESTRICT dest, const float *CXXPH_RESTRICT src, size_t n) CXXPH_NOEXCEPT
{
    for (int i = 0; i < n; ++i) {
        (*dest) = (*src);
        --src;
        ++dest;
    }
}

static void generate_generalized_hamming_window(float *dest, size_t n, double alpha) CXXPH_NOEXCEPT
{
    const double beta = 1.0f - alpha;
    const double phase_step = (n > 1) ? ((2 * M_PI) / (n - 1)) : 0.0;
    utils::fast_sincos_generator<double> gen(0.0, phase_step);

    const size_t n1 = ((n + 1) / 2);
    const size_t n2 = (n / 2);

    for (size_t i = 0; i < n1; ++i) {
        dest[i] = static_cast<float>(alpha - beta * gen.c());
        gen.update();
    }

    if (n1 < n) {
        mirror_copy(&dest[n1], &dest[n2 - 1], n2);
    }
}

//
// Rectangular
//
void generate_rectangular_window(float *dest, size_t n) CXXPH_NOEXCEPT
{
    assert(dest);
    assert(n > 0);

    for (size_t i = 0; i < n; ++i) {
        dest[i] = 1.0f;
    }
}

//
// Hann
//
void generate_hann_window(float *dest, size_t n) CXXPH_NOEXCEPT
{
    const double alpha = 0.5;

    assert(dest);
    assert(n > 0);

    if (!dest) {
        return;
    }
    if (n == 0) {
        return;
    }

    generate_generalized_hamming_window(dest, n, alpha);
}

//
// Hamming
//
void generate_hamming_window(float *dest, size_t n) CXXPH_NOEXCEPT
{
    const double alpha = 0.54;

    assert(dest);
    assert(n > 0);

    if (!dest) {
        return;
    }
    if (n == 0) {
        return;
    }

    generate_generalized_hamming_window(dest, n, alpha);
}

//
// Blackman
//
void generate_blackman_window(float *dest, size_t n, double alpha) CXXPH_NOEXCEPT
{
    const double a0 = (1.0 - alpha) * 0.5;
    const double a1 = 0.5;
    const double a2 = alpha * 0.5;

    assert(dest);
    assert(n > 0);

    if (!dest) {
        return;
    }
    if (n == 0) {
        return;
    }

    const double phase_step = (n > 1) ? ((2 * M_PI) / (n - 1)) : 0.0;

    utils::fast_sincos_generator<double> gen1(0.0, phase_step * 1);
    utils::fast_sincos_generator<double> gen2(0.0, phase_step * 2);

    const size_t n1 = ((n + 1) / 2);
    const size_t n2 = (n / 2);

    for (size_t i = 0; i < n1; ++i) {
        double t = 0.0;
        t += a0;

        t -= a1 * gen1.c();
        gen1.update();

        t += a2 * gen2.c();
        gen2.update();

        dest[i] = t;
    }

    if (n1 < n) {
        mirror_copy(&dest[n1], &dest[n2 - 1], n2);
    }
}

//
// Flat Top
//
void generate_flat_top_window(float *dest, size_t n) CXXPH_NOEXCEPT
{
    COMPILE_TIME_CONST double a0 = 1.0;
    COMPILE_TIME_CONST double a1 = 1.93;
    COMPILE_TIME_CONST double a2 = 1.29;
    COMPILE_TIME_CONST double a3 = 0.338;
    COMPILE_TIME_CONST double a4 = 0.028;
    COMPILE_TIME_CONST double peak_level = 4.585999999999999;
    /*a0  - a1 * cos(2 * M_PI * 0.5) + a2 * cos(4 * M_PI * 0.5)
    - a3 * cos(6 * M_PI * 0.5) + a4 * cos(8 * M_PI * 0.5)*/;
    COMPILE_TIME_CONST double noramlize_coeff = 1.0 / peak_level;

    assert(dest);
    assert(n > 0);

    if (!dest) {
        return;
    }
    if (n == 0) {
        return;
    }

    const double phase_step = (n > 1) ? ((2 * M_PI) / (n - 1)) : 0.0;
    utils::fast_sincos_generator<double> gen1(0.0, phase_step * 1);
    utils::fast_sincos_generator<double> gen2(0.0, phase_step * 2);
    utils::fast_sincos_generator<double> gen3(0.0, phase_step * 3);
    utils::fast_sincos_generator<double> gen4(0.0, phase_step * 4);

    const size_t n1 = ((n + 1) / 2);
    const size_t n2 = (n / 2);

    for (size_t i = 0; i < n1; ++i) {
        double t = 0.0;
        t += a0;

        t -= a1 * gen1.c();
        gen1.update();

        t += a2 * gen2.c();
        gen2.update();

        t -= a3 * gen3.c();
        gen3.update();

        t += a4 * gen4.c();
        gen4.update();

        dest[i] = t * noramlize_coeff;
    }

    if (n1 < n) {
        mirror_copy(&dest[n1], &dest[n2 - 1], n2);
    }
}

} // namespace window
} // namespace cxxdasp
