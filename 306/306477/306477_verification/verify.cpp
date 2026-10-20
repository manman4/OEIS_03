#include <array>
#include <bit>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using U = std::uint64_t;
using Wide = __uint128_t;
constexpr U limit = 1000000000000000ULL;

U choose(U m, unsigned k) {
    if (m < k) return 0;
    Wide value = 1;
    for (unsigned i = 1; i <= k; ++i) {
        value = value * (m - i + 1) / i;
    }
    if (value > UINT64_MAX) throw std::overflow_error("binomial overflow");
    return static_cast<U>(value);
}

// One zero term, followed by the strictly increasing positive binomials.
std::vector<U> binomials(unsigned k, U n) {
    std::vector<U> values;
    for (U m = k - 1;; ++m) {
        const U value = choose(m, k);
        if (value > n) return values;
        values.push_back(value);
    }
}

// Integer Newton iteration, starting at an upper bound for sqrt(n).
U isqrt(U n) {
    if (n < 2) return n;
    U r = U{1} << ((std::bit_width(n) + 1) / 2);
    for (;;) {
        const U next = (r + n / r) / 2;
        if (next >= r) return r;
        r = next;
    }
}

struct Result {
    U count = 0;
    U triples = 0;
};

Result verify(U n, bool show_progress) {
    const auto c4 = binomials(4, n);
    const auto c6 = binomials(6, n);
    const auto c8 = binomials(8, n);
    // Every integer square has a square residue modulo 4095.
    std::array<bool, 4095> square_residue{};
    for (U r = 0; r < square_residue.size(); ++r) {
        square_residue[(r * r) % square_residue.size()] = true;
    }
    Result result;
    const auto started = std::chrono::steady_clock::now();
    for (std::size_t iz = 0; iz < c8.size(); ++iz) {
        const U rest8 = n - c8[iz];
        for (std::size_t iy = 0; iy < c6.size(); ++iy) {
            // The triangular summand must be at least 1.
            if (c6[iy] >= rest8) break;
            const U rest6 = rest8 - c6[iy];
            for (std::size_t ix = 0; ix < c4.size(); ++ix) {
                if (c4[ix] >= rest6) break;
                ++result.triples;
                const U remainder = rest6 - c4[ix];
                const U d = 8 * remainder + 1;
                if (!square_residue[d % square_residue.size()]) continue;
                const U root = isqrt(d);
                if (root * root != d) continue;
                const U w = (root + 1) / 2;
                if (choose(w, 2) != remainder) {
                    throw std::logic_error("triangular test failed");
                }
                ++result.count;
                std::cout << "representation " << n << " = C(" << w
                          << ",2) + C(" << ix + 3 << ",4) + C("
                          << iy + 5 << ",6) + C(" << iz + 7 << ",8)\n";
            }
        }
        if (show_progress && (iz % 25 == 0 || iz + 1 == c8.size())) {
            std::cerr << "n=" << n << " z=" << iz + 7 << "/"
                      << c8.size() + 6 << " triples=" << result.triples << '\n';
        }
    }
    const auto seconds = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - started).count();
    std::cout << "result n=" << n << " representations=" << result.count
              << " triples=" << result.triples << " seconds=" << seconds << '\n';
    return result;
}

int main(int argc, char** argv) {
    try {
        if (argc < 2) throw std::invalid_argument("usage: verify N [N ...]");
        for (int i = 1; i < argc; ++i) {
            std::size_t used = 0;
            const std::string argument = argv[i];
            const U n = std::stoull(argument, &used);
            if (used != argument.size() || n > limit || argument[0] == '-') {
                throw std::invalid_argument("N must be in 0..10^15");
            }
            verify(n, n > 1000000);
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}
