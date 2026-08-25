#!/usr/bin/env ruby
# frozen_string_literal: true
#
# A399271
#   a(n) is the smallest nonnegative integer k where there are exactly n
#   nonnegative integer solutions to x^2 + 9*y^2 = k.
#   （A399214 の非負解版）
#
# ------------------------------------------------------------------
# 理論（判別式 -36 だが、二平方和への初等的な帰着で処理する）
# ------------------------------------------------------------------
#   N(m)  := u^2 + v^2 = m の非負整数の順序対 (u, v) の個数（= A399202 の数え方）
#   N_9(k):= x^2 + 9y^2 = k の非負整数の順序対 (x, y) の個数
#
#   (a) 9 | k のとき: x^2+9y^2 ≡ x^2 (mod 3) なので 3 | x。x = 3u とおけば
#           u^2 + y^2 = k/9      よって  N_9(k) = N(k/9)
#       （k = 0 もここに含まれ、N_9(0) = N(0) = 1）
#   (b) k ≡ 1 (mod 3) のとき: u^2+v^2 = k の各解では u, v のちょうど一方だけが
#       3 で割れる（平方は法 3 で 0 か 1）。軸上解 (0, v) でも 0 は 3 で割れ、
#       v は割れないので同じ。特に u ≠ v（u = v なら 2u^2 ≡ 1 mod 3 となり
#       u^2 ≡ 2 mod 3 で不可能）なので座標交換は不動点を持たず、非負解は
#       2 個ずつの組になり、その一方だけが 3 で割れる第 2 座標を持つ。よって
#           N_9(k) = N(k) / 2
#   (c) それ以外: k ≡ 2 (mod 3) は x^2 ∈ {0,1} (mod 3) より不可能。
#       3 | k なら 3 | x となり 9 | k なので、3 | k かつ 9 ∤ k も不可能。
#
#   k <= 200000 で総当たりと完全一致。
#
# ------------------------------------------------------------------
# 二平方和の非負解の個数
# ------------------------------------------------------------------
#   m = 2^s * Π p_i^{e_i} * Π q_j^{h_j}   (p ≡ 1 mod 4, q ≡ 3 mod 4)
#   D(m) := Π(e_i + 1)（h_j に奇数があれば 0）、S(m) := [m は平方数] とすると
#       N(m) = D(m) + S(m)
#   （Jacobi より符号込みは 4D(m)、平方数のとき軸上に符号込み 4 解。
#     N = (4D - 4S)/4 + 2S = D + S）
#
#   よって N(m) = n の候補は「非平方数で D = n」「平方数で D = n-1」の 2 種類。
#   これを最小化したものが A399202 で
#       B(n) := A399202(n)
#             = min( minprod(n), minprod(n-1) )   (n 偶, n >= 2)
#             = 2 * minprod(n)                    (n 奇, n >= 3)
#       B(0) = 3,  B(1) = 0
#   minprod(D) := 4k+1 型素数 5, 13, 17, 29, ... で Π(e_i+1) = D となる最小数
#   （= A018782(D)）
#
# ------------------------------------------------------------------
# k ≡ 1 (mod 3) という制約
# ------------------------------------------------------------------
#   C(n) := k ≡ 1 (mod 3) かつ N(k) = 2n となる最小の k とおくと、n >= 1 で
#       a(n) = min( 9*B(n), C(n) )
#
#   4k+1 型素数を
#       P 型: p ≡ 1  (mod 12)   13, 37, 61, 73, 97, ...   （法 3 で 1）
#       Q 型: p ≡ 5  (mod 12)   5, 17, 29, 41, 53, ...    （法 3 で 2）
#   に分ける。k ≡ 1 (mod 3) より 3 ∤ k、3 以外の 3 (mod 4) 型素数は偶数乗で
#   法 3 では 1、2 は法 3 で 2 なので
#       k ≡ 2^{s + Σ_j f_j} (mod 3),   s = v_2(k), f_j = Q 型の指数
#   したがって条件は  s + Σ f_j ≡ 0 (mod 2)。s は D に効かないので、
#   Q 型指数和が奇数なら 2 を 1 個補えばよい（最安）。よって
#       非平方候補 (D = 2n)  : min over 指数配置 of  mu(c) * Π p^e,
#                              c = Σ_Q e mod 2,  mu(0)=1, mu(1)=2
#                              （D 偶 ⇒ 奇指数が存在 ⇒ 非平方は自動）
#       平方候補   (D = 2n-1): 全指数偶 ⇒ c 偶 ⇒ s = 0 でよく、条件は自動成立。
#                              minprod(2n-1)（奇数の平方数）
#
#   まとめると n >= 2 に対する候補は 3 つ:
#       9 * B(n),   minprod_cong(2n),   minprod(2n-1)
#
#   枝刈りについて:
#     平方候補は制約がないので「小さい素数ほど大きい指数」としてよい。
#     非平方候補は mu(c) が絡み、型をまたぐ指数の交換が c の偶奇を壊しうるので
#     全体での非増加は使えない。同じ型の 2 素数の間の交換は D も c も変えないので
#     「型ごとに、小さい素数ほど大きい指数」としてよい。
#
#   例外:
#     a(0) = 2  ... k ≡ 2 (mod 3) は解を持たない。その最小。
#     a(1) = 0  ... B(1) = 0 すなわち k = 0 が (0, 0) の 1 解を持つ。
#
# 使い方:
#   ruby 399271_01.rb           # a(n) が 1000 桁を超える手前まで出力
#   ruby 399271_01.rb 30        # n = 0..30 を出力（桁数制限も併用）
#   ruby 399271_01.rb --digits=200   # 桁数の上限を変更
#   ruby 399271_01.rb --known   # 総当たりで確定済みの項と照合
#   ruby 399271_01.rb --verify  # 総当たりと突き合わせて検証

