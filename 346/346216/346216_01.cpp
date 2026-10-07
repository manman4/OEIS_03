// Exact, bounded feasibility probe for U(10^10).
// Algorithmic ideas: Philip Gibbs (2015, 2017), Donald Knuth (2016).
// This is a new implementation, not a translation of their source code.
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <sys/resource.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

using U = std::uint64_t;
using Wide = __uint128_t;
static constexpr U MiB = 1024 * 1024;
static constexpr U Block = 65536;
static constexpr U Prefix = 18000;
static constexpr U Width = 18;
static constexpr U P = 856371966;
static constexpr U Q = 350477575;

static U number(const std::string& s) {
    if (s.empty() || s.find_first_not_of("0123456789") != std::string::npos)
        throw std::runtime_error("invalid unsigned integer: " + s);
    std::size_t used = 0;
    U v = std::stoull(s, &used);
    if (used != s.size()) throw std::runtime_error("invalid integer");
    return v;
}

struct Options {
    U terms = 1000000, max_value = 0, memory_mib = 256, cache_mib = 8;
    U verify_until = 0, seconds = 60, window = 100000, threshold = 100;
    U report = 1000000, p = P, q = Q;
    std::string disk;
    bool self_test = false;
};

// RAM or an explicit, bounded, FIFO block cache. Only new bytes are written.
// Disk files must be new; existing files cannot be overwritten.
class Bytes {
    struct Page { std::vector<std::uint8_t> data; bool dirty = false; };
    std::vector<std::uint8_t> ram;
    std::unordered_map<U, Page> pages;
    std::vector<U> fifo;
    std::size_t hand = 0, capacity = 0;
    int fd = -1;
    U length;
    void write_page(U key, Page& page) {
        if (!page.dirty) return;
        std::size_t done = 0;
        while (done < Block) {
            ssize_t n = pwrite(fd, page.data.data() + done, Block - done,
                              static_cast<off_t>(key * Block + done));
            if (n < 0 && errno == EINTR) continue;
            if (n <= 0) throw std::runtime_error("pwrite failed");
            done += static_cast<std::size_t>(n);
        }
        page.dirty = false;
        ++writes;
    }
    Page& load(U key) {
        auto it = pages.find(key);
        if (it != pages.end()) return it->second;
        if (pages.size() == capacity) {
            U victim = fifo[hand];
            auto found = pages.find(victim);
            write_page(victim, found->second);
            pages.erase(found);
            fifo[hand] = key;
            hand = (hand + 1) % capacity;
        } else fifo.push_back(key);
        Page page{std::vector<std::uint8_t>(Block, 0), false};
        std::size_t done = 0;
        while (done < Block) {
            ssize_t n = pread(fd, page.data.data() + done, Block - done,
                             static_cast<off_t>(key * Block + done));
            if (n < 0 && errno == EINTR) continue;
            if (n < 0) throw std::runtime_error("pread failed");
            if (!n) break;
            done += static_cast<std::size_t>(n);
        }
        ++reads;
        return pages.emplace(key, std::move(page)).first->second;
    }
public:
    U reads = 0, writes = 0;
    Bytes(U size, const Options& o) : length(size) {
        if (o.disk.empty()) {
            if (size > o.memory_mib * MiB)
                throw std::runtime_error("RAM table exceeds --memory-mib; use smaller range or --disk");
            ram.resize(static_cast<std::size_t>(size));
        } else {
            capacity = static_cast<std::size_t>(o.cache_mib * MiB / Block);
            if (!capacity) throw std::runtime_error("disk cache too small");
            fd = open(o.disk.c_str(), O_CREAT | O_EXCL | O_RDWR, 0600);
            if (fd < 0) throw std::runtime_error("cannot create new disk file: " + o.disk);
            U rounded = (size + Block - 1) / Block * Block;
            if (ftruncate(fd, static_cast<off_t>(rounded))) {
                close(fd); fd = -1;
                throw std::runtime_error("ftruncate failed");
            }
            pages.reserve(capacity);
            fifo.reserve(capacity);
        }
    }
    ~Bytes() { if (fd >= 0) close(fd); }
    std::uint8_t get(U x) {
        if (x >= length) throw std::runtime_error("byte read out of bounds");
        if (fd < 0) return ram[static_cast<std::size_t>(x)];
        return load(x / Block).data[static_cast<std::size_t>(x % Block)];
    }
    void set(U x, std::uint8_t v) {
        if (x >= length) throw std::runtime_error("byte write out of bounds");
        if (fd < 0) { ram[static_cast<std::size_t>(x)] = v; return; }
        auto& page = load(x / Block);
        page.data[static_cast<std::size_t>(x % Block)] = v;
        page.dirty = true;
    }
    void flush() {
        if (fd < 0) return;
        for (auto& entry : pages) write_page(entry.first, entry.second);
        if (fsync(fd)) throw std::runtime_error("fsync failed");
    }
};

