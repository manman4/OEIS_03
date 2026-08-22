#!/usr/bin/env ruby
# frozen_string_literal: true
#
# A399202
#   a(n) is the smallest nonnegative integer k where there are exactly n
#   nonnegative integer solutions to x^2 + y^2 = k.
#   （A328151 の非負・順序付き版）
#
# ------------------------------------------------------------------
# 理論（判別式 -4 は基本判別式。Z[i] は最大整環かつ PID なので
#       2 進の補正因子は現れない）
# ------------------------------------------------------------------
#   k = 2^a * Π p_i^e_i * Π q_j^f_j    (p_i ≡ 1 mod 4,  q_j ≡ 3 mod 4)
#
#   M(k) := Π(e_i + 1)   ... f_j が全て偶数のとき
#         := 0           ... それ以外
#   （2 の指数 a は M に効かない。2 は分岐する）
#
#   Jacobi の古典的公式より符号込み整数解の総数は
#       r_2(k) = 4 * (d_1(k) - d_3(k)) = 4 * M(k)
#
#   退化解は x=0 と y=0 の 2 種で、どちらも k が平方数のときに現れる。
#   符号込みでは (0, ±sqrt k) と (±sqrt k, 0) の計 4 個。よって
#       r_2(k) = 4*P(k) + 4*[k は平方]      (P は x, y > 0 の順序付きの組)
#       P(k)   = M(k) - [k は平方]
#   非負解は P に軸上の 2 点 (0, sqrt k), (sqrt k, 0) を足したものなので
#       N(k) = P(k) + 2*[k は平方] = M(k) + [k は平方]
#
#   ここが x^2+2y^2 などと違う点: [k は平方] は M のパリティだけでは決まらず、
#   2 の指数 a の偶奇にも依存する。
#       M が奇 <=> 全 e_i が偶 <=> k = 2^a * (平方数)
#       このとき k が平方 <=> a が偶
#       M が偶 <=> ある e_i が奇 => k は平方でない
#   まとめると
#       N(k) = M(k) + [a が偶 かつ M(k) が奇]
#   （k <= 20 万で総当たりと完全一致）
#
#   したがって N(k) = n となる最小の k は次のように決まる。
#     n が奇数: N が奇 <=> M = n が奇 かつ a が奇。a の最安は 1 なので
#               a(n) = 2 * minprod(n)
#     n が偶数: M = n（偶、a は 0 でよい。M 偶なので自動的に非平方）または
#               M = n-1（奇、a = 0 なら k は平方で N = M+1 = n）。よって
#               a(n) = min( minprod(n), minprod(n-1) )
#
#   minprod(M) := 4k+1 型素数 5, 13, 17, 29, ... のみを使って
#                 Π(e_i + 1) = M となる最小の数（= A018782(M)）
#
#   例外:
#     a(0) = 3  ... M = 0 になる最小の k。3 ≡ 3 (mod 4) が 1 乗で現れる。
#     a(1) = 0  ... k = 0 は (0, 0) の 1 解を持ち、これが最小。
#                   （公式側は 2*minprod(1) = 2 を返すが 0 のほうが小さい）
#
# 使い方:
#   ruby 399202_01.rb           # n = 0..100 を出力
#   ruby 399202_01.rb 30        # n = 0..30 を出力
#   ruby 399202_01.rb --known   # 総当たりで確定済みの項と照合
#   ruby 399202_01.rb --verify  # 総当たりと突き合わせて理論式を検証

module A399202
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
    return 3 if n.zero?  # M = 0 になる最小の k
    return 0 if n == 1   # k = 0 -> (0, 0)

    if n.odd?
      # M = n（奇）, a は奇数。最安は a = 1
      w = minprod(n)
      w && 2 * w
    else
      # M = n（偶, a = 0） または M = n-1（奇, a = 0 で k は平方）
      cands = []
      [n, n - 1].each { |m|
        v = minprod(m)
        cands << v if v
      }
      cands.min
    end
  end

  def sequence(nmax)
    (0..nmax).map { |n| a(n) }
  end

  # k <= 3*10^6 の総当たり走査で確定した項（n => a(n)）
  KNOWN = {
    0 => 3, 1 => 0, 2 => 1, 3 => 50, 4 => 25, 5 => 1250, 6 => 325,
    7 => 31250, 8 => 1105, 9 => 8450, 10 => 4225, 12 => 5525,
    14 => 203125, 15 => 211250, 16 => 27625, 18 => 71825
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

  # x^2 + y^2 = k となる非負整数解（順序付き）の個数
  def count_solutions(k)
    return 1 if k.zero?

    cnt = 0
    y = 0
    while y * y <= k
      r = k - y * y
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
    while y * y <= limit
      x = 0
      while x * x + y * y <= limit
        cnt[x * x + y * y] += 1
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
    A399202.check_known
  elsif ARGV.include?('--verify')
    nums = ARGV.select { |s| s =~ /\A\d+\z/ }.map(&:to_i)
    A399202.verify(nums[0] || 5_000_000, nums[1] || 40)
  else
    nmax = (ARGV[0] || 100).to_i
    A399202.sequence(nmax).each_with_index { |v, n| puts "#{n} #{v}" }
  end
end
