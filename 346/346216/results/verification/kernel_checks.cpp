#define main ulam_program_main
#include "346216_02.cpp"
#undef main
#include <random>
#include <set>

static void require(bool yes, const char* text) { if (!yes) throw std::runtime_error(text); }
static U mix(U x) {
    x ^= x >> 30; x *= 0xbf58476d1ce4e5b9ULL;
    x ^= x >> 27; x *= 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}
static void pattern_test(U limit, bool disk) {
    Options o; o.terms = 1; o.max_value = Prefix + (1ULL << Width) * Width + 1;
    o.dictionary_limit = limit; o.cache_mib = 1;
    o.disk = disk ? "patterns_" + std::to_string(limit) + ".bin" : "";
    Budget budget(16 * MiB); Membership m(o, budget);
    for (U x = 1; x <= o.max_value; ++x) {
        bool bit = x >= Prefix && ((((x - Prefix) / Width) & Mask) >> (x % Width) & 1U);
        m.put(x, bit);
    }
    for (U i = 0; i < (1ULL << Width); ++i) {
        U b = Prefix / Width + i;
        require(m.block(b) == i, "all-pattern block decode");
        for (U bit = 0; bit < Width; ++bit)
            require(m.get(b * Width + bit) == bool((i >> bit) & 1U), "all-pattern bit decode");
    }
    require(m.patterns() == (1ULL << Width), "all-pattern distinct count");
    require(m.escaped_blocks == (1ULL << Width) - limit, "all-pattern escape count");
    m.flush();
    std::cerr << "PASS all 262144 patterns, dictionary=" << limit << " disk=" << disk << '\n';
}
static void pair_test() {
    Options o; o.disk.clear(); o.terms = 2000; o.max_value = 30000;
    Control control(o);
    // All 2^12 possible membership sets. Compare each split of the descending
    // search to an independent nested-loop enumeration of unordered pairs.
    U cases = 0;
    for (U mask = 0; mask < (1U << 12); ++mask) {
        Budget b(8 * MiB); Membership m(o, b);
        for (U x = 1; x <= 12; ++x) m.put(x, (mask >> (x - 1)) & 1U);
        for (U u = 3; u <= 13; ++u) {
            int wanted = 0;
            for (U a = 1; a < u; ++a)
                for (U c = a + 1; c < u; ++c)
                    if (a + c == u && m.get(a) && m.get(c)) ++wanted;
            wanted = std::min(2, wanted);
            for (U split = 0; split < u; ++split) {
                int initial = 0;
                for (U a = 1; a < u; ++a)
                    for (U c = a + 1; c < u; ++c)
                        if (a + c == u && c > split && m.get(a) && m.get(c)) ++initial;
                if (initial >= 2) continue;
                U tests = 0;
                int actual = exhaustive(u, split, initial, m, control, tests);
                require(actual == wanted, "fallback split/diagonal count"); ++cases;
            }
        }
    }
    // Synthetic sparse sets straddle Prefix and the 18-bit block boundaries.
    for (U seed = 1; seed <= 120; ++seed) {
        U u = Prefix - 30 + seed;
        Options options = o; options.dictionary_limit = 1;
        Budget b(8 * MiB); Membership m(options, b);
        std::vector<U> values;
        for (U x = 1; x < u; ++x) {
            bool yes = mix(x + seed * 1000003) % 128 == 0;
            m.put(x, yes); if (yes) values.push_back(x);
        }
        int wanted = 0;
        for (U a : values) for (U c : values) if (a < c && a + c == u) ++wanted;
        U tests = 0;
        require(exhaustive(u, u - 1, 0, m, control, tests) == std::min(2, wanted), "compressed fallback count");
        ++cases;
    }
    std::cerr << "PASS fallback splits, diagonal exclusions, compression boundaries: " << cases << " cases\n";
}
static void treap_test() {
    Options o; o.disk.clear(); o.terms = 2000; o.max_value = 30000;
    Budget b(8 * MiB); Membership m(o, b);
    std::vector<U> all;
    for (U x = 1; x <= 2000; ++x) {
        bool yes = mix(x + 291827) % 5 == 0;
        m.put(x, yes); if (yes) all.push_back(x);
    }
    U checks = 0;
    for (U p : {U{22}, U{373}, U{1000000000}}) {
        U q = (p * 409 + 500) / 1000;
        for (U limit : {U{0}, U{1}, U{2}, U{4}, U{17}, U{257}, U{1000}})
            for (U t : {U{0}, U{1}, U{100}}) {
                Anchors lo(p, t, limit), hi(p, t, limit);
                std::vector<U> past;
                for (U a : all) {
                    U r = static_cast<U>(static_cast<W>(a) * q % p);
                    if (r <= p / 2) lo.add(r, a); else hi.add(p - r, a);
                    past.push_back(a); lo.audit(); hi.audit();
                    U expected_lo = 0, expected_hi = 0;
                    for (U v : past) {
                        U key = static_cast<U>(static_cast<W>(v) * q % p);
                        if (key <= p / 2 && key < lo.cutoff) ++expected_lo;
                        if (key > p / 2 && p - key < hi.cutoff) ++expected_hi;
                    }
                    require(lo.size() == expected_lo && hi.size() == expected_hi, "complete retained prefix");
                    U u = a + 1;
                    U target = static_cast<U>(static_cast<W>(u) * q % p);
                    if (target <= p / 4 || p - target <= p / 4) continue;
                    int left = 0, right = 0;
                    // Numeric order, independently determine the smaller residue
                    // endpoint for each unordered pair in the partial search.
                    for (U x : past) for (U y : past) if (x < y && x + y == u) {
                        U rx = static_cast<U>(static_cast<W>(x) * q % p);
                        U ry = static_cast<U>(static_cast<W>(y) * q % p);
                        if (rx + ry == target && std::min(rx, ry) < lo.cutoff) ++left;
                        if (rx + ry == target + p && std::min(p - rx, p - ry) < hi.cutoff) ++right;
                    }
                    require(lo.search(target, u, m, 0) == std::min(2, left), "low-residue pair count");
                    require(hi.search(p - target, u, m, 0) == std::min(2, right), "high-residue pair count");
                    ++checks;
                }
            }
    }
    std::cerr << "PASS treap rotations, trim/reuse, complete prefix and independent partial counts: " << checks << " cases\n";
}
static void cache_test() {
    Budget b(8 * MiB);
    U size = 96 * PageSize;
    Bytes bytes(1, size, "random_cache.bin", 1, b);
    std::vector<std::uint8_t> expected(static_cast<std::size_t>(size));
    std::mt19937_64 rng(9918327);
    for (U step = 0; step < 3000; ++step) {
        U address = rng() % size;
        auto value = static_cast<std::uint8_t>(rng());
        bytes.set(address, value); expected[address] = value;
        for (unsigned k = 0; k < 3; ++k) {
            U probe = rng() % (address + 1);
            require(bytes.get(probe) == expected[probe], "random cache read");
        }
        require(bytes.get(address) == value, "hot cache pointer");
    }
    // Extend to the maximum and verify bytes on both sides of each page edge.
    bytes.set(size - 1, 93); expected[size - 1] = 93;
    for (U address = 0; address < size; ++address)
        require(bytes.get(address) == expected[address], "full random cache round trip");
    bytes.flush();
    std::cerr << "PASS cache growth, slot reuse, dirty eviction, hot pointer and page edges\n";
}
int main() {
    try {
        pattern_test(1, false); pattern_test(255, false);
        pattern_test(1, true); pattern_test(255, true);
        pair_test(); treap_test(); cache_test();
        return 0;
    } catch (const std::exception& e) { std::cerr << "FAIL " << e.what() << '\n'; return 1; }
}
