#!/usr/bin/env ruby
# frozen_string_literal: true
#
# A399213
#   a(n) is the smallest nonnegative integer k where exactly n pairs of
#   positive integers (x, y) exist such that x^2 + 8*y^2 = k.
#
# ------------------------------------------------------------------
# 理論（判別式 -32。x^2+2y^2 への帰着で処理する）
# ------------------------------------------------------------------
#   判別式 -32 は Z[sqrt(-2)] の中の指数 2 の非最大整環に対応するが、
#   類群を直接扱わなくても、初等的に x^2+2y^2 (判別式 -8、PID) に
#   帰着できる。R_8, R_2 をそれぞれ正整数解の個数として
#
#     (a) k が奇数のとき x^2+8y^2=k なら x は奇数なので k ≡ 1 (mod 8)。
#         逆に k ≡ 1 (mod 8) で X^2+2Y^2=k なら、Y が奇数だと
#         X^2+2Y^2 ≡ 3 (mod 8) となるので Y は偶数。Y=2y とおけば
#         X^2+8y^2=k。よって
#             k 奇数のとき  R_8(k) = R_2(k) [k ≡ 1 mod 8]、それ以外 0
#     (b) k が偶数のとき x は偶数なので 4 | k。x=2X, k=4u とおけば
#             R_8(4u) = R_2(u)
#         （k ≡ 2 (mod 4) なら解なし）
#
#   そこで
#     B(n) := R_2(k) = n となる最小の正整数 k          (= A200977(n))
#     C(n) := R_2(k) = n となる最小の奇数 k ≡ 1 (mod 8)
#   とおくと、n >= 1 について
#       a(n) = min( C(n), 4*B(n) )
#   （k = 9^n は R_8 = n を与えるので C(n) は常に存在する）
#
# ------------------------------------------------------------------
# x^2+2y^2 の表現数
# ------------------------------------------------------------------
#   D(u) := Π(e_i + 1)   （p_i ≡ 1, 3 mod 8 の指数について）
#           ただし q ≡ 5, 7 mod 8 が奇数乗で現れれば D(u) = 0
#   E(u) := [u が平方数または2倍の平方数]
#   とおくと  R_2(u) = (D(u) - E(u)) / 2  であり、R_2(u) = n の候補は
#       非退化: D(u) = 2n   （u は平方でも2倍平方でもない）
#       退化  : D(u) = 2n+1 （u は平方または2倍平方）
#   の2種類だけ。2 の冪や q^(偶数) は D を変えず値を大きくするだけなので、
#   分解素数 3, 11, 17, 19, 41, 43, ... だけを考えればよい。
#
# ------------------------------------------------------------------
# 合同条件 k ≡ 1 (mod 8) の扱い
# ------------------------------------------------------------------
#   分解素数を
#       Q 型: p ≡ 3 (mod 8)   3, 11, 19, 43, 59, 67, ...
#       P 型: p ≡ 1 (mod 8)   17, 41, 73, 89, 97, 113, ...
#   に分けると、奇数 N について
#       N ≡ 1 (mod 8)  <=>  Σ_{p が Q 型} e_p が偶数
#   （q^2 ≡ 1 (mod 8) なので惰性素数の偶数乗も合同条件に影響しない）
#
#   D が奇数のときは全指数が偶数なので Q 型指数和も偶数、つまり合同条件は
#   自動的に成立する。したがって
#       C(n) = min( minprod_cong(2n), minprod(2n+1) )
#
#   探索の枝刈りについて:
#     制約なしの minprod では「小さい素数ほど大きい指数」としてよいが、
#     合同条件つきでは型をまたぐ指数の交換が Q 型指数和の偶奇を壊すので、
#     全体での非増加は仮定できない。一方、同じ型の 2 素数の間で指数を
#     交換しても D も偶奇も変わらないので、
#         「型ごとに、小さい素数ほど大きい指数」
#     としてよい。また同じ型の小さい素数を飛ばす理由もないので、
#     使われる素数は各型の先頭からの連続部分になる。minprod_cong は
#     この 2 つを枝刈りに使っている。
#
# ------------------------------------------------------------------
# 系（別文書で証明）
#   9 | a(n)、ただし a(2) = 33 のみ例外。
#   n >= 3 では非退化候補が補題により 9 の倍数、退化候補は 3 の指数が
#   正の偶数なのでやはり 9 の倍数になる。
# ------------------------------------------------------------------
#
# 使い方:
#   ruby 399213_01.rb           # n = 0..100 を出力
#   ruby 399213_01.rb 30        # n = 0..30 を出力
#   ruby 399213_01.rb --known   # 総当たりで確定済みの項と照合
#   ruby 399213_01.rb --verify  # 総当たりと突き合わせて検証

