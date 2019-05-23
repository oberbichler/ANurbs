#pragma once

#include "../Define.h"

#include <cmath>
#include <vector>

namespace ANurbs::Math {

constexpr inline int binom(const int n, const int k) noexcept
{
    // clang-format off
    return (k > n               ) ? 0           :  // out of range
           (k == 0 || k == n    ) ? 1           :  // edge
           (k == 1 || k == n - 1) ? n           :  // first
           (k + k < n           ) ?                // recursive:
           (binom(n - 1, k - 1  ) * n) / k      :  //   path to k = 1     faster
           (binom(n - 1, k      ) * n) / (n - k);  //   path to k = n - 1 faster
    // clang-format on
}

constexpr inline int single_index(const int rows, const int cols, const int row,
    const int col) noexcept
{
    return row * cols + col;
}

template <typename TFunction>
double romberg(TFunction f, double a, double b, int max_iter, double tolerance)
{
    std::vector<double> r1(max_iter);
    std::vector<double> r2(max_iter);

    double* Rp = &r1[0]; // current row
    double* Rc = &r2[0]; // previous row

    double h = (b - a); // step size

    Rp[0] = (f(a) + f(b)) * h * 0.5; // first trapezoidal step

    for (int i = 1; i < max_iter; ++i) {
        h /= 2.0;

        double c = 0;

        int ep = 1 << (i - 1); // 2^(n-1)

        for (int j = 1; j <= ep; ++j) {
            c += f(a + (2 * j - 1) * h);
        }

        Rc[0] = h * c + 0.5 * Rp[0]; // R(i,0)

        for (int j = 1; j <= i; ++j) {
            double n_k = std::pow(4, j);
            Rc[j] = (n_k * Rc[j - 1] - Rp[j - 1]) / (n_k - 1); // compute R(i,j)
        }

        if (i > 1 && fabs(Rp[i - 1] - Rc[i]) < tolerance) {
            return Rc[i - 1];
        }

        // swap Rn and Rc as we only need the last row
        double* rt = Rp;

        Rp = Rc;
        Rc = rt;
    }

    return Rp[max_iter - 1];
}

} // namespace ANurbs::Math