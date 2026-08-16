#!/usr/bin/env ruby
# frozen_string_literal: true
#
# A328151
#   a(n) is the smallest nonnegative integer k where exactly n ordered pairs
#   of positive integers (x, y) exist such that x^2 + y^2 = k.
#
# ------------------------------------------------------------------
# 理論（判別式 -4, Z[i] は最大整環かつ PID なので表現数は完全に乗法的）
# ------------------------------------------------------------------
#   k = 2^a * Π p_i^e_i * Π q_j^f_j        (p_i ≡ 1 mod 4,  q_j ≡ 3 mod 4)
#
#   符号込み整数解の総数は
#       r(k) = 4 * Π(e_i + 1)   ... f_j が全て偶数のとき
#            = 0                ... それ以外
#   x, y > 0 の順序付きの組の個数は
#       P(k) = M(k) - [k は平方数],      M(k) = Π(e_i + 1)
#
#   2^a と q_j^(偶数) は M を変えず k を増やすだけ。ただし「平方かどうか」
#   の調整には使える（2 倍が最安）。よって P(k) = n となる最小の k は
#   次の 2 通りを比べるだけでよい:
#
#     (A) k は非平方,  M = n
#           n が偶数 → minprod(n)      （M 偶 ⇒ ある e_i が奇 ⇒ 自動的に非平方）
#           n が奇数 → 2 * minprod(n)  （M 奇 ⇒ 全 e_i 偶 ⇒ 必ず平方なので 2 倍）
#     (B) k は平方,    M = n + 1        （n が偶数のときのみ成立）
#           → minprod(n + 1)           （M 奇 ⇒ 全 e_i 偶 ⇒ 自動的に平方）
#
#   minprod(M) := 4k+1 型素数のみを使って Π(e_i + 1) = M となる最小の数
#                 （「約数が M 個の最小数」を素数 5, 13, 17, 29, ... で行う版）
#
#   a(0) = 0 は例外扱い（k = 0 に正の組は存在しない）。
# ------------------------------------------------------------------
#
# 使い方:
#   ruby 328151_01.rb           # n = 0..100 を出力
#   ruby 328151_01.rb 30        # n = 0..30 を出力
#   ruby 328151_01.rb --verify  # 総当たりと突き合わせて理論式を検証
#   ruby 328151_01.rb --known   # OEIS 既知項 a(0)..a(28) と照合

module A328151
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

  # ---------- minprod ----------

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
    divisors_from(m, 2).each do |d|
      e = d - 1
      next if e > max_exp

      rest = minprod(m / d, primes, idx + 1, e, memo)
      next if rest.nil?

      cand = primes[idx]**e * rest
      best = cand if best.nil? || cand < best
    end
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
    # (A) k は非平方, M = n
    base = minprod(n)
    cands << (n.odd? ? 2 * base : base) if base
    # (B) k は平方, M = n + 1  （n が偶数のときのみ）
    if n.even?
      sq = minprod(n + 1)
      cands << sq if sq
    end
    cands.min
  end

  def sequence(nmax)
    (0..nmax).map { |n| a(n) }
  end

  # OEIS に登録済みの a(0)..a(28)（照合用）
  KNOWN = [
    0, 2, 5, 50, 65, 1250, 325, 31250, 1105, 8450, 8125, 19531250, 5525,
    488281250, 105625, 211250, 27625, 305175781250, 71825, 7629394531250,
    138125, 5281250, 126953125, 4768371582031250, 160225, 35701250,
    1221025, 2442050, 3453125
  ].freeze

  def check_known
    ok = true
    KNOWN.each_with_index do |want, n|
      got = a(n)
      good = (got == want)
      ok &&= good
      puts format('%-4s n=%-3d got=%-22d oeis=%d', good ? 'OK' : 'NG', n, got, want)
    end
    puts ok ? "\nmatches OEIS a(0)..a(#{KNOWN.size - 1})." : "\nMISMATCH."
    ok
  end

  # ---------- 検証用の総当たり ----------

  # x^2 + y^2 = k となる正整数の順序付きの組の個数
  def count_pairs(k)
    return 0 if k < 2

    cnt = 0
    x = 1
    while x * x < k
      r = k - x * x
      s = Integer.sqrt(r)
      cnt += 1 if s * s == r && s.positive?
      x += 1
    end
    cnt
  end

  # k <= limit を総当たりして first[n] を作り、理論値と突き合わせる。
  # 理論値が limit を超える n は「その範囲に現れないこと」だけを確認する。
  def verify(limit = 5_000_000, nmax = 40)
    warn "brute force scan up to k = #{limit} ..."
    cnt = Array.new(limit + 1, 0)
    x = 1
    while x * x <= limit
      y = 1
      while x * x + y * y <= limit
        cnt[x * x + y * y] += 1
        y += 1
      end
      x += 1
    end

    first = {}
    cnt.each_with_index { |c, k| first[c] ||= k }
    first[0] = 0 # k = 0 に正の組はない

    ok = true
    (0..nmax).each do |n|
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
    end
    puts ok ? "\nall matched." : "\nMISMATCH FOUND."
    ok
  end
end

if __FILE__ == $PROGRAM_NAME
  if ARGV.include?('--known')
    A328151.check_known
  elsif ARGV.include?('--verify')
    nums = ARGV.select{|s| s =~ /\A\d+\z/}.map(&:to_i)
    A328151.verify(nums[0] || 5_000_000, nums[1] || 40)
  else
    nmax = (ARGV[0] || 100).to_i
    A328151.sequence(nmax).each_with_index{|v, n|
      puts "#{n} #{v}"
    }
  end
end