// A completed block has one byte of storage and an exact 18-bit dictionary
// entry. A 257th pattern aborts instead of discarding or approximating bits.
class Membership {
    Bytes storage;
    std::vector<std::uint8_t> prefix = std::vector<std::uint8_t>(Prefix, 0);
    std::vector<std::uint16_t> inverse = std::vector<std::uint16_t>(1U << Width, 65535);
    std::vector<std::uint32_t> codes{0};
    U current = Prefix, seen = 0;
    std::uint32_t bits = 0;
    void finish_block() {
        auto& code = inverse[bits];
        if (code == 65535) {
            if (codes.size() == 256) throw std::runtime_error("257th compression pattern; stopped exactly");
            code = static_cast<std::uint16_t>(codes.size());
            codes.push_back(bits);
        }
        storage.set(current / Width, static_cast<std::uint8_t>(code));
        current += Width;
        bits = 0;
    }
public:
    Membership(const Options& o) : storage(o.max_value / Width + 1, o) { inverse[0] = 0; }
    bool get(U x) {
        if (x > seen) throw std::runtime_error("membership queried beyond completed prefix");
        if (x < Prefix) return prefix[static_cast<std::size_t>(x)] != 0;
        if (x >= current) return (bits >> (x - current)) & 1U;
        return (codes[storage.get(x / Width)] >> (x % Width)) & 1U;
    }
    void put(U x, bool yes) {
        if (x != seen + 1) throw std::runtime_error("nonsequential membership write");
        if (x < Prefix) prefix[static_cast<std::size_t>(x)] = static_cast<std::uint8_t>(yes);
        else {
            if (x == current + Width) finish_block();
            if (yes) bits |= 1U << (x - current);
        }
        seen = x;
    }
    void flush() { storage.flush(); }
    U pattern_count() const { return codes.size(); }
    U reads() const { return storage.reads; }
    U writes() const { return storage.writes; }
};

class Anchors {
    using Key = std::pair<U, U>; // exact residue key, then value
    std::map<Key, bool> values;
    U cutoff, p, threshold;
public:
    U examined = 0, peak = 0, omitted = 0;
    Anchors(U prime, U t) : cutoff(prime / 2 + 1), p(prime), threshold(t) {}
    void add(U key, U u) {
        if (key >= cutoff) { ++omitted; return; }
        U delta = 3 * key > p ? 3 * key - p : 0;
        // These products are bounded below 2^128 by CLI limits.
        Wide right = static_cast<Wide>(3 * p) * threshold;
        bool retain = !delta || static_cast<Wide>(delta) * delta * u < right * right;
        if (!retain) {
            cutoff = key;
            values.erase(values.lower_bound({key, 0}), values.end());
            ++omitted;
            return;
        }
        if (values.size() >= 500000) throw std::runtime_error("anchor memory cap reached");
        values.emplace(Key{key, u}, true);
        peak = std::max(peak, static_cast<U>(values.size()));
    }
    // Count distinct unordered pairs for one of the two residue orientations.
    // A search that could need a forgotten anchor stops with an error.
    int search(U target_key, U u, Membership& members, int hits) {
        U bound = target_key / 2;
        for (const auto& entry : values) {
            U key = entry.first.first, a = entry.first.second;
            if (key > bound) break;
            if (2 * key == target_key && a >= u - a) continue;
            ++examined;
            if (members.get(u - a) && ++hits == 2) return 2;
        }
        if (bound >= cutoff)
            throw std::runtime_error("search reached omitted anchors; increase threshold, then restart");
        return hits;
    }
    U size() const { return values.size(); }
    U bound() const { return cutoff; }
};

// Independent oracle: saturating counts of all distinct pair sums.
class Oracle {
    U limit;
    std::vector<std::uint8_t> sums;
    std::vector<U> terms;
public:
    explicit Oracle(U n) : limit(n), sums(n ? static_cast<std::size_t>(n + 1) : 0, 0) {}
    void add(U u) {
        if (!limit || u > limit) return;
        for (U a : terms) {
            if (a > limit - u) break;
            auto& count = sums[static_cast<std::size_t>(a + u)];
            if (count < 2) ++count;
        }
        terms.push_back(u);
    }
    void check(U u, int hits) const {
        if (limit && u <= limit && sums[static_cast<std::size_t>(u)] != hits)
            throw std::runtime_error("independent oracle disagreement at candidate " + std::to_string(u));
    }
};