module A399271
  module_function

  # 出力する a(n) の桁数上限。ちょうどこの桁数までは出力し、
  # 超えた時点で打ち切る（1000 桁は OK、1001 桁で停止）。
  MAX_DIGITS = 1000
  INF = Float::INFINITY

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

  # p ≡ res (mod 12) な素数を小さい順に count 個
  def primes_mod12(res, count)
    ps = []
    n = 5
    while ps.size < count
      ps << n if n % 12 == res && prime?(n)
      n += 2
    end
    ps
  end

  # p ≡ 1 (mod 4) な素数を小さい順に count 個
  def primes_1mod4(count)
    ps = []
    n = 5
    while ps.size < count
      ps << n if n % 4 == 1 && prime?(n)
      n += 2
    end
    ps
  end

  # [素数, Q 型か] を小さい順に。各型 count 個ずつ用意して統合する。
  def typed_primes(count)
    qs = primes_mod12(5, count).map { |p| [p, true] }
    ps = primes_mod12(1, count).map { |p| [p, false] }
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

  # ---------- minprod（= A018782） ----------

  def minprod(d, primes = nil, idx = 0, max_exp = INF, memo = {})
    return 1 if d == 1

    primes ||= primes_1mod4(Math.log2(d).floor + 1)
    key = [d, idx, max_exp]
    cached = memo[key]
    return cached if cached
    return nil if idx >= primes.size

    best = nil
    divisors_from(d, 2).each { |dv|
      e = dv - 1
      next if e > max_exp

      rest = minprod(d / dv, primes, idx + 1, e, memo)
      next if rest.nil?

      cand = primes[idx]**e * rest
      best = cand if best.nil? || cand < best
    }
    memo[key] = best
  end

  # ---------- 合同条件つき（コストは mu(c) * Π p^e） ----------

  # 状態 = [残りの D, 素数の添字, Q 型の指数上限, P 型の指数上限, Q 型指数和の偶奇]
  # をメモ化する。使われる素数は各型の先頭からの連続部分になるので、
  # 各型 log2(D)+1 個で足りる。
  def minprod_cong(d)
    return 1 if d == 1

    primes = typed_primes(Math.log2(d).floor + 1)
    memo = {}
    v = cong_rec(d, 0, INF, INF, 0, primes, memo)
    v == INF ? nil : v
  end

  # 戻り値は mu(c) を掛ける前の Π p^e ではなく、最後に mu を掛けた総コスト。
  # 途中の分岐では c の偶奇だけを持ち回り、rem == 1 で mu を確定させる。
  def cong_rec(rem, idx, max_q, max_p, qpar, primes, memo)
    return (qpar.zero? ? 1 : 2) if rem == 1
    return INF if idx >= primes.size

    key = [rem, idx, max_q, max_p, qpar]
    cached = memo[key]
    return cached if cached

    p, q_type = primes[idx]
    best = cong_rec(rem, idx + 1, max_q, max_p, qpar, primes, memo) # 使わない

    lim = q_type ? max_q : max_p
    divisors_from(rem, 2).each { |dv|
      e = dv - 1
      next if e > lim

      rest = if q_type
               cong_rec(rem / dv, idx + 1, e, max_p, (qpar + e) % 2, primes, memo)
             else
               cong_rec(rem / dv, idx + 1, max_q, e, qpar, primes, memo)
             end
      next if rest == INF

      cand = p**e * rest
      best = cand if cand < best
    }
    memo[key] = best
  end

  # ---------- 本体 ----------

  # B(n) = A399202(n): N(m) = n となる最小の非負整数 m
  def b(n)
    return 3 if n.zero? # 二平方和で表せない最小の非負整数
    return 0 if n == 1  # m = 0 の (0, 0)

    if n.even?
      [minprod(n), minprod(n - 1)].compact.min
    else
      w = minprod(n)
      w && 2 * w
    end
  end

  def a(n)
    return 2 if n.zero? # k ≡ 2 (mod 3) の最小
    return 0 if n == 1  # B(1) = 0

    cands = [9 * b(n)]
    v = minprod_cong(2 * n)   # 非平方候補（合同条件つき、mu 込み）
    cands << v if v
    v = minprod(2 * n - 1)    # 平方候補（条件は自動成立）
    cands << v if v
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
    0 => 2, 1 => 0, 2 => 9, 3 => 325, 4 => 225, 5 => 4225, 6 => 2925,
    7 => 203125, 8 => 9945, 9 => 76050, 10 => 38025, 11 => 2640625,
    12 => 49725, 13 => 17850625, 14 => 1221025, 15 => 1901250,
    16 => 248625, 18 => 646425, 20 => 1243125, 21 => 47531250,
    22 => 23765625, 23 => 30525625, 24 => 1442025, 27 => 21978450
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

  # x^2 + 9*y^2 = k となる非負整数解の個数
  def count_solutions(k)
    return 1 if k.zero?

    cnt = 0
    y = 0
    while 9 * y * y <= k
      r = k - 9 * y * y
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
    while 9 * y * y <= limit
      x = 0
      while x * x + 9 * y * y <= limit
        cnt[x * x + 9 * y * y] += 1
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
    A399271.check_known
  elsif ARGV.include?('--verify')
    nums = ARGV.select { |s| s =~ /\A\d+\z/ }.map(&:to_i)
    A399271.verify(nums[0] || 5_000_000, nums[1] || 40)
  else
    dig = ARGV.find { |t| t =~ /\A--digits=(\d+)\z/ } ? Regexp.last_match(1).to_i
                                                      : A399271::MAX_DIGITS
    cap = ARGV.find { |t| t =~ /\A\d+\z/ }&.to_i
    rows = A399271.sequence(cap, dig)
    rows.each { |n, v| puts "#{n} #{v}" }
    last = rows.last
    if last && (cap.nil? || last[0] < cap)
      nxt = last[0] + 1
      warn "stopped: a(#{nxt}) has #{A399271.a(nxt).to_s.size} digits (> #{dig})"
    end
  end
end
