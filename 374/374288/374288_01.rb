#!/usr/bin/env ruby
# frozen_string_literal: true
#
# A374288
#   a(n) is the smallest nonnegative integer k where there are exactly n
#   nonnegative integer solutions to x^2 + 5*y^2 = k.
#
# ------------------------------------------------------------------
# 理論（判別式 -20、類数 2、ただし 5 は idoneal number）
# ------------------------------------------------------------------
#   判別式 -20 は基本判別式で Z[sqrt(-5)] は最大整環なので、導手による補正因子は
#   現れない。類数は 2 で一意分解は壊れているが、種が 2 つあって各種にちょうど
#   1 類なので（5 は idoneal number）、表現数は合同条件だけで決まる。
#
#   k = 2^a * 5^c * Π p_i^{e_i} * Π q_j^{f_j} * Π r_l^{2 g_l}
#       p_i ≡ 1, 9 (mod 20) : 分解、主類          29, 41, 61, 89, 101, 109, ...
#       q_j ≡ 3, 7 (mod 20) : 分解、非主類        3, 7, 23, 43, 47, 67, ...
#       r_l ≡ 11, 13, 17, 19 (mod 20) : 惰性（奇数乗があれば表現不可）
#       2 : 分岐、非主類（2 = 2x^2+2xy+3y^2 の x=1, y=0）
#       5 : 分岐、主類    （5 = 0^2 + 5*1^2）
#
#   非主類を C とすると類群の位数が 2 なので C = C^{-1}。よって非主類素数 q の
#   指数 f を共役素イデアルへどう振り分けても、その類は i によらず C^f になる。
#   ゆえにノルム k のイデアルはすべて同じ類 C^{a + Σ f_j} に属し、
#
#       類条件:  a + Σ_j f_j ≡ 0 (mod 2)      （a = v_2(k)）
#
#   が成り立つときに限り主イデアルになる。5 の指数 c は主類なので効かない。
#   振り分けの総数は
#       M(k) = Π(e_i + 1) * Π(f_j + 1)
#   で、単数は ±1 だけだから符号込み整数解の総数は 2*M(k)（類条件が崩れれば 0）。
#
#   退化解は y=0（k が平方数）と x=0（k が 5 倍の平方数）で、いずれも非負では
#   1 個。両立しない（k = u^2 = 5v^2 は不可能）ので
#       A(k) := [k は平方数または 5 倍の平方数] ∈ {0, 1}
#   とおくと、正整数解 P(k) = (M(k) - A(k))/2 に退化解を足して
#       N(k) = P(k) + A(k) = (M(k) + A(k)) / 2
#
#   さらに A(k) = M(k) mod 2 が自動的に成り立つ:
#       M が奇 <=> 全 e_i, f_j が偶 => Σf_j 偶 => 類条件より a も偶
#                 => k = 2^a 5^c (平方数) で、c 偶なら平方数、c 奇なら 5 倍平方数
#       M が偶 <=> ある指数が奇 => k は平方数でも 5 倍平方数でもない
#   よって
#       N(k) = ceil( M(k) / 2 )
#   （k <= 200000 で総当たりと完全一致）
#
# ------------------------------------------------------------------
# 最小化
# ------------------------------------------------------------------
#   N(k) = n (n >= 2) となるのは M(k) ∈ {2n-1, 2n} のときだけ。惰性素数の
#   偶数乗と 5 の冪は M も類条件も変えず値を大きくするだけなので現れない。
#
#     (A) M = 2n（偶）: ある指数が奇なので A(k) = 0 は自動。
#         c := Σ_j f_j mod 2 とおくと、類条件を満たす最小の 2 冪は
#             mu(c) = 1 (c = 0),  2 (c = 1)
#             候補 = min over 指数配置 of  mu(c) * Π p^e
#     (B) M = 2n-1（奇）: 全指数が偶なので c も偶、a = 0 でよく類条件は自動成立。
#         5 を含まない平方数候補が 5 倍平方数候補より小さい。
#             候補 = 分解素数だけからなる最小の平方数
#
#   よって候補は 2 個だけ。
#
#   枝刈りについて:
#     (B) は制約がないので「小さい素数ほど大きい指数」としてよい。
#     (A) は mu(c) が絡み、型をまたぐ指数の交換が c の偶奇を壊しうるので
#     全体での非増加は使えない。同じ型の 2 素数の間の交換は M も c も変えない
#     ので「型ごとに、小さい素数ほど大きい指数」としてよい。
#
#   例外:
#     a(0) = 2  ... k = 2 は a + Σf = 1 で類条件を満たさず解を持たない。その最小。
#     a(1) = 0  ... k = 0 は (0, 0) の 1 解を持ち、これが最小。
#                   （公式側は M = 1 の k = 1 を返すが 0 のほうが小さい）
#
#   姉妹形との関係:
#     x^2+6y^2 の非負版 A399278 とまったく同じ構造。違うのは分解素数の集合
#     （mod 20 と mod 24）と、分岐素数のうちどちらが非主類かだけである。
#
# 使い方:
#   ruby 374288_01.rb           # a(n) が 1000 桁を超える手前まで出力
#   ruby 374288_01.rb 30        # n = 0..30 を出力（桁数制限も併用）
#   ruby 374288_01.rb --digits=200   # 桁数の上限を変更
#   ruby 374288_01.rb --known   # 総当たりで確定済みの項と照合
#   ruby 374288_01.rb --verify  # 総当たりと突き合わせて検証

