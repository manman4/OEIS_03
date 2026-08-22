#!/usr/bin/env ruby
# frozen_string_literal: true
#
# A399198
#   a(n) is the smallest nonnegative integer k where there are exactly n
#   nonnegative integer solutions to x^2 + 4*y^2 = k.
#   （A399191 の非負解版）
#
# ------------------------------------------------------------------
# 理論（判別式 -16 = 4*(-4)。Z[2i] は Z[i] の指数 2 の非最大整環で、
#       2 は Q(i) で「分岐」する）
# ------------------------------------------------------------------
#   O = Z[2i] = Z + 2*Z[i],  O_K = Z[i],  h(-4) = 1,  O_K^* = {±1, ±i}
#   2*O_K = p^2  (p = (1+i))、O_K/2*O_K は局所環で Z の像は {0, 1}。
#   ノルム 2^v * u (u 奇) のイデアルは p^v * J（N(J) = u）と一意に書け、
#   その個数は M(u) := Π(e_i + 1)（p_i ≡ 1 mod 4、q_j ≡ 3 mod 4 は偶指数、
#   でなければ 0）。生成元 alpha, i*alpha, -alpha, -i*alpha のうち O に入る
#   個数は
#       c(v) := 2 (v = 0), 0 (v = 1), 4 (v >= 2)
#   （v>=2 は alpha ∈ 2*O_K で 4 個、v=0 は [alpha] と [i*alpha] のちょうど
#     一方が 1 で 2 個、v=1 は両方 1+i で 0 個）。よって
#       R(k) = c(v) * M(u)   （符号込み整数解の総数、k <= 20 万で総当たりと一致）
#
#   退化解は y=0 (k が平方) と x=0 (k/4 が平方)。k/4 が平方なら k=(2s)^2 も
#   平方なので 2 条件は独立でなく
#       eps := [k は平方] + [k/4 は平方]
#            = 0 (非平方) / 1 (奇数の平方) / 2 (偶数の平方)
#   x, y >= 0 の解の個数は R = 4*P + 2*eps と N = P + eps から
#       N(k) = (R(k) + 2*eps) / 4
#   v ごとに整理すると（eps は v と M から自動的に決まる）
#       v = 0   -> eps = M mod 2 なので  N = ceil(M / 2)
#       v = 1   -> N = 0
#       v >= 2  -> eps = 2 <=> (v が偶 かつ M が奇) なので
#                  N = M + [v が偶 かつ M が奇]
#
#   したがって N(k) = n (n >= 2) となる最小の k の候補は
#     (B) v = 0        : M ∈ {2n-1, 2n} なので minprod(2n-1), minprod(2n)
#     (A1) v 奇 (最安 v=3): N = M なので 8 * minprod(n)
#     (A2) v 偶 (最安 v=2): N = M + [M 奇] なので
#                          M = n   (n が偶、M も偶) -> 4 * minprod(n)
#                          M = n-1 (n が偶、M は奇) -> 4 * minprod(n-1)
#   の高々 4 通り。n が奇数のとき (A2) は N が必ず偶数になるので使えず、
#   n が偶数のとき (A1) は (A2) に必ず負ける（8 > 4）。まとめると
#       n 奇: 3 候補   n 偶: 4 候補
#
#   minprod(M) := 4k+1 型素数 5, 13, 17, 29, ... のみを使って
#                 Π(e_i + 1) = M となる最小の数（= A018782(M)）
#
#   例外:
#     a(0) = 2  ... N = 0 になる最小の k。v_2(k) = 1 なら c(1) = 0。
#     a(1) = 0  ... k = 0 は (0, 0) の 1 解を持ち、これが最小。
#                   （公式側は minprod(1) = 1 を返すが 0 のほうが小さい）
#
# 使い方:
#   ruby 399198_01.rb           # n = 0..100 を出力
#   ruby 399198_01.rb 30        # n = 0..30 を出力
#   ruby 399198_01.rb --known   # 総当たりで確定済みの項と照合
#   ruby 399198_01.rb --verify  # 総当たりと突き合わせて理論式を検証

module A399198
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

  # p ≡ 1 (mod 4) な素数を小さい順に count 個
  def primes_1mod4(count)
    ps = []
    n = 5
    while ps.size < count
      ps << n if prime?(n)
      n += 4
    end
    ps
  end

  # ---------- minprod（= A018782） ----------

  # Π(e_i + 1) = m となる最小の Π primes[i]^e_i
  # 指数は非増加としてよい（小さい素数に大きい指数を割り当てるのが最適）
  def minprod(m, primes = nil, idx = 0, max_exp = Float::INFINITY, memo = {})
    return 1 if m == 1

    primes ||= primes_1mod4(Math.log2(m).floor + 1)
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
    return 2 if n.zero?  # v_2(k) = 1 の最小
    return 0 if n == 1   # k = 0 -> (0, 0)

    cands = []
    # (B) v = 0 : N = ceil(M / 2)
    [2 * n - 1, 2 * n].each { |m|
      v = minprod(m)
      cands << v if v
    }
    if n.odd?
      # (A1) v 奇（最安 v = 3）: N = M
      w = minprod(n)
      cands << 8 * w if w
    else
      # (A2) v 偶（最安 v = 2）: N = M + [M 奇]
      [n, n - 1].each { |m|
        w = minprod(m)
        cands << 4 * w if w
      }
    end
    cands.min
  end

  def sequence(nmax)
    (0..nmax).map { |n| a(n) }
  end

  # k <= 3*10^6 の総当たり走査で確定した項（n => a(n)）
  KNOWN = {
    0 => 2, 1 => 0, 2 => 4, 3 => 200, 4 => 100, 5 => 4225, 6 => 1300,
    7 => 125000, 8 => 4420, 9 => 33800, 10 => 16900, 11 => 2640625,
    12 => 22100, 14 => 812500, 15 => 845000, 16 => 110500, 18 => 287300,
    20 => 552500
  }.freeze

  def check_known
    ok = true
    KNOWN.keys.sort.each { |n|
      want = KNOWN[n]
      got = a(n)
      good = (got == want)
      ok &&= good
      puts format('%-4s n=%-3d got=%-14d known=%d', good ? 'OK' : 'NG', n, got, want)
    }
    puts ok ? "\nmatches all #{KNOWN.size} known terms." : "\nMISMATCH."
    ok
  end

  # ---------- 検証用の総当たり ----------

  # x^2 + 4*y^2 = k となる非負整数解の個数
  def count_solutions(k)
    return 1 if k.zero?

    cnt = 0
    y = 0
    while 4 * y * y <= k
      r = k - 4 * y * y
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
    while 4 * y * y <= limit
      x = 0
      while x * x + 4 * y * y <= limit
        cnt[x * x + 4 * y * y] += 1
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
        puts format('%-4s n=%-4d theory=%-16d brute=%s',
                    good ? 'OK' : 'NG', n, theory, brute.inspect)
      else
        good = brute.nil?
        puts format('%-4s n=%-4d theory=%-16d (> limit, brute=%s)',
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
    A399198.check_known
  elsif ARGV.include?('--verify')
    nums = ARGV.select { |s| s =~ /\A\d+\z/ }.map(&:to_i)
    A399198.verify(nums[0] || 5_000_000, nums[1] || 40)
  else
    nmax = (ARGV[0] || 100).to_i
    A399198.sequence(nmax).each_with_index { |v, n| puts "#{n} #{v}" }
  end
end
