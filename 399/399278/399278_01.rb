#!/usr/bin/env ruby
# frozen_string_literal: true
#
# A399278
#   a(n) is the smallest nonnegative integer k where there are exactly n
#   nonnegative integer solutions to x^2 + 6*y^2 = k.
#   （A399215 の非負解版）
#
# ------------------------------------------------------------------
# 理論（判別式 -24、類数 2、ただし 6 は idoneal number）
# ------------------------------------------------------------------
#   判別式 -24 は基本判別式で Z[sqrt(-6)] は最大整環なので、導手による 2 進の
#   補正因子は現れない。類数は 2 で一意分解は壊れているが、種が 2 つあって
#   各種にちょうど 1 類なので（6 は idoneal number）、表現数は合同条件だけで
#   決まる。
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
#   が成り立つときに限り主イデアルになる。振り分けの総数は
#       D(k) = Π(e_i + 1) * Π(f_j + 1)
#   で、単数は ±1 だけだから符号込み整数解の総数は 2*D(k)（類条件が崩れれば 0）。
#
#   退化解は y=0（k が平方数）と x=0（k が 6 倍の平方数）で、いずれも非負では
#   1 個。両立しない（k = u^2 = 6v^2 は不可能）ので
#       A(k) := [k は平方数または 6 倍の平方数] ∈ {0, 1}
#   とおくと、正整数解 P(k) = (D(k) - A(k))/2 に退化解を足して
#       N(k) = P(k) + A(k) = (D(k) + A(k)) / 2
#
#   さらに A(k) = D(k) mod 2 が自動的に成り立つ:
#       D が奇 <=> 全 e_i, f_j が偶 => k = 2^s 3^t (平方数) で
#                 s+t 偶なら平方数、s+t 奇なら 6 倍の平方数（類条件より s+t は偶）
#       D が偶 <=> ある指数が奇 => k は平方数でも 6 倍平方数でもない
#   よって
#       N(k) = ceil( D(k) / 2 )
#   （k <= 200000 で総当たりと完全一致）
#
# ------------------------------------------------------------------
# 最小化
# ------------------------------------------------------------------
#   N(k) = n (n >= 2) となるのは D(k) ∈ {2n-1, 2n} のときだけ。惰性素数の
#   偶数乗は D も類条件も変えず値を大きくするだけなので現れない。
#
#     (A) D = 2n（偶）: ある指数が奇なので A(k) = 0 は自動。
#         c := Σ_j f_j mod 2 とおくと、類条件を満たす最小の分岐素数部分は
#             mu(c) = 1 (c = 0),  2 (c = 1)
#         （c が奇なら 2 か 3 を奇数回補う必要があり、最小は 2 を 1 個）。
#             候補 = min over 指数配置 of  mu(c) * Π p^e
#     (B) D = 2n-1（奇）: 全指数が偶なので c も偶、s = t = 0 でよく類条件は
#         自動的に成立する。平方数候補は分岐素数を必要とせず、6 倍平方数候補
#         （同じ形の 6 倍）より常に小さい。
#             候補 = 分解素数だけからなる最小の平方数
#
#   よって候補は 2 個だけ。
#
#   枝刈りについて:
#     (B) は制約がないので「小さい素数ほど大きい指数」としてよい。
#     (A) は mu(c) が絡み、型をまたぐ指数の交換が c の偶奇を壊しうるので
#     全体での非増加は使えない。同じ型の 2 素数の間の交換は D も c も変えない
#     ので「型ごとに、小さい素数ほど大きい指数」としてよい。
#
#   例外:
#     a(0) = 2  ... k = 2 は s+t+Σf = 1 で類条件を満たさず解を持たない。その最小。
#     a(1) = 0  ... k = 0 は (0, 0) の 1 解を持ち、これが最小。
#                   （公式側は D = 1 の k = 1 を返すが 0 のほうが小さい）
#
#   姉妹列との関係:
#     A399215（正の組、P = (D-A)/2）は D ∈ {2n, 2n+1} を見るのに対し、
#     こちらは D ∈ {2n-1, 2n}。添字が 1 つずれるだけで構造は同じ。
#
# 使い方:
#   ruby 399278_01.rb           # a(n) が 1000 桁を超える手前まで出力
#   ruby 399278_01.rb 30        # n = 0..30 を出力（桁数制限も併用）
#   ruby 399278_01.rb --digits=200   # 桁数の上限を変更
#   ruby 399278_01.rb --known   # 総当たりで確定済みの項と照合
#   ruby 399278_01.rb --verify  # 総当たりと突き合わせて検証

