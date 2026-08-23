#!/usr/bin/env ruby
# frozen_string_literal: true
#
# A399215
#   a(n) is the smallest nonnegative integer k where exactly n pairs of
#   positive integers (x, y) exist such that x^2 + 6*y^2 = k.
#
# ------------------------------------------------------------------
# 理論（判別式 -24、類数 2、ただし 6 は idoneal number）
# ------------------------------------------------------------------
#   判別式 -24 は基本判別式で Z[sqrt(-6)] は最大整環なので、導手による
#   2 進の補正因子は現れない。類数は 2 で一意分解は壊れているが、種が 2 つ
#   あって各種にちょうど 1 類なので（6 は idoneal number）、表現数は
#   合同条件だけで決まる。
#
#   k = 2^s * 3^t * Π p_i^{e_i} * Π q_j^{f_j} * Π r_l^{2 g_l}
#       p_i ≡ 1, 7  (mod 24) : 分解、主類          7, 31, 73, 79, 97, ...
#       q_j ≡ 5, 11 (mod 24) : 分解、非主類        5, 11, 29, 53, 59, ...
#       r_l ≡ 13, 17, 19, 23 (mod 24) : 惰性（奇数乗があれば表現不可）
#       2, 3 : 分岐、どちらも非主類
#
#   非主類を C とすると類群の位数が 2 なので C = C^{-1}。よって非主類素数 q の
#   指数 f を共役素イデアルへどう振り分けても、その類は i によらず C^f になる。
#   ゆえにノルム k のイデアルはすべて同じ類 C^{s+t+Σf_j} に属し、
#
#       類条件:  s + t + Σ_j f_j ≡ 0 (mod 2)
#
#   が成り立つときに限り、それらがすべて主イデアルになる。振り分けの総数は
#
#       D(k) = Π(e_i + 1) * Π(f_j + 1)
#
#   で、単数は ±1 だけだから符号込み整数解の総数は 2*D(k)（類条件が崩れれば 0）。
#
#   退化解は y=0（k が平方数）と x=0（k が 6 倍の平方数）で、いずれも符号込み
#   2 個。両立しない（k = u^2 = 6v^2 は不可能）ので
#       A(k) := [k は平方数または 6 倍の平方数] ∈ {0, 1}
#   とおくと
#       R(k) = (2*D(k) - 2*A(k)) / 4 = (D(k) - A(k)) / 2
#   （k <= 300000 で総当たりと完全一致）
#
#   したがって R(k) = n の候補は 2 種類だけ:
#     非退化候補: A(k) = 0 かつ D(k) = 2n
#     退化候補  : A(k) = 1 かつ D(k) = 2n+1
#
# ------------------------------------------------------------------
# 最小候補の形
# ------------------------------------------------------------------
#   惰性素数の偶数乗は D も類条件も変えず値を大きくするだけなので現れない。
#
#   非退化候補: D = 2n は偶数なので分解素数の指数に奇数のものがあり、
#     平方数にも 6 倍平方数にもならない（A(k)=0 は自動）。c := Σ_j f_j mod 2
#     とおくと、類条件を満たす最小の分岐素数部分は
#         mu(c) = 1 (c = 0),  2 (c = 1)
#     （c が奇なら 2 か 3 を奇数回補う必要があり、最小は 2 を 1 個）。よって
#         最小値 = min over 指数配置 of  mu(c) * Π p^e
#
#   退化候補: D = 2n+1 は奇数なので全指数が偶数、c も偶数で類条件は自動的に
#     成立する。平方数候補は分岐素数を必要とせず、6 倍平方数候補（同じ形の
#     6 倍）より常に小さい。よって分解素数だけからなる最小の平方数でよい。
#
#   枝刈りについて:
#     退化候補は制約がないので「小さい素数ほど大きい指数」としてよい。
#     非退化候補は mu(c) が絡むため、型をまたぐ指数の交換が c の偶奇を
#     壊しうるので全体での非増加は仮定できない。一方、同じ型の 2 素数の間で
#     指数を交換しても D も c も変わらないので
#         「型ごとに、小さい素数ほど大きい指数」
#     としてよい。同じ型の小さい素数を飛ばす理由もない。
#
# ------------------------------------------------------------------
# 系（別文書で証明）
#   5 | a(n)、ただし a(1) = 7 のみ例外。
# ------------------------------------------------------------------
#
# 使い方:
#   ruby 399215_01.rb           # n = 0..100 を出力
#   ruby 399215_01.rb 30        # n = 0..30 を出力
#   ruby 399215_01.rb --known   # 総当たりで確定済みの項と照合
#   ruby 399215_01.rb --verify  # 総当たりと突き合わせて検証

