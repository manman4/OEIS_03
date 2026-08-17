#!/usr/bin/env ruby
# frozen_string_literal: true
#
# A374286
#   a(n) is the smallest nonnegative integer k where there are exactly n
#   nonnegative integer solutions to x^2 + 3*y^2 = k.
#   （A119395(k) = n となる最小の k）
#
# ------------------------------------------------------------------
# 理論（判別式 -12 は非基本判別式。Z[sqrt(-3)] は Z[omega] の指数 2 の
#       非最大整環なので、表現数は k mod 4 に依存する補正を受ける）
# ------------------------------------------------------------------
#   k = 2^a * 3^c * Π p_i^e_i * Π q_j^f_j    (p_i ≡ 1 mod 3, q_j ≡ 2 mod 3)
#
#   M(k) := Π(e_i + 1)   ... f_j が全て偶数のとき
#         := 0           ... それ以外
#   （2 と 3 の指数は M に効かない）
#
#   符号込み整数解の総数は、r_{-3}(k) = 6*(d_{1,3}(k) - d_{2,3}(k)) = 6*M として
#       R(k) = 6*M   ... 4 | k
#            = 2*M   ... k が奇数        <-- ここの 1/3 倍が「補正」
#            = 0     ... k ≡ 2 (mod 4)
#
#   退化解は y=0 (k が平方) と x=0 (k/3 が平方) の 2 種で、それぞれ符号込み 2 個。
#   eps := [k は平方] + [k/3 は平方] ∈ {0, 1} とおくと x, y >= 0 の解の個数は
#       N(k) = (R(k) + 2*eps) / 4
#   すなわち
#       4 | k     -> N = (3M + eps) / 2
#       k が奇数  -> N = (M + eps) / 2
#
#   eps は M のパリティで自動的に決まる:
#     M が奇 <=> 全 e_i が偶 <=> p 部分が平方
#       k 奇  : k = 3^c * (平方) なので c の偶奇で必ず eps = 1
#       4 | k : a が偶なら eps = 1、a が奇なら eps = 0
#     M が偶 <=> ある e_i が奇 => k も k/3 も平方でない => eps = 0
#   よって eps = M mod 2 が整合条件になり
#       k が奇数  -> N = ceil(M / 2)
#       4 | k     -> N = (3M + (M mod 2)) / 2     （M 奇のときは a が偶、最安は a = 2）
#
#   したがって N(k) = n (n >= 2) となる最小の k の候補は、m ∈ {2n-1, 2n} に対し
#     (B) k が奇数: minprod(m)
#     (A) 4 | k   : 3 | m のとき 4 * minprod(m / 3)
#   の高々 4 通りだけ。
#
#   minprod(M) := 3k+1 型素数 7, 13, 19, 31, ... のみを使って
#                 Π(e_i + 1) = M となる最小の数
#
#   例外:
#     a(0) = 2  ... N = 0 になる最小の k。k ≡ 2 (mod 4) は R も eps も 0。
#     a(1) = 0  ... k = 0 は (0, 0) の 1 解を持ち、これが最小。
#
#   n ≡ 1 (mod 3) のとき 2n-1 も 2n も 3 で割れず、3 倍お得な分岐 (A) が
#   完全に封じられる。a(4) = 1729, a(7) = 1529437, a(13) = 68574961 の
#   ような跳ね上がりはこれが原因。
#   （姉妹列 A374158 は正の組を数えるので m ∈ {2n, 2n+1} となり、
#     詰まる剰余類が n ≡ 2 (mod 3) にずれる。）
#
# 使い方:
#   ruby 374286_03.rb           # n = 0..100 を出力
#   ruby 374286_03.rb 30        # n = 0..30 を出力
#   ruby 374286_03.rb --known   # OEIS 既知項と照合
#   ruby 374286_03.rb --verify  # 総当たりと突き合わせて理論式を検証

module A374286
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

  # p ≡ 1 (mod 3) な素数を小さい順に count 個
  def primes_1mod3(count)
    ps = []
    n = 7
    while ps.size < count
      ps << n if prime?(n)
      n += 3
    end
    ps
  end

  # ---------- minprod ----------

  # Π(e_i + 1) = m となる最小の Π primes[i]^e_i
  # 指数は非増加としてよい（小さい素数に大きい指数を割り当てるのが最適）
  def minprod(m, primes = nil, idx = 0, max_exp = Float::INFINITY, memo = {})
    return 1 if m == 1

    primes ||= primes_1mod3(Math.log2(m).floor + 1)
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
    return 2 if n.zero?  # k ≡ 2 (mod 4) の最小
    return 0 if n == 1   # k = 0 -> (0, 0)

    cands = []
    [2 * n - 1, 2 * n].each { |m|
      # (B) k が奇数: N = ceil(M / 2) なので M = 2n-1 または 2n
      v = minprod(m)
      cands << v if v

      # (A) 4 | k: 3M + (M mod 2) = 2n すなわち 3M = m のとき
      next unless (m % 3).zero?

      w = minprod(m / 3)
      cands << 4 * w if w
    }
    cands.min
  end

  def sequence(nmax)
    (0..nmax).map { |n| a(n) }
  end

  # OEIS の既知項（n => a(n)）。data 部 a(0)..a(18) とコメント欄の値。
  KNOWN = {
    0 => 2, 1 => 0, 2 => 4, 3 => 28, 4 => 1729, 5 => 196, 6 => 364,
    7 => 1529437, 8 => 9604, 9 => 2548, 10 => 593047, 11 => 470596,
    12 => 6916, 13 => 68574961, 14 => 33124, 15 => 124852, 16 => 1983163,
    17 => 1129900996, 18 => 48412,
    20 => 18384457, 21 => 6117748, 23 => 1623076, 24 => 214396,
    27 => 629356, 30 => 2372188, 32 => 79530724, 36 => 1500772,
    41 => 11957764, 42 => 116237212
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

  # x^2 + 3*y^2 = k となる非負整数解の個数（A119395）
  def count_solutions(k)
    return 1 if k.zero?

    cnt = 0
    y = 0
    while 3 * y * y <= k
      r = k - 3 * y * y
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
    while 3 * y * y <= limit
      x = 0
      while x * x + 3 * y * y <= limit
        cnt[x * x + 3 * y * y] += 1
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
    A374286.check_known
  elsif ARGV.include?('--verify')
    nums = ARGV.select { |s| s =~ /\A\d+\z/ }.map(&:to_i)
    A374286.verify(nums[0] || 5_000_000, nums[1] || 40)
  else
    nmax = (ARGV[0] || 100).to_i
    A374286.sequence(nmax).each_with_index { |v, n| puts "#{n} #{v}" }
  end
end