module A374288
  module_function

  # 出力する a(n) の桁数上限。ちょうどこの桁数までは出力し、
  # 超えた時点で打ち切る（1000 桁は OK、1001 桁で停止）。
  MAX_DIGITS = 1000
  INF = Float::INFINITY

  PRINCIPAL_RES    = [1, 9].freeze   # 主類（分解）
  NONPRINCIPAL_RES = [3, 7].freeze   # 非主類（分解）

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

  # 剰余集合 res (mod 20) の素数を小さい順に count 個
  def primes_mod20(res, count)
    ps = []
    n = 3
    while ps.size < count
      ps << n if res.include?(n % 20) && prime?(n)
      n += 2
    end
    ps
  end

  # 分解素数（両型）を小さい順に。[素数, 非主類か] の組で返す。
  def split_primes(count)
    qs = primes_mod20(NONPRINCIPAL_RES, count).map { |p| [p, true] }
    ps = primes_mod20(PRINCIPAL_RES, count).map { |p| [p, false] }
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

  # ---------- (A) 非退化候補: M = d、コストは mu(c) * Π p^e ----------

  # 状態 = [残りの M, 素数の添字, 非主類の指数上限, 主類の指数上限, c の偶奇]
  # をメモ化する。使われる素数は各型の先頭からの連続部分になるので、
  # 各型 log2(M)+1 個で足りる。
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

  # ---------- (B) 退化候補: M = d（奇数）となる最小の平方数 ----------

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
    0 => 2, 1 => 0, 2 => 9, 3 => 81, 4 => 189, 5 => 441, 6 => 1449,
    7 => 10206, 8 => 3969, 9 => 12789, 10 => 13041, 11 => 35721,
    12 => 30429, 13 => 194481, 14 => 117369, 15 => 115101, 16 => 186921,
    17 => 2893401, 18 => 273861, 20 => 378189, 21 => 1035909,
    22 => 9506889, 23 => 2099601, 24 => 882441, 25 => 5639949,
    27 => 6765381, 28 => 3403701
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

  # x^2 + 5*y^2 = k となる非負整数解の個数
  def count_solutions(k)
    return 1 if k.zero?

    cnt = 0
    y = 0
    while 5 * y * y <= k
      r = k - 5 * y * y
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
    while 5 * y * y <= limit
      x = 0
      while x * x + 5 * y * y <= limit
        cnt[x * x + 5 * y * y] += 1
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
    A374288.check_known
  elsif ARGV.include?('--verify')
    nums = ARGV.select { |s| s =~ /\A\d+\z/ }.map(&:to_i)
    A374288.verify(nums[0] || 5_000_000, nums[1] || 40)
  else
    dig = ARGV.find { |t| t =~ /\A--digits=(\d+)\z/ } ? Regexp.last_match(1).to_i
                                                      : A374288::MAX_DIGITS
    cap = ARGV.find { |t| t =~ /\A\d+\z/ }&.to_i
    rows = A374288.sequence(cap, dig)
    rows.each { |n, v| puts "#{n} #{v}" }
    last = rows.last
    if last && (cap.nil? || last[0] < cap)
      nxt = last[0] + 1
      warn "stopped: a(#{nxt}) has #{A374288.a(nxt).to_s.size} digits (> #{dig})"
    end
  end
end
