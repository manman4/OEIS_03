#!/usr/bin/env ruby
# frozen_string_literal: true
#
# A399267
#   a(n) is the smallest nonnegative integer k where there are exactly n
#   nonnegative integer solutions to x^2 + 8*y^2 = k.
#   （A399213 の非負解版）
#
# ------------------------------------------------------------------
# 理論（判別式 -32。x^2+2y^2 への帰着で処理する）
# ------------------------------------------------------------------
#   判別式 -32 は Z[sqrt(-2)] の中の指数 2 の非最大整環に対応するが、
#   類群を直接扱わなくても、初等的に x^2+2y^2 (判別式 -8、PID) に帰着できる。
#   N_8, N_2 をそれぞれ非負整数解の個数とすると
#
#     (a) k が奇数なら x も奇数なので x^2 ≡ 1 (mod 8)、よって k ≡ 1 (mod 8)。
#         逆に k ≡ 1 (mod 8) なら X^2+2Y^2=k の解は Y が偶数（Y 奇だと
#         X^2+2Y^2 ≡ 3 mod 8）。Y=2y とおけば X^2+8y^2=k。
#         また奇数 k では x=0 の解（8y^2=k）はないので、退化解の対応も一致する。
#             k ≡ 1 (mod 8)        -> N_8(k) = N_2(k)
#             k 奇かつ k ≢ 1 (mod 8) -> N_8(k) = 0
#     (b) k が偶数なら x も偶数なので 4 | k。x=2X, k=4u とおけば
#             N_8(4u) = N_2(u)
#         （k ≡ 2 (mod 4) なら x^2 ≡ 2 (mod 4) は不可能で解なし）
#     （k = 0 も 4|k の場合に含まれ、N_8(0) = N_2(0) = 1）
#
#   k <= 200000 で総当たりと完全一致。
#
# ------------------------------------------------------------------
# x^2+2y^2 の非負解の個数
# ------------------------------------------------------------------
#   u = 2^s * Π p_i^{e_i} * Π q_j^{f_j}
#       p_i ≡ 1, 3 (mod 8) : 分解  3, 11, 17, 19, 41, 43, 59, 67, ...
#       q_j ≡ 5, 7 (mod 8) : 惰性
#   M(u) := Π(e_i + 1)（f_j に奇数があれば 0）とすると（A374285 と同じ）
#       N_2(u) = ceil( M(u) / 2 )
#   なので N_2(u) = n となるのは M(u) ∈ {2n-1, 2n} のときだけ。
#
# ------------------------------------------------------------------
# 合同条件 k ≡ 1 (mod 8)
# ------------------------------------------------------------------
#   分解素数を
#       Q 型: p ≡ 3 (mod 8)   3, 11, 19, 43, 59, 67, ...
#       P 型: p ≡ 1 (mod 8)   17, 41, 73, 89, 97, 113, ...
#   に分けると、奇数 N について
#       N ≡ 1 (mod 8)  <=>  Σ_{p が Q 型} e_p が偶数
#   （惰性素数 q は偶数乗で現れ、q^2 ≡ 1 (mod 8) なので合同条件に影響しない）
#
#   M が奇数のときは全指数が偶数なので Q 型指数和も偶数、つまり合同条件は
#   自動的に成立する。したがって、n >= 2 に対する候補は次の 3 つだけ:
#
#       (1) k 奇, M = 2n     -> minprod_cong(2n)     （合同条件つき最小）
#       (2) k 奇, M = 2n-1   -> minprod(2n-1)        （条件は自動成立）
#       (3) 4 | k            -> 4 * minprod(2n)      （= 4*A374285(n) の主要部）
#
#   (3) で 4*minprod(2n-1) を挙げないのは、(2) の minprod(2n-1) に必ず負けるため。
#
#   探索の枝刈りについて:
#     制約なしの minprod では「小さい素数ほど大きい指数」としてよいが、
#     合同条件つきでは型をまたぐ指数の交換が Q 型指数和の偶奇を壊すので、
#     全体での非増加は仮定できない。同じ型の 2 素数の間の交換は D も偶奇も
#     変えないので「型ごとに、小さい素数ほど大きい指数」としてよい。
#
#   例外:
#     a(0) = 2  ... k ≡ 2 (mod 4) は解を持たない。その最小。
#     a(1) = 0  ... k = 0 は (0, 0) の 1 解を持ち、これが最小。
#
# 使い方:
#   ruby 399267_01.rb           # a(n) が 1000 桁を超える手前まで出力
#   ruby 399267_01.rb 30        # n = 0..30 を出力（桁数制限も併用）
#   ruby 399267_01.rb --digits=200   # 桁数の上限を変更
#   ruby 399267_01.rb --known   # 総当たりで確定済みの項と照合
#   ruby 399267_01.rb --verify  # 総当たりと突き合わせて検証