static void self_test(const Options& options) {
    // Independent wide arithmetic checks of the residue recurrence, including
    // both sides of 32-bit boundaries and the intended production value range.
    for (U p : {U{22}, P, U{120500181}}) {
        U q = p == 22 ? 9 : (p == P ? Q : 49315733);
        for (U base : {U{0}, U{4294967290ULL}, U{135180000000ULL}, U{999999999990ULL}}) {
            U r = static_cast<U>(static_cast<Wide>(base) * q % p);
            for (U x = base; x < base + 100; ++x) {
                if (r != static_cast<U>(static_cast<Wide>(x) * q % p))
                    throw std::runtime_error("residue boundary test failed");
                r += q; if (r >= p) r -= p;
            }
        }
    }
    Options o; o.max_value = 22000;
    Membership m(o);
    for (U x = 1; x <= o.max_value; ++x) {
        bool yes = x % 9 == 1 || x % 9 == 4;
        m.put(x, yes);
        for (U back : {U{0}, U{1}, U{17}, U{18}, U{19}, U{18000}})
            if (x >= back) {
                U y = x - back;
                bool expected = y && (y % 9 == 1 || y % 9 == 4);
                if (m.get(y) != expected) throw std::runtime_error("compression round trip failed");
            }
    }
    // Deliberately require 257 patterns; the codec must refuse the new one.
    o.max_value = Prefix + 300 * Width;
    Membership overflow(o);
    bool caught = false;
    try {
        for (U x = 1; x <= o.max_value; ++x) {
            U block = x >= Prefix ? (x - Prefix) / Width : 0;
            bool yes = x >= Prefix && ((block >> ((x - Prefix) % Width)) & 1U);
            overflow.put(x, yes);
        }
    } catch (const std::runtime_error& e) {
        caught = std::string(e.what()).find("257th") != std::string::npos;
    }
    if (!caught) throw std::runtime_error("compression overflow was not rejected");
    if (!options.disk.empty()) {
        Options disk_options;
        disk_options.disk = options.disk + ".address-test";
        disk_options.cache_mib = 1;
        // Sparse file: exercises file/table addresses beyond 2^32, without
        // allocating gigabytes of RAM or writing gigabytes to the disk.
        U base = 1ULL << 32;
        Bytes data(base + 40 * Block, disk_options);
        for (U i = 0; i < 32; ++i)
            data.set(base + i * Block + 123, static_cast<std::uint8_t>(i + 1));
        for (U i = 0; i < 32; ++i)
            if (data.get(base + i * Block + 123) != i + 1)
                throw std::runtime_error("64-bit disk address or cache eviction test failed");
        data.flush();
        std::cerr << "storage_test=passed (64-bit addresses, dirty eviction, reload)\n";
    }
    std::cerr << "self_test=passed (residues, compression boundaries, dictionary overflow)\n";
}

