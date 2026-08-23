#!/usr/bin/env ruby
# frozen_string_literal: true
#
# A399214
#   a(n) is the smallest nonnegative integer k where exactly n pairs of
#   positive integers (x, y) exist such that x^2 + 9*y^2 = k.
#
# ------------------------------------------------------------------
# 理論（判別式 -36 だが、二平方和への初等的な帰着で処理する）
# ------------------------------------------------------------------
#   T(m) := u^2 + v^2 = m の正整数の順序対 (u, v) の個数（= A328151 の数え方）
#   R_9(k) := x^2 + 9y^2 = k の正整数の順序対 (x, y) の個数
#
#   (a) 9 | k のとき: x^2+9y^2 ≡ x^2 (mod 3) なので 3 | x。x = 3u とおけば
#           u^2 + y^2 = k/9      よって  R_9(k) = T(k/9)
#   (b) k ≡ 1 (mod 3) のとき: u^2+v^2 = k の各解では u, v のちょうど一方だけが
#       3 で割れる（平方は法 3 で 0 か 1）。特に u ≠ v なので座標交換は不動点を
#       持たず、解は 2 個ずつの組になり、その一方だけが 3 で割れる第 2 座標を
#       持つ。よって  R_9(k) = T(k)/2
#   (c) それ以外: k ≡ 2 (mod 3) は x^2 ∈ {0,1} (mod 3) より不可能。
#       3 | k なら 3 | x となり 9 | k なので、3 | k かつ 9 ∤ k も不可能。
#
#   したがって n >= 1 について
#       a(n) = min( 9*B(n), C(n) )
#   ここで
#       B(n) := T(m) = n となる最小の正整数 m          （= A328151(n)）
#       C(n) := m ≡ 1 (mod 3) かつ T(m) = 2n となる最小の m
#   どちらも空でない（T(2*5^(n-1)) = n、T(2*5^(2n-1)) = 2n かつ ≡ 1 mod 3）。
#
# ------------------------------------------------------------------
# 二平方和の数え上げ
# ------------------------------------------------------------------
#   m = 2^s * Π p_i^{e_i} * Π q_j^{h_j}   (p ≡ 1 mod 4, q ≡ 3 mod 4)
#   D(m) := Π(e_i + 1)（h_j に奇数があれば 0）、S(m) := [m は平方数]
#   Jacobi の二平方定理より符号込み整数解は 4*D(m) 個、平方数のときだけ
#   軸上解が 4 個あるので
#       T(m) = D(m) - S(m)
#   よって T(m) = n の候補は「非平方数で D = n」「平方数で D = n+1」の 2 種類。
#
#   B(n) は制約なしなので A328151 と同じ形になる:
#       n 偶: min( minprod(n), minprod(n+1) )
#       n 奇: 2*minprod(n)     （D 奇 ⇒ 全指数偶 ⇒ 平方数なので 2 倍で非平方化）
#   minprod(D) := 4k+1 型素数 5, 13, 17, 29, ... で Π(e_i+1) = D となる最小数
#   （= A018782(D)）
#
# ------------------------------------------------------------------
# C(n): m ≡ 1 (mod 3) という制約
# ------------------------------------------------------------------
#   4k+1 型素数を
#       P 型: p ≡ 1  (mod 12)   13, 37, 61, 73, 97, ...   （法 3 で 1）
#       Q 型: q ≡ 5  (mod 12)   5, 17, 29, 41, 53, ...    （法 3 で 2）
#   に分ける。m ≡ 1 (mod 3) より 3 ∤ m、3 以外の 3 (mod 4) 型素数は偶数乗で
#   法 3 では 1、2 は法 3 で 2 なので
#       m ≡ 2^{s + Σ_j f_j} (mod 3),   s = v_2(m), f_j = Q 型の指数
#   したがって条件は  s + Σ f_j ≡ 0 (mod 2)。最小の 2 進部分は Q 型指数和が
#   偶数なら 1、奇数なら 2 である。よって
#       非平方候補 (D = 2n)  : min over 指数配置 of  mu(c) * Π p^e,
#                              c = Σ_Q e mod 2,  mu(0)=1, mu(1)=2
#                              （D 偶 ⇒ 奇指数が存在 ⇒ 非平方は自動）
#       平方候補   (D = 2n+1): 全指数偶 ⇒ c 偶 ⇒ 条件は自動成立。
#                              minprod(2n+1) そのもの
#
#   枝刈り: 非平方候補は mu(c) が絡むため、型をまたぐ指数交換が c の偶奇を
#   壊しうるので全体での非増加は使えない。同じ型の 2 素数の間の交換は D も c も
#   変えないので「型ごとに、小さい素数ほど大きい指数」としてよい。
#
# ------------------------------------------------------------------
# 系（別文書で証明）: すべての項が 5 の倍数（例外なし）。
# ------------------------------------------------------------------
#
# 使い方:
#   ruby 399214_01.rb           # n = 0..100 を出力
#   ruby 399214_01.rb 30        # n = 0..30 を出力
#   ruby 399214_01.rb --known   # 総当たりで確定済みの項と照合
#   ruby 399214_01.rb --verify  # 総当たりと突き合わせて検証