module A399267
  module_function

  # 出力する a(n) の桁数上限。ちょうどこの桁数までは出力し、
  # 超えた時点で打ち切る（1000 桁は OK、1001 桁で停止）。
  MAX_DIGITS = 1000

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

  # [素数, Q 型か] を小さい順に。各型 count 個ずつ用意して統合する。
  def typed_primes(count)
    qs = primes_mod8(3, count).map { |p| [p, true] }
    ps = primes_mod8(1, count).map { |p| [p, false] }
    (qs + ps).sort_by(&:first)
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

  # ---------- 制約なしの最小値 ----------

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

  # Π(e_i + 1) = m かつ Σ_{Q 型} e_i が偶数（= N ≡ 1 mod 8）となる最小の奇数 N。
  # 型ごとに非増加という枝刈りと、状態のメモ化を使う。
  #   状態 = [残りの m, 素数の添字, Q 型の指数上限, P 型の指数上限, Q 型指数和の偶奇]
  # 使われる素数は各型の先頭からの連続部分になるので、各型 log2(m)+1 個で足りる。
  INF = Float::INFINITY

  def minprod_cong(m)
    return 1 if m == 1

    primes = typed_primes(Math.log2(m).floor + 1)
    memo = {}
    v = cong_rec(m, 0, INF, INF, 0, primes, memo)
    v == INF ? nil : v
  end

  def cong_rec(rem, idx, max_q, max_p, qpar, primes, memo)
    return qpar.zero? ? 1 : INF if rem == 1
    return INF if idx >= primes.size

    key = [rem, idx, max_q, max_p, qpar]
    cached = memo[key]
    return cached if cached

    p, q_type = primes[idx]
    best = cong_rec(rem, idx + 1, max_q, max_p, qpar, primes, memo) # 使わない

    lim = q_type ? max_q : max_p
    divisors_from(rem, 2).each { |d|
      e = d - 1
      next if e > lim

      rest = if q_type
               cong_rec(rem / d, idx + 1, e, max_p, (qpar + e) % 2, primes, memo)
             else
               cong_rec(rem / d, idx + 1, max_q, e, qpar, primes, memo)
             end
      next if rest == INF

      cand = p**e * rest
      best = cand if cand < best
    }
    memo[key] = best
  end

  # ---------- 本体 ----------

  def a(n)
    return 2 if n.zero? # k ≡ 2 (mod 4) の最小
    return 0 if n == 1  # k = 0 -> (0, 0)

    cands = []
    v = minprod_cong(2 * n)   # 奇数 k, M = 2n（合同条件つき）
    cands << v if v
    v = minprod(2 * n - 1)    # 奇数 k, M = 2n-1（条件は自動成立）
    cands << v if v
    v = minprod(2 * n)        # 4 | k
    cands << 4 * v if v
    cands.min
  end

  # a(n) の桁数が max_digits を超えた時点で打ち切る。
  # nmax を与えた場合はそこでも打ち切る（どちらか早いほう）。
  # 戻り値は [n, a(n)] の配列。
  def sequence(nmax = nil, max_digits = MAX_DIGITS)
    out = []
    n = 0
    loop {
      break if nmax && n > nmax

      v = a(n)
      break if v.to_s.size > max_digits

      out << [n, v]
      n += 1
    }
    out
  end

  # k <= 6*10^7 の総当たり走査で確定した項
  KNOWN = {
    0 => 2, 1 => 0, 2 => 9, 3 => 81, 4 => 297, 5 => 1089, 6 => 1881,
    7 => 12393, 8 => 5049, 9 => 18513, 10 => 16929, 11 => 88209,
    12 => 31977, 13 => 1185921, 14 => 152361, 15 => 166617, 16 => 207009,
    17 => 7144929, 18 => 543609, 20 => 287793, 21 => 1499553,
    22 => 12341241, 23 => 2832489, 24 => 1055241, 25 => 20160657,
    27 => 6683193
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

  # x^2 + 8*y^2 = k となる非負整数解の個数
  def count_solutions(k)
    return 1 if k.zero?

    cnt = 0
    y = 0
    while 8 * y * y <= k
      r = k - 8 * y * y
      s = Integer.sqrt(r)
      cnt += 1 if s * s == r
      y += 1
    end
    cnt
  end

  def verify(limit = 5_000_000, nmax = 40)
    warn "brute force scan up to k = #{limit} ..."
    cnt = Array.new(limit + 1, 0)
    y = 0
    while 8 * y * y <= limit
      x = 0
      while x * x + 8 * y * y <= limit
        cnt[x * x + 8 * y * y] += 1
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
    A399267.check_known
  elsif ARGV.include?('--verify')
    nums = ARGV.select { |s| s =~ /\A\d+\z/ }.map(&:to_i)
    A399267.verify(nums[0] || 5_000_000, nums[1] || 40)
  else
    dig = ARGV.find { |t| t =~ /\A--digits=(\d+)\z/ } ? Regexp.last_match(1).to_i
                                                      : A399267::MAX_DIGITS
    cap = ARGV.find { |t| t =~ /\A\d+\z/ }&.to_i
    rows = A399267.sequence(cap, dig)
    rows.each { |n, v| puts "#{n} #{v}" }
    last = rows.last
    if last && (cap.nil? || last[0] < cap)
      nxt = last[0] + 1
      warn "stopped: a(#{nxt}) has #{A399267.a(nxt).to_s.size} digits (> #{dig})"
    end
  end
end