int main(int argc, char** argv) {
    U n = 0, last = 0;
    try {
        Options o;
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--self-test") { o.self_test = true; continue; }
            if (i + 1 == argc) throw std::runtime_error("missing option value");
            std::string s = argv[++i];
            if (arg == "--disk") { o.disk = s; continue; }
            U v = number(s);
            if (arg == "--terms") o.terms = v;
            else if (arg == "--max-value") o.max_value = v;
            else if (arg == "--memory-mib") o.memory_mib = v;
            else if (arg == "--cache-mib") o.cache_mib = v;
            else if (arg == "--verify-until") o.verify_until = v;
            else if (arg == "--seconds") o.seconds = v;
            else if (arg == "--window") o.window = v;
            else if (arg == "--threshold") o.threshold = v;
            else if (arg == "--report") o.report = v;
            else if (arg == "--p") o.p = v;
            else if (arg == "--q") o.q = v;
            else throw std::runtime_error("unknown option: " + arg);
        }
        if (o.terms < 2 || o.terms > 100000000000ULL || o.memory_mib > 1024 ||
            !o.memory_mib || !o.cache_mib || o.cache_mib > 256 ||
            o.verify_until > 200000 || o.seconds > 86400 || !o.seconds ||
            o.window < 3 || o.window > 1000000 || o.threshold > 1000000 ||
            o.p > 1000000000 || !o.q || o.q > o.p / 2 || o.p > 3 * o.q)
            throw std::runtime_error("option outside bounded probe limits");
        if (!o.max_value) o.max_value = 14 * o.terms;
        if (o.max_value < 3 || o.max_value > 2000000000000ULL)
            throw std::runtime_error("max-value outside probe limits");
        if (o.self_test) self_test(o);
        Membership members(o);
        Oracle oracle(o.verify_until);
        Anchors lo(o.p, o.threshold), hi(o.p, o.threshold);
        std::vector<U> window(static_cast<std::size_t>(o.window), 0);
        U cursor = 0, r = 0, hash = 14695981039346656037ULL;
        auto remember = [&](U x) {
            ++n; last = x;
            window[static_cast<std::size_t>(cursor)] = x;
            cursor = (cursor + 1) % o.window;
            if (r <= o.p / 2) lo.add(r, x); else hi.add(o.p - r, x);
            oracle.add(x);
            hash = (hash ^ x) * 1099511628211ULL;
        };
        for (U x = 1; x <= 2; ++x) {
            r += o.q; if (r >= o.p) r -= o.p;
            members.put(x, true); remember(x);
        }
        auto start = std::chrono::steady_clock::now();
        U brute_tests = 0, brute_max = 0, next_power = 10, completed_candidate = 2;
        std::cout << "index,value\n";
        for (U u = 3; n < o.terms; ++u) {
            if (u > o.max_value) throw std::runtime_error("max-value reached before requested term");
            r += o.q; if (r >= o.p) r -= o.p;
            int hits = 0;
            if (r <= o.p / 4 || o.p - r <= o.p / 4) {
                U count = 0, available = std::min(n, o.window), pos = cursor;
                while (count < available) {
                    pos = pos ? pos - 1 : o.window - 1;
                    U a = window[static_cast<std::size_t>(pos)];
                    if (a <= u / 2) break;
                    ++count; ++brute_tests;
                    if (members.get(u - a) && ++hits == 2) break;
                }
                brute_max = std::max(brute_max, count);
                if (hits < 2 && count == available && n > available)
                    throw std::runtime_error("recent window exhausted; stopped exactly");
            } else {
                hits = lo.search(r, u, members, hits);
                if (hits < 2) hits = hi.search(o.p - r, u, members, hits);
            }
            oracle.check(u, hits);
            members.put(u, hits == 1);
            completed_candidate = u;
            if (hits == 1) {
                remember(u);
                if (n == next_power) {
                    std::cout << n << ',' << u << '\n';
                    next_power *= 10;
                }
                if (o.report && n % o.report == 0) {
                    double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
                    U percent100 = static_cast<U>(static_cast<Wide>(n) * 10000 / o.terms);
                    double eta = elapsed * static_cast<double>(o.terms - n) / static_cast<double>(n - 2);
                    std::cerr << "progress terms=" << n << " value=" << u << " seconds=" << elapsed
                              << " target=" << o.terms << " percent=" << percent100 / 100 << '.'
                              << (percent100 % 100 < 10 ? "0" : "") << percent100 % 100
                              << " eta_seconds=" << eta << '\n';
                }
            }
            if ((u & 65535U) == 0 &&
                std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count() >= static_cast<double>(o.seconds))
                throw std::runtime_error("time cap reached; partial computation only");
        }
        members.flush();
        double elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
        rusage usage{}; getrusage(RUSAGE_SELF, &usage);
        U rss = static_cast<U>(usage.ru_maxrss);
#ifndef __APPLE__
        rss *= 1024;
#endif
        if (n != next_power / 10) std::cout << n << ',' << last << '\n';
        std::cout.flush();
        std::cerr << "{\"status\":\"complete\",\"terms\":" << n << ",\"value\":" << last
                  << ",\"completed_candidate\":" << completed_candidate << ",\"seconds\":" << elapsed
                  << ",\"rss_bytes\":" << rss << ",\"table_capacity_bytes\":" << o.max_value / Width + 1
                  << ",\"patterns\":" << members.pattern_count() << ",\"digest64\":" << hash
                  << ",\"anchor_tests\":" << lo.examined + hi.examined << ",\"brute_tests\":" << brute_tests
                  << ",\"brute_max\":" << brute_max << ",\"lo_size\":" << lo.size() << ",\"hi_size\":" << hi.size()
                  << ",\"lo_peak\":" << lo.peak << ",\"hi_peak\":" << hi.peak
                  << ",\"lo_cutoff\":" << lo.bound() << ",\"hi_cutoff\":" << hi.bound()
                  << ",\"disk_reads\":" << members.reads() << ",\"disk_writes\":" << members.writes()
                  << ",\"verified_candidates_through\":" << std::min(o.verify_until, completed_candidate) << "}\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "status=incomplete terms=" << n << " last=" << last << " reason=" << e.what() << '\n';
        return 1;
    }
}