module A399278
  module_function

  # 出力する a(n) の桁数上限。ちょうどこの桁数までは出力し、
  # 超えた時点で打ち切る（1000 桁は OK、1001 桁で停止）。
  MAX_DIGITS = 1000
  INF = Float::INFINITY

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

  # 分解素数（両型）を小さい順に。[素数, 非主類か] の組で返す。
  def split_primes(count)
    qs = primes_mod24(NONPRINCIPAL_RES, count).map { |p| [p, true] }
    ps = primes_mod24(PRINCIPAL_RES, count).map { |p| [p, false] }
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

  # ---------- (A) 非退化候補: D = d、コストは mu(c) * Π p^e ----------

  # 状態 = [残りの D, 素数の添字, 非主類の指数上限, 主類の指数上限, c の偶奇]
  # をメモ化する。使われる素数は各型の先頭からの連続部分になるので、
  # 各型 log2(D)+1 個で足りる。
  def min_nondegenerate(d)
    return 1 if d == 1

    primes = split_primes(Math.log2(d).floor + 1)
    memo = {}
    v = nd_rec(d, 0, INF, INF, 0, primes, memo)
    v == INF ? nil : v
  end

  # rem == 1 で mu(c) を確定させるので、戻り値は mu 込みの総コスト。
  def nd_rec(rem, idx, max_q, max_p, cpar, primes, memo)
    return (cpar.zero? ? 1 : 2) if rem == 1
    return INF if idx >= primes.size

    key = [rem, idx, max_q, max_p, cpar]
    cached = memo[key]
    return cached if cached

    p, nonprincipal = primes[idx]
    best = nd_rec(rem, idx + 1, max_q, max_p, cpar, primes, memo) # 使わない

    lim = nonprincipal ? max_q : max_p
    divisors_from(rem, 2).each { |dv|
      e = dv - 1
      next if e > lim

      rest = if nonprincipal
               nd_rec(rem / dv, idx + 1, e, max_p, (cpar + e) % 2, primes, memo)
             else
               nd_rec(rem / dv, idx + 1, max_q, e, cpar, primes, memo)
             end
      next if rest == INF

      cand = p**e * rest
      best = cand if cand < best
    }
    memo[key] = best
  end

  # ---------- (B) 退化候補: D = d（奇数）となる最小の平方数 ----------

  # 制約がないので全体で非増加としてよい
  def min_degenerate(d, primes = nil, idx = 0, max_exp = INF, memo = {})
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
    return 2 if n.zero? # 類条件を満たさない最小の k
    return 0 if n == 1  # k = 0 -> (0, 0)

    [min_nondegenerate(2 * n), min_degenerate(2 * n - 1)].compact.min
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
    0 => 2, 1 => 0, 2 => 25, 3 => 175, 4 => 385, 5 => 1225, 6 => 2695,
    7 => 109375, 8 => 9625, 9 => 21175, 10 => 96250, 11 => 765625,
    12 => 55825, 13 => 1500625, 14 => 148225, 15 => 529375, 16 => 298375,
    18 => 390775, 20 => 1395625, 21 => 13234375, 23 => 3705625,
    24 => 1730575, 25 => 33013750, 27 => 4594975
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

  # x^2 + 6*y^2 = k となる非負整数解の個数
  def count_solutions(k)
    return 1 if k.zero?

    cnt = 0
    y = 0
    while 6 * y * y <= k
      r = k - 6 * y * y
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
    while 6 * y * y <= limit
      x = 0
      while x * x + 6 * y * y <= limit
        cnt[x * x + 6 * y * y] += 1
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
    A399278.check_known
  elsif ARGV.include?('--verify')
    nums = ARGV.select { |s| s =~ /\A\d+\z/ }.map(&:to_i)
    A399278.verify(nums[0] || 5_000_000, nums[1] || 40)
  else
    dig = ARGV.find { |t| t =~ /\A--digits=(\d+)\z/ } ? Regexp.last_match(1).to_i
                                                      : A399278::MAX_DIGITS
    cap = ARGV.find { |t| t =~ /\A\d+\z/ }&.to_i
    rows = A399278.sequence(cap, dig)
    rows.each { |n, v| puts "#{n} #{v}" }
    last = rows.last
    if last && (cap.nil? || last[0] < cap)
      nxt = last[0] + 1
      warn "stopped: a(#{nxt}) has #{A399278.a(nxt).to_s.size} digits (> #{dig})"
    end
  end
end
