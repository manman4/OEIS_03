#!/usr/bin/env ruby
# frozen_string_literal: true
#
# A374158
#   a(n) is the smallest nonnegative integer k where exactly n pairs of
#   positive integers (x, y) exist such that x^2 + 3*y^2 = k.
#
# ------------------------------------------------------------------
# 理論（判別式 -12 は非基本判別式。Z[sqrt(-3)] は Z[omega] の指数 2 の
#       非最大整環なので、表現数は k mod 4 に依存する補正を受ける）
# ------------------------------------------------------------------
#   k = 2^a * 3^c * Π p_i^e_i * Π q_j^f_j    (p_i ≡ 1 mod 3, q_j ≡ 2 mod 3)
#
#   M(k) := Π(e_i + 1)   ... f_j が全て偶数のとき
#         := 0           ... それ以外
#   （2 と 3 の指数は M に効かない）
#
#   符号込み整数解の総数は、r_{-3}(k) = 6*(d_{1,3}(k) - d_{2,3}(k)) = 6*M として
#       R(k) = 6*M   ... 4 | k
#            = 2*M   ... k が奇数        <-- ここの 1/3 倍が「補正」
#            = 0     ... k ≡ 2 (mod 4)
#   R は乗法的でない。例: R(4)*R(7) = 6*4 = 24 だが R(28) = 12。
#
#   退化解は y=0 (k が平方) と x=0 (k/3 が平方) の 2 種で、それぞれ符号込み 2 個。
#   eps := [k は平方] + [k/3 は平方] ∈ {0, 1} とおくと x, y > 0 の組の個数は
#       P(k) = (R(k) - 2*eps) / 4
#   すなわち
#       4 | k     -> P = (3M - eps) / 2
#       k が奇数  -> P = (M - eps) / 2
#
#   eps は M のパリティで自動的に決まる:
#     M が奇 <=> 全 e_i が偶 <=> p 部分が平方
#       k 奇  : k = 3^c * (平方) なので c の偶奇で必ず eps = 1
#       4 | k : a が偶なら eps = 1、a が奇なら eps = 0
#     M が偶 <=> ある e_i が奇 => k も k/3 も平方でない => eps = 0
#
#   よって P(k) = n となる最小の k の候補は次の 4 通りだけ:
#     (B) k が奇数:  P = floor(M / 2)  なので  M ∈ {2n, 2n+1}
#           -> minprod(M)
#     (A) 4 | k   :  3M = 2n + eps, eps = M mod 2  なので  3 | (2n + eps)
#           -> 4 * minprod((2n + eps) / 3)      （a = 2 が最安）
#
#   minprod(M) := 3k+1 型素数 7, 13, 19, 31, ... のみを使って
#                 Π(e_i + 1) = M となる最小の数
#
#   n ≡ 2 (mod 3) のとき 2n も 2n+1 も 3 で割れず、分岐 (A) が使えない。
#   3 倍お得な 4|k 側から締め出されるので a(n) が跳ね上がる
#   （a(11) = 7^10 * 13、a(23) = 7^22 * 13 など）。
#
# 使い方:
#   ruby 374158_01.rb           # n = 0..100 を出力
#   ruby 374158_01.rb 30        # n = 0..30 を出力
#   ruby 374158_01.rb --known   # OEIS 既知項 a(0)..a(22) と照合
#   ruby 374158_01.rb --verify  # 総当たりと突き合わせて理論式を検証

module A374158
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

  # Π(e_i + 1) = m となる最小の Π primes[i]^e_i
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

    cands = []
    [2 * n, 2 * n + 1].each { |m|
      # (B) k が奇数: M = 2n または 2n+1
      v = minprod(m)
      cands << v if v

      # (A) 4 | k: 3M = 2n + eps = m のとき
      next unless (m % 3).zero?

      w = minprod(m / 3)
      cands << 4 * w if w
    }
    cands.min
  end

  def sequence(nmax)
    (0..nmax).map { |n| a(n) }
  end

  # OEIS に登録済みの a(0)..a(22)（照合用）
  KNOWN = [
    0, 4, 91, 28, 196, 31213, 364, 9604, 53599, 2548, 470596, 3672178237,
    6916, 33124, 29059303, 124852, 1983163, 3360173089, 48412, 55365148804,
    18384457, 6117748, 1623076
  ].freeze

  def check_known
    ok = true
    KNOWN.each_with_index { |want, n|
      got = a(n)
      good = (got == want)
      ok &&= good
      puts format('%-4s n=%-3d got=%-22d oeis=%d', good ? 'OK' : 'NG', n, got, want)
    }
    puts ok ? "\nmatches OEIS a(0)..a(#{KNOWN.size - 1})." : "\nMISMATCH."
    ok
  end

  # ---------- 検証用の総当たり ----------

  # x^2 + 3*y^2 = k となる正整数の組の個数
  def count_pairs(k)
    cnt = 0
    y = 1
    while 3 * y * y < k
      r = k - 3 * y * y
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
    while 3 * y * y <= limit
      x = 1
      while x * x + 3 * y * y <= limit
        cnt[x * x + 3 * y * y] += 1
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
    A374158.check_known
  elsif ARGV.include?('--verify')
    nums = ARGV.select { |s| s =~ /\A\d+\z/ }.map(&:to_i)
    A374158.verify(nums[0] || 5_000_000, nums[1] || 40)
  else
    nmax = (ARGV[0] || 100).to_i
    A374158.sequence(nmax).each_with_index { |v, n| puts "#{n} #{v}" }
  end
end