module A399213
  module_function

  # ---------- 素数まわり（prime gem に依存しない） ----------

  def prime?(n)
    return false if n < 2
    return true  if n < 4
    return false if n.even?

    d = 3
    while d * d <= n
      return false if (n % d).zero?
      d += 2
    end
    true
  end

  # 剰余 res (mod 8) の奇素数を小さい順に count 個
  def primes_mod8(res, count)
    ps = []
    n = 3
    while ps.size < count
      ps << n if n % 8 == res && prime?(n)
      n += 2
    end
    ps
  end

  # 分解素数 p ≡ 1, 3 (mod 8) を小さい順に count 個
  def split_primes(count)
    (primes_mod8(1, count) + primes_mod8(3, count)).sort.first(count)
  end

  def divisors_from(m, lower)
    ds = []
    d = 1
    while d * d <= m
      if (m % d).zero?
        ds << d if d >= lower
        e = m / d
        ds << e if e != d && e >= lower
      end
      d += 1
    end
    ds.sort
  end

  # ---------- 制約なしの最小値（= A200977 の材料） ----------

  # Π(e_i + 1) = m となる最小の Π p_i^{e_i}（p_i ≡ 1, 3 mod 8）
  def minprod(m, primes = nil, idx = 0, max_exp = Float::INFINITY, memo = {})
    return 1 if m == 1

    primes ||= split_primes(Math.log2(m).floor + 1)
    key = [m, idx, max_exp]
    cached = memo[key]
    return cached if cached
    return nil if idx >= primes.size

    best = nil
    divisors_from(m, 2).each { |d|
      e = d - 1
      next if e > max_exp

      rest = minprod(m / d, primes, idx + 1, e, memo)
      next if rest.nil?

      cand = primes[idx]**e * rest
      best = cand if best.nil? || cand < best
    }
    memo[key] = best
  end

  # ---------- 合同条件つきの最小値 ----------

  # Π(e_i + 1) = m かつ Σ_{Q 型} e_i が偶数（すなわち N ≡ 1 mod 8）
  # となる最小の奇数 N。型ごとに非増加という枝刈りを使う。
  def minprod_cong(m)
    return 1 if m == 1

    k = Math.log2(m).floor + 1
    qs = primes_mod8(3, k)
    ps = primes_mod8(1, k)
    primes = (qs + ps).sort.map { |p| [p, p % 8 == 3] } # [素数, Q 型か]

    best = nil
    rec = lambda { |rem, idx, cur, max_q, max_p, qpar|
      return if best && cur >= best
      if rem == 1
        best = cur if qpar.zero? && (best.nil? || cur < best)
        return
      end
      return if idx >= primes.size

      p, q_type = primes[idx]
      return if best && p >= best

      # この素数を使わない
      rec.call(rem, idx + 1, cur, max_q, max_p, qpar)

      lim = q_type ? max_q : max_p
      divisors_from(rem, 2).each { |d|
        e = d - 1
        next if e > lim

        v = cur * p**e
        next if best && v >= best

        if q_type
          rec.call(rem / d, idx + 1, v, e, max_p, (qpar + e) % 2)
        else
          rec.call(rem / d, idx + 1, v, max_q, e, qpar)
        end
      }
    }
    rec.call(m, 0, 1, Float::INFINITY, Float::INFINITY, 0)
    best
  end

  # ---------- 本体 ----------

  # B(n) = A200977(n): R_2(k) = n となる最小の正整数 k
  def b(n)
    [minprod(2 * n), minprod(2 * n + 1)].compact.min
  end

  # C(n): R_2(k) = n となる最小の奇数 k ≡ 1 (mod 8)
  #   非退化 D = 2n は合同条件つき、退化 D = 2n+1 は自動的に条件を満たす
  def c(n)
    [minprod_cong(2 * n), minprod(2 * n + 1)].compact.min
  end

  def a(n)
    return 0 if n.zero?

    [c(n), 4 * b(n)].min
  end

  def sequence(nmax)
    (0..nmax).map { |n| a(n) }
  end

  # k <= 6*10^7 の総当たり走査で確定した項
  KNOWN = {
    0 => 0, 1 => 9, 2 => 33, 3 => 153, 4 => 297, 5 => 1377, 6 => 1881,
    7 => 9801, 8 => 5049, 9 => 18513, 10 => 16929, 11 => 1003833,
    12 => 31977, 13 => 314721, 14 => 152361, 15 => 166617, 16 => 207009,
    17 => 10673289, 18 => 543609, 20 => 287793, 21 => 1499553,
    22 => 2832489, 24 => 1055241, 25 => 20160657
  }.freeze

  def check_known
    ok = true
    KNOWN.keys.sort.each { |n|
      want = KNOWN[n]
      got = a(n)
      good = (got == want)
      ok &&= good
      puts format('%-4s n=%-3d got=%-12d known=%d', good ? 'OK' : 'NG', n, got, want)
    }
    puts ok ? "\nmatches all #{KNOWN.size} known terms." : "\nMISMATCH."
    ok
  end

  # ---------- 検証用の総当たり ----------

  # x^2 + 8*y^2 = k となる正整数の組の個数
  def count_pairs(k)
    cnt = 0
    y = 1
    while 8 * y * y < k
      r = k - 8 * y * y
      s = Integer.sqrt(r)
      cnt += 1 if s * s == r && s.positive?
      y += 1
    end
    cnt
  end

  def verify(limit = 5_000_000, nmax = 40)
    warn "brute force scan up to k = #{limit} ..."
    cnt = Array.new(limit + 1, 0)
    y = 1
    while 8 * y * y <= limit
      x = 1
      while x * x + 8 * y * y <= limit
        cnt[x * x + 8 * y * y] += 1
        x += 1
      end
      y += 1
    end

    first = {}
    cnt.each_with_index { |c, k| first[c] ||= k }
    first[0] = 0 # k = 0 に正の組はない

    ok = true
    (0..nmax).each { |n|
      theory = a(n)
      brute  = first[n]

      if theory <= limit
        good = (brute == theory)
        puts format('%-4s n=%-4d theory=%-14d brute=%s',
                    good ? 'OK' : 'NG', n, theory, brute.inspect)
      else
        good = brute.nil?
        puts format('%-4s n=%-4d theory=%-14d (> limit, brute=%s)',
                    good ? 'skip' : 'NG', n, theory, brute.inspect)
      end
      ok &&= good
    }
    puts ok ? "\nall matched." : "\nMISMATCH FOUND."
    ok
  end
end

if __FILE__ == $PROGRAM_NAME
  if ARGV.include?('--known')
    A399213.check_known
  elsif ARGV.include?('--verify')
    nums = ARGV.select { |s| s =~ /\A\d+\z/ }.map(&:to_i)
    A399213.verify(nums[0] || 5_000_000, nums[1] || 40)
  else
    nmax = (ARGV[0] || 100).to_i
    A399213.sequence(nmax).each_with_index { |v, n| puts "#{n} #{v}" }
  end
end
