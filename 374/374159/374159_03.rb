#!/usr/bin/env ruby
# frozen_string_literal: true
#
# A374159
#   a(n) is the smallest nonnegative integer k where exactly n pairs of
#   positive integers (x, y) exist such that x^2 + 7*y^2 = k.
#   （A216511(k) = n となる最小の k）
#
# ------------------------------------------------------------------
# 理論（判別式 -28 = 4*(-7) は非基本判別式。Z[sqrt(-7)] は
#       Z[(1+sqrt(-7))/2] の指数 2 の非最大整環）
# ------------------------------------------------------------------
#   x^2+3y^2 (判別式 -12) と同じく指数 2 の非最大整環だが、-3 のときと違って
#   2 は Q(sqrt(-7)) で分解する（chi_7(2) = +1、2 = 3^2 mod 7）。そのため
#   補正は「奇数か 4 の倍数か」ではなく v_2(k) 全体に効く。
#
#   k = 2^v * 7^c * Π p_i^e_i * Π q_j^f_j
#       chi_7(p_i) = +1  (p_i ≡ 1, 2, 4 mod 7、p_i は 11, 23, 29, 37, 43, ...)
#       chi_7(q_j) = -1  (q_j ≡ 3, 5, 6 mod 7)
#
#   M(k) := Π(e_i + 1)   ... f_j が全て偶数のとき
#         := 0           ... それ以外
#   （7 の指数 c は M に効かない。chi_7(7) = 0）
#
#   c(v) := 1      (v = 0)
#           0      (v = 1)
#           v - 1  (v >= 2)
#   とおくと、符号込み整数解の総数はぴったり
#       R(k) = 2 * c(v) * M(k)
#   （k <= 20000 で総当たりと完全一致）。v = 1 で消え、v >= 2 で線形に増える
#   のがこの形の特徴で、A374158 の「4 | k なら 3 倍」に対応する部分。
#
#   退化解は y=0 (k が平方) と x=0 (k/7 が平方) の 2 種。
#   eps := [k は平方] + [k/7 は平方] ∈ {0, 1} とおくと
#       P(k) = (R(k) - 2*eps) / 4 = (c(v)*M - eps) / 2
#   ここで eps = (c(v)*M) mod 2 が自動的に成り立つ:
#       c(v) が奇 <=> v が偶、  M が奇 <=> 全 e_i が偶
#       この 2 つが同時に成り立つことが「k または k/7 が平方」と同値。
#   よって T(k) := c(v) * M(k) とおけば
#       P(k) = floor(T(k) / 2)
#
#   したがって P(k) = n となる最小の k は、T ∈ {2n, 2n+1} の各約数 d について
#       d = 1  -> minprod(T)              (v = 0、k は奇数)
#       d >= 2 -> 2^(d+1) * minprod(T/d)  (v = d+1、c(v) = d)
#   を全部並べて最小を取ればよい。
#   （v = 2 も c = 1 だが 4 * minprod(T) となり v = 0 に必ず負けるので不要）
#
#   minprod(M) := 11, 23, 29, 37, 43, ... （p ≡ 1,2,4 mod 7 の奇素数）だけを
#                 使って Π(e_i + 1) = M となる最小の数
#
#   参考: T が偶数のとき minprod(T) >= 11 * minprod(T/2) > 8 * minprod(T/2)
#   （指数 e が奇の素数を 1 つ選び e -> (e-1)/2 とすると M は半分、値は
#     p^((e+1)/2) >= 11 分の 1 以下になる）なので、偶数 T 側の d = 1 は
#     d = 2 に必ず負ける。ゆえに奇数の項が出るとすれば T = 2n+1 経由のみ。
#   n <= 3000 では全項が 8 の倍数で、OEIS の予想と整合。
#
# 使い方:
#   ruby 374159_01.rb           # n = 0..100 を出力
#   ruby 374159_01.rb 30        # n = 0..30 を出力
#   ruby 374159_01.rb --known   # OEIS 既知項と照合
#   ruby 374159_01.rb --verify  # 総当たりと突き合わせて理論式を検証

