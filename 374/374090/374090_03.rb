#!/usr/bin/env ruby
# frozen_string_literal: true
#
# A374090
#   a(n) is the smallest nonnegative integer k where exactly n ordered pairs
#   of positive integers (x, y) exist such that x^2 + x*y + y^2 = k.
#
# ------------------------------------------------------------------
# 理論（判別式 -3。基本判別式で Z[omega] は最大整環、類数 1）
# ------------------------------------------------------------------
#   x^2 + xy + y^2 は Eisenstein 整数のノルム形式。判別式 -3 は基本判別式で
#   Z[omega] (omega = (-1+sqrt(-3))/2) は最大整環なので、A374158 (x^2+3y^2、
#   同じ体の指数 2 の整環) のような 2 進の補正因子 C(v) は一切現れない。
#   ただし単数群の位数が 6 である点が x^2+y^2 (単数 4 個) と異なる。
#
#   k = 3^t * Π p_i^{e_i} * Π q_j^{f_j}
#       p_i ≡ 1 (mod 3) : 分解  7, 13, 19, 31, 37, 43, 61, 67, ...
#       q_j ≡ 2 (mod 3) : 惰性（2 もここに入る）
#       3 : 分岐
#
#   M(k) := Π(e_i + 1)   ... f_j が全て偶数のとき
#         := 0           ... それ以外
#   （3 の指数 t は M に効かない）
#
#   符号込み整数解の総数は 6*M(k)。単数群（位数 6）は平面を 60 度ずつ 6 つの
#   扇形に分けるので、各軌道からちょうど 1 点が x, y > 0 の扇形に入る。例外は
#   境界の半直線 (x=0, y=0, x+y=0) に乗る軌道で、これは k が平方数のときに
#   ちょうど 1 個存在する。したがって
#
#       P(k) = M(k) - [k は平方数]
#
#   （k <= 200000 で総当たりと完全一致）
#
#   これは A328151（x^2+y^2 の順序付き正整数解）とまったく同じ形である。
#
# ------------------------------------------------------------------
# 最小化
# ------------------------------------------------------------------
#   3^t や q^(偶数) は M を変えず k を大きくするだけだが、平方性の調整には
#   使える。P(k) = n となる最小の k は次の 2 通りを比べればよい。
#
#     (A) k は非平方, M = n
#         n 偶 -> minprod(n)   （M 偶 ⇒ ある e_i が奇 ⇒ 自動的に非平方）
#         n 奇 -> M 奇 ⇒ 全 e_i 偶 ⇒ minprod(n) は必ず平方数なので
#                 非平方化が要る。ここで 2 は使えない（2 は惰性なので
#                 奇数乗にすると M = 0 になり解が消える）。惰性素数の
#                 偶数乗では平方性が壊れない。よって唯一の手段は分岐素数 3 を
#                 奇数乗で足すことで、最安は 3 倍。-> 3 * minprod(n)
#     (B) k は平方, M = n+1  （n が偶数のときのみ可能）
#         -> minprod(n+1)   （M 奇 ⇒ 全 e_i 偶 ⇒ 自動的に平方）
#
#   まとめると
#       a(n) = min( minprod(n), minprod(n+1) )   (n 偶, n >= 2)
#       a(n) = 3 * minprod(n)                    (n 奇)
#       a(0) = 0
#
#   minprod(M) := 3k+1 型素数 7, 13, 19, 31, ... のみを使って
#                 Π(e_i + 1) = M となる最小の数
#
#   A328151 との対応: 「p ≡ 1 mod 4」→「p ≡ 1 mod 3」、
#   「n 奇のとき 2 倍」→「3 倍」（分岐素数が 2 から 3 に変わるため）。
#
# ------------------------------------------------------------------
# 系
#   7 | a(n)、ただし a(1) = 3 のみ例外（n >= 2 では両候補とも M >= 2 で
#     最小の分解素数 7 を必ず含む）。
#   3 | a(n) <=> n が奇数。そのとき a(n) = 3*m^2（m^2 = minprod(n)）。
# ------------------------------------------------------------------
#
# 使い方:
#   ruby 374090_03.rb           # n = 0..100 を出力
#   ruby 374090_03.rb 30        # n = 0..30 を出力
#   ruby 374090_03.rb --known   # 総当たりで確定済みの項と照合
#   ruby 374090_03.rb --verify  # 総当たりと突き合わせて検証

module A374090
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

  # Π(e_i + 1) = m となる最小の Π primes[i]^{e_i}
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
    return 0 if n.zero?

    if n.odd?
      # (A) M = n（奇）は必ず平方数になるので、分岐素数 3 を 1 個足して非平方化
      w = minprod(n)
      w && 3 * w
    else
      # (A) 非平方 M = n  /  (B) 平方 M = n+1
      cands = []
      [n, n + 1].each { |m|
        v = minprod(m)
        cands << v if v
      }
      cands.min
    end
  end

  def sequence(nmax)
    (0..nmax).map { |n| a(n) }
  end

  # k <= 6*10^7 の総当たり走査で確定した項
  KNOWN = {
    0 => 0, 1 => 3, 2 => 7, 3 => 147, 4 => 91, 5 => 7203, 6 => 637,
    7 => 352947, 8 => 1729, 9 => 24843, 10 => 31213, 12 => 12103,
    14 => 405769, 15 => 1217307, 16 => 53599, 18 => 157339,
    20 => 593047, 21 => 59648043, 24 => 375193, 26 => 2989441,
    27 => 8968323, 28 => 29059303
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

  # x^2 + x*y + y^2 = k となる正整数の順序対の個数
  def count_pairs(k)
    cnt = 0
    x = 1
    while x * x + x + 1 <= k
      # y^2 + x*y + (x^2 - k) = 0 を y について解く
      disc = x * x - 4 * (x * x - k)
      if disc >= 0
        s = Integer.sqrt(disc)
        if s * s == disc
          y = (-x + s)
          cnt += 1 if y.positive? && y.even?
        end
      end
      x += 1
    end
    cnt
  end

  def verify(limit = 5_000_000, nmax = 40)
    warn "brute force scan up to k = #{limit} ..."
    cnt = Array.new(limit + 1, 0)
    x = 1
    while x * x + x + 1 <= limit
      y = 1
      while x * x + x * y + y * y <= limit
        cnt[x * x + x * y + y * y] += 1
        y += 1
      end
      x += 1
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
    A374090.check_known
  elsif ARGV.include?('--verify')
    nums = ARGV.select { |s| s =~ /\A\d+\z/ }.map(&:to_i)
    A374090.verify(nums[0] || 5_000_000, nums[1] || 40)
  else
    nmax = (ARGV[0] || 100).to_i
    A374090.sequence(nmax).each_with_index { |v, n| puts "#{n} #{v}" }
  end
end