module A399215
  module_function

  PRINCIPAL_RES    = [1, 7].freeze   # 主類（分解）
  NONPRINCIPAL_RES = [5, 11].freeze  # 非主類（分解）

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

  # 剰余集合 res (mod 24) の素数を小さい順に count 個
  def primes_mod24(res, count)
    ps = []
    n = 5
    while ps.size < count
      ps << n if res.include?(n % 24) && prime?(n)
      n += 2
    end
    ps
  end

  # 分解素数（両型）を小さい順に count 個。[素数, 非主類か] の組で返す。
  def split_primes(count)
    qs = primes_mod24(NONPRINCIPAL_RES, count)
    ps = primes_mod24(PRINCIPAL_RES, count)
    ((qs.map { |p| [p, true] }) + (ps.map { |p| [p, false] })).sort_by(&:first)
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

  # ---------- 非退化候補: D = d、コストは mu(c) * Π p^e ----------

  def min_nondegenerate(d)
    return 1 if d == 1

    primes = split_primes(Math.log2(d).floor + 1)
    best = nil
    rec = lambda { |rem, idx, cur, max_q, max_p, cpar|
      cost = cur * (cpar.zero? ? 1 : 2)
      return if best && cost >= best
      if rem == 1
        best = cost if best.nil? || cost < best
        return
      end
      return if idx >= primes.size

      p, nonprincipal = primes[idx]
      return if best && p >= best

      rec.call(rem, idx + 1, cur, max_q, max_p, cpar) # この素数を使わない

      lim = nonprincipal ? max_q : max_p
      divisors_from(rem, 2).each { |dv|
        e = dv - 1
        next if e > lim

        v = cur * p**e
        next if best && v >= best

        if nonprincipal
          rec.call(rem / dv, idx + 1, v, e, max_p, (cpar + e) % 2)
        else
          rec.call(rem / dv, idx + 1, v, max_q, e, cpar)
        end
      }
    }
    rec.call(d, 0, 1, Float::INFINITY, Float::INFINITY, 0)
    best
  end

  # ---------- 退化候補: D = d（奇数）となる最小の平方数 ----------

  # 制約がないので全体で非増加としてよい
  def min_degenerate(d, primes = nil, idx = 0, max_exp = Float::INFINITY, memo = {})
    return 1 if d == 1

    primes ||= split_primes(Math.log2(d).floor + 1).map(&:first)
    key = [d, idx, max_exp]
    cached = memo[key]
    return cached if cached
    return nil if idx >= primes.size

    best = nil
    divisors_from(d, 2).each { |dv|
      e = dv - 1
      next if e > max_exp

      rest = min_degenerate(d / dv, primes, idx + 1, e, memo)
      next if rest.nil?

      cand = primes[idx]**e * rest
      best = cand if best.nil? || cand < best
    }
    memo[key] = best
  end

  # ---------- 本体 ----------

  def a(n)
    return 0 if n.zero?

    [min_nondegenerate(2 * n), min_degenerate(2 * n + 1)].compact.min
  end

  def sequence(nmax)
    (0..nmax).map { |n| a(n) }
  end

  # k <= 6*10^7 の総当たり走査で確定した項
  KNOWN = {
    0 => 0, 1 => 7, 2 => 55, 3 => 175, 4 => 385, 5 => 4375, 6 => 2695,
    7 => 30625, 8 => 9625, 9 => 21175, 10 => 96250, 12 => 55825,
    13 => 148225, 14 => 2406250, 15 => 529375, 16 => 298375,
    17 => 37515625, 18 => 390775, 20 => 1395625, 21 => 13234375,
    22 => 3705625, 24 => 1730575, 25 => 33013750, 27 => 4594975
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

  # x^2 + 6*y^2 = k となる正整数の組の個数
  def count_pairs(k)
    cnt = 0
    y = 1
    while 6 * y * y < k
      r = k - 6 * y * y
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
    while 6 * y * y <= limit
      x = 1
      while x * x + 6 * y * y <= limit
        cnt[x * x + 6 * y * y] += 1
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
    A399215.check_known
  elsif ARGV.include?('--verify')
    nums = ARGV.select { |s| s =~ /\A\d+\z/ }.map(&:to_i)
    A399215.verify(nums[0] || 5_000_000, nums[1] || 40)
  else
    nmax = (ARGV[0] || 100).to_i
    A399215.sequence(nmax).each_with_index { |v, n| puts "#{n} #{v}" }
  end
end