module A374159
  module_function

  QR7 = [1, 2, 4].freeze

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

  # chi_7(p) = +1 となる奇素数を小さい順に count 個
  def primes_qr7(count)
    ps = []
    n = 3
    while ps.size < count
      ps << n if n != 7 && QR7.include?(n % 7) && prime?(n)
      n += 2
    end
    ps
  end

  # ---------- minprod ----------

  # Π(e_i + 1) = m となる最小の Π primes[i]^e_i
  # 指数は非増加としてよい（小さい素数に大きい指数を割り当てるのが最適）
  def minprod(m, primes = nil, idx = 0, max_exp = Float::INFINITY, memo = {})
    return 1 if m == 1

    primes ||= primes_qr7(Math.log2(m).floor + 1)
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

  def divisors(m)
    divisors_from(m, 1)
  end

  # ---------- 本体 ----------

  def a(n)
    return 0 if n.zero?

    cands = []
    [2 * n, 2 * n + 1].each { |t|
      divisors(t).each { |d|
        w = minprod(t / d)
        next if w.nil?

        cands << (d == 1 ? w : (1 << (d + 1)) * w)
      }
    }
    cands.min
  end

  def sequence(nmax)
    (0..nmax).map { |n| a(n) }
  end

  # OEIS の既知項（n => a(n)）。data 部 a(0)..a(33) とコメント欄の値。
  KNOWN = {
    0 => 0, 1 => 8, 2 => 32, 3 => 128, 4 => 352, 5 => 704, 6 => 1408,
    7 => 2816, 8 => 5632, 9 => 11264, 10 => 16192, 11 => 45056,
    12 => 32384, 13 => 123904, 14 => 64768, 15 => 178112, 16 => 129536,
    17 => 2883584, 18 => 259072, 19 => 1982464, 20 => 469568,
    21 => 712448, 22 => 1036288, 23 => 184549376, 24 => 939136,
    25 => 21551552, 26 => 4145152, 27 => 2849792, 28 => 1878272,
    29 => 11811160064, 30 => 5165248, 31 => 16386304, 32 => 3756544,
    33 => 11399168, 34 => 66322432, 35 => 86206208, 36 => 7513088
  }.freeze

  def check_known
    ok = true
    KNOWN.keys.sort.each { |n|
      want = KNOWN[n]
      got = a(n)
      good = (got == want)
      ok &&= good
      puts format('%-4s n=%-3d got=%-14d oeis=%d', good ? 'OK' : 'NG', n, got, want)
    }
    puts ok ? "\nmatches all #{KNOWN.size} known terms." : "\nMISMATCH."
    ok
  end

  # ---------- 検証用の総当たり ----------

  # x^2 + 7*y^2 = k となる正整数の組の個数（A216511）
  def count_pairs(k)
    cnt = 0
    y = 1
    while 7 * y * y < k
      r = k - 7 * y * y
      s = Integer.sqrt(r)
      cnt += 1 if s * s == r && s.positive?
      y += 1
    end
    cnt
  end

  # k <= limit を総当たりして first[n] を作り、理論値と突き合わせる。
  # 理論値が limit を超える n は「その範囲に現れないこと」だけを確認する。
  def verify(limit = 5_000_000, nmax = 40)
    warn "brute force scan up to k = #{limit} ..."
    cnt = Array.new(limit + 1, 0)
    y = 1
    while 7 * y * y <= limit
      x = 1
      while x * x + 7 * y * y <= limit
        cnt[x * x + 7 * y * y] += 1
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
    A374159.check_known
  elsif ARGV.include?('--verify')
    nums = ARGV.select { |s| s =~ /\A\d+\z/ }.map(&:to_i)
    A374159.verify(nums[0] || 5_000_000, nums[1] || 40)
  else
    nmax = (ARGV[0] || 100).to_i
    A374159.sequence(nmax).each_with_index { |v, n| puts "#{n} #{v}" }
  end
end
