#!/usr/bin/env ruby
# frozen_string_literal: true
#
# A374285
#   a(n) is the smallest nonnegative integer k where there are exactly n
#   nonnegative integer solutions to x^2 + 2*y^2 = k.
#
# ------------------------------------------------------------------
# 理論（判別式 -8 は基本判別式。Z[sqrt(-2)] は最大整環かつ PID なので
#       表現数は完全に乗法的。A374158 のような mod 4 の補正は出ない）
# ------------------------------------------------------------------
#   k = 2^a * Π p_i^e_i * Π q_j^f_j    (p_i ≡ 1,3 mod 8,  q_j ≡ 5,7 mod 8)
#
#   M(k) := Π(e_i + 1)   ... f_j が全て偶数のとき
#         := 0           ... それ以外
#   （2 の指数 a は M に効かない）
#
#   符号込み整数解の総数は  R(k) = 2 * M(k)   （分岐なし）
#   退化解は y=0 (k が平方) と x=0 (k/2 が平方) の 2 種で、それぞれ符号込み 2 個。
#   eps := [k は平方] + [k/2 は平方] ∈ {0, 1} とおくと、x, y >= 0 の解の個数は
#       N(k) = (R(k) + 2*eps) / 4 = (M + eps) / 2
#
#   eps は M のパリティで自動的に決まる:
#     M が奇 <=> 全 e_i が偶 <=> p 部分が平方
#              => k = 2^a * (平方) なので a の偶奇で必ず eps = 1
#     M が偶 <=> ある e_i が奇 => k も k/2 も平方でない => eps = 0
#   よって
#       N(k) = ceil(M / 2)
#
#   したがって N(k) = n となる最小の k は
#       a(n) = min( minprod(2n - 1), minprod(2n) )      (n >= 2)
#   の 2 通りを比べるだけでよい。2^a や q^(偶数) を掛けても M は変わらず
#   k が増えるだけなので、純粋な p 部分の最小値がそのまま答になる。
#
#   minprod(M) := 8k+1, 8k+3 型素数 3, 11, 17, 19, 41, ... のみを使って
#                 Π(e_i + 1) = M となる最小の数
#
#   例外:
#     a(0) = 5  ... N = 0 は M = 0、すなわち q ≡ 5,7 (mod 8) が奇数乗で
#                   現れる最小の k。それは 5 自身。
#     a(1) = 0  ... k = 0 は (0, 0) の 1 解を持ち、これが最小。
#                   （公式側は minprod(1) = 1 を返すが 0 のほうが小さい）
#
# 使い方:
#   ruby 374285_01.rb           # n = 0..100 を出力
#   ruby 374285_01.rb 30        # n = 0..30 を出力
#   ruby 374285_01.rb --known   # OEIS 既知項と照合
#   ruby 374285_01.rb --verify  # 総当たりと突き合わせて理論式を検証

module A374285
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

  # p ≡ 1 or 3 (mod 8) な素数を小さい順に count 個（x^2+2y^2 で分解する素数）
  def primes_split(count)
    ps = []
    n = 3
    while ps.size < count
      ps << n if [1, 3].include?(n % 8) && prime?(n)
      n += 2
    end
    ps
  end

  # ---------- minprod ----------

  # Π(e_i + 1) = m となる最小の Π primes[i]^e_i
  # 指数は非増加としてよい（小さい素数に大きい指数を割り当てるのが最適）
  def minprod(m, primes = nil, idx = 0, max_exp = Float::INFINITY, memo = {})
    return 1 if m == 1

    primes ||= primes_split(Math.log2(m).floor + 1)
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

  # ---------- 本体 ----------

  def a(n)
    return 5 if n.zero?  # M = 0 になる最小の k
    return 0 if n == 1   # k = 0 -> (0, 0)

    cands = []
    [2 * n - 1, 2 * n].each { |m|
      v = minprod(m)
      cands << v if v
    }
    cands.min
  end

  def sequence(nmax)
    (0..nmax).map { |n| a(n) }
  end

  # OEIS の既知項（n => a(n)）。data 部 a(0)..a(18) とコメント欄の値。
  KNOWN = {
    0 => 5, 1 => 0, 2 => 9, 3 => 81, 4 => 297, 5 => 891, 6 => 1683,
    7 => 8019, 8 => 5049, 9 => 18513, 10 => 15147, 11 => 88209,
    12 => 31977, 13 => 1185921, 14 => 136323, 15 => 166617, 16 => 95931,
    17 => 7144929, 18 => 351747,
    20 => 287793, 21 => 1499553, 22 => 11042163, 23 => 2832489,
    24 => 863379, 25 => 20160657, 26 => 99379467, 27 => 5979699,
    28 => 2590137, 30 => 3165723, 32 => 3933171, 33 => 121463793,
    35 => 181445913, 36 => 9497169
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

  # x^2 + 2*y^2 = k となる非負整数解の個数
  def count_solutions(k)
    return 1 if k.zero?

    cnt = 0
    y = 0
    while 2 * y * y <= k
      r = k - 2 * y * y
      s = Integer.sqrt(r)
      cnt += 1 if s * s == r
      y += 1
    end
    cnt
  end

  # k <= limit を総当たりして first[n] を作り、理論値と突き合わせる。
  # 理論値が limit を超える n は「その範囲に現れないこと」だけを確認する。
  def verify(limit = 5_000_000, nmax = 40)
    warn "brute force scan up to k = #{limit} ..."
    cnt = Array.new(limit + 1, 0)
    y = 0
    while 2 * y * y <= limit
      x = 0
      while x * x + 2 * y * y <= limit
        cnt[x * x + 2 * y * y] += 1
        x += 1
      end
      y += 1
    end

    first = {}
    cnt.each_with_index { |c, k| first[c] ||= k }

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
    A374285.check_known
  elsif ARGV.include?('--verify')
    nums = ARGV.select { |s| s =~ /\A\d+\z/ }.map(&:to_i)
    A374285.verify(nums[0] || 5_000_000, nums[1] || 40)
  else
    nmax = (ARGV[0] || 100).to_i
    A374285.sequence(nmax).each_with_index { |v, n| puts "#{n} #{v}" }
  end
end