module A399214
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

  # [素数, Q 型か] を小さい順に count 個
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

  def minprod(d, primes = nil, idx = 0, max_exp = Float::INFINITY, memo = {})
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

  def min_cong(d)
    return 1 if d == 1

    primes = typed_primes(Math.log2(d).floor + 1)
    best = nil
    rec = lambda { |rem, idx, cur, max_q, max_p, cpar|
      cost = cur * (cpar.zero? ? 1 : 2)
      return if best && cost >= best
      if rem == 1
        best = cost if best.nil? || cost < best
        return
      end
      return if idx >= primes.size

      p, q_type = primes[idx]
      return if best && p >= best

      rec.call(rem, idx + 1, cur, max_q, max_p, cpar) # 使わない

      lim = q_type ? max_q : max_p
      divisors_from(rem, 2).each { |dv|
        e = dv - 1
        next if e > lim

        v = cur * p**e
        next if best && v >= best

        if q_type
          rec.call(rem / dv, idx + 1, v, e, max_p, (cpar + e) % 2)
        else
          rec.call(rem / dv, idx + 1, v, max_q, e, cpar)
        end
      }
    }
    rec.call(d, 0, 1, Float::INFINITY, Float::INFINITY, 0)
    best
  end

  # ---------- 本体 ----------

  # B(n) = A328151(n): T(m) = n となる最小の正整数 m
  def b(n)
    if n.even?
      [minprod(n), minprod(n + 1)].compact.min
    else
      2 * minprod(n)
    end
  end

  # C(n): m ≡ 1 (mod 3) かつ T(m) = 2n となる最小の m
  def c(n)
    [min_cong(2 * n), minprod(2 * n + 1)].compact.min
  end

  def a(n)
    return 0 if n.zero?

    [9 * b(n), c(n)].min
  end

  def sequence(nmax)
    (0..nmax).map { |n| a(n) }
  end

  # k <= 6*10^7 の総当たり走査で確定した項
  KNOWN = {
    0 => 0, 1 => 10, 2 => 45, 3 => 325, 4 => 585, 5 => 8125, 6 => 2925,
    7 => 105625, 8 => 9945, 9 => 76050, 10 => 73125, 12 => 49725,
    13 => 1221025, 14 => 950625, 15 => 1901250, 16 => 248625,
    18 => 646425, 20 => 1243125, 21 => 47531250, 22 => 30525625,
    24 => 1442025, 26 => 10989225, 27 => 21978450, 28 => 31078125
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

  # x^2 + 9*y^2 = k となる正整数の組の個数
  def count_pairs(k)
    cnt = 0
    y = 1
    while 9 * y * y < k
      r = k - 9 * y * y
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
    while 9 * y * y <= limit
      x = 1
      while x * x + 9 * y * y <= limit
        cnt[x * x + 9 * y * y] += 1
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
    A399214.check_known
  elsif ARGV.include?('--verify')
    nums = ARGV.select { |s| s =~ /\A\d+\z/ }.map(&:to_i)
    A399214.verify(nums[0] || 5_000_000, nums[1] || 40)
  else
    nmax = (ARGV[0] || 100).to_i
    A399214.sequence(nmax).each_with_index { |v, n| puts "#{n} #{v}" }
  end
end
