#!/usr/bin/env ruby
# frozen_string_literal: true
#
# A399191
#   a(n) is the smallest nonnegative integer k where exactly n pairs of
#   positive integers (x, y) exist such that x^2 + 4*y^2 = k.
#
# ------------------------------------------------------------------
# 理論（判別式 -16 = 4*(-4)。Z[2i] は Z[i] の指数 2 の非最大整環。
#       今回は 2 が Q(i) で「分岐」する場合）
# ------------------------------------------------------------------
#   O = Z[2i] = Z + 2*Z[i],  O_K = Z[i],  h(-4) = 1,  O_K^* = {±1, ±i}
#   2*O_K = p^2  (p = (1+i))、O_K/2*O_K は F_2[t]/(t^2) 型の局所環で
#   Z の像は {0, 1}。したがって alpha = a+bi について
#       alpha ∈ O  <=>  b が偶  <=>  alpha mod 2*O_K ∈ {0, 1}
#
#   ノルム k = 2^v * u (u 奇) のイデアルは p^v * J（N(J) = u）と一意に書け、
#   その個数は M(u) := Π(e_i + 1)（p_i ≡ 1 mod 4、q_j ≡ 3 mod 4 は偶指数、
#   でなければ 0）。生成元は alpha, i*alpha, -alpha, -i*alpha の 4 個で、
#   -alpha ≡ alpha (mod 2) だからクラスは [alpha] と [i*alpha] の 2 つ。
#     v >= 2: alpha ∈ 2*O_K なので両方 0 -> 4 個すべて O に入る
#     v = 0 : [alpha] ∈ {1, i}、[i*alpha] = i*[alpha] なのでちょうど一方が 1
#             -> 2 個
#     v = 1 : [alpha] = [i*alpha] = 1+i で {0,1} に入らない -> 0 個
#   よって c(v) := 2 (v=0), 0 (v=1), 4 (v>=2) として
#       R(k) = c(v) * M(u)
#   （k <= 20000 で総当たりと完全一致）
#
#   退化解は y=0 (k が平方) と x=0 (k/4 が平方)。ここで k/4 が平方なら
#   k = (2s)^2 も平方なので、この 2 条件は独立でなく
#       eps := [k は平方] + [k/4 は平方]
#            = 0 (k は非平方) / 1 (k は奇数の平方) / 2 (k は偶数の平方)
#   R(k) = 4*P(k) + 2*eps より
#       v = 0      -> P = (2M - 2*eps)/4 = (M - eps)/2、eps = M mod 2 なので
#                     P = floor(M / 2)
#       v = 1      -> P = 0
#       v >= 2     -> P = (4M - 2*eps)/4 = M - eps/2、eps ∈ {0, 2} で
#                     eps = 2 <=> v が偶 かつ M が奇 なので
#                     P = M - [v が偶 かつ M が奇]
#
#   したがって P(k) = n (n >= 1) となる最小の k の候補は
#     (B) v = 0        : minprod(2n), minprod(2n+1)
#     (A1) v 奇 (最安 v=3): P = M なので 8 * minprod(n)
#     (A2) v 偶 (最安 v=2): P = M - [M 奇] なので
#                          M = n   (n が偶) -> 4 * minprod(n)
#                          M = n+1 (n が偶、n+1 は奇) -> 4 * minprod(n+1)
#   の高々 4 通り。n が奇数のとき (A2) は P が必ず偶数になるので使えず、
#   n が偶数のとき (A1) は (A2) に必ず負ける（8 > 4）。まとめると
#       n 奇: 3 候補   n 偶: 4 候補
#
#   minprod(M) := 4k+1 型素数 5, 13, 17, 29, ... のみを使って
#                 Π(e_i + 1) = M となる最小の数（= A018782(M)）
#
#   a(0) = 0（k = 0 に正の組はない）。
#
# 使い方:
#   ruby 399191_02.rb           # n = 0..100 を出力
#   ruby 399191_02.rb 30        # n = 0..30 を出力
#   ruby 399191_02.rb --verify  # 総当たりと突き合わせて理論式を検証

module A399191
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
    return 0 if n.zero?

    cands = []
    # (B) v = 0 : P = floor(M / 2)
    [2 * n, 2 * n + 1].each { |m|
      v = minprod(m)
      cands << v if v
    }
    if n.odd?
      # (A1) v 奇（最安 v = 3）: P = M
      w = minprod(n)
      cands << 8 * w if w
    else
      # (A2) v 偶（最安 v = 2）: P = M - [M 奇]
      [n, n + 1].each { |m|
        w = minprod(m)
        cands << 4 * w if w
      }
    end
    cands.min
  end

  def sequence(nmax)
    (0..nmax).map { |n| a(n) }
  end

  # ---------- 検証用の総当たり ----------

  # x^2 + 4*y^2 = k となる正整数の組の個数
  def count_pairs(k)
    cnt = 0
    y = 1
    while 4 * y * y < k
      r = k - 4 * y * y
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
    while 4 * y * y <= limit
      x = 1
      while x * x + 4 * y * y <= limit
        cnt[x * x + 4 * y * y] += 1
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
  if ARGV.include?('--verify')
    nums = ARGV.select { |s| s =~ /\A\d+\z/ }.map(&:to_i)
    A399191.verify(nums[0] || 5_000_000, nums[1] || 40)
  else
    nmax = (ARGV[0] || 100).to_i
    A399191.sequence(nmax).each_with_index { |v, n| puts "#{n} #{v}" }
  end
end
