#!/usr/bin/env ruby
# frozen_string_literal: true
#
# A374094
#   a(n) is the smallest nonnegative integer k where there are exactly n
#   solutions to x^2 + x*y + y^2 = k with 0 < x < y.
#
# ------------------------------------------------------------------
# 理論（判別式 -3。基本判別式で Z[omega] は最大整環、類数 1）
# ------------------------------------------------------------------
#   k = 3^t * Π p_i^{e_i} * Π q_j^{f_j}
#       p_i ≡ 1 (mod 3) : 分解  7, 13, 19, 31, 37, 43, 61, 67, ...
#       q_j ≡ 2 (mod 3) : 惰性（2 もここに入る）
#       3 : 分岐
#
#   M(k) := Π(e_i + 1)   ... f_j が全て偶数のとき
#         := 0           ... それ以外
#   （3 の指数 t は M に効かない）
#
#   符号込み整数解は 6*M(k) 個。単数群（位数 6）は平面を 60 度ずつ 6 つの
#   扇形に分け、各軌道からちょうど 1 点が x, y > 0 に入る。ただし境界
#   (x=0, y=0, x+y=0) に乗る軌道は k が平方数のときにちょうど 1 個あって
#   寄与しない。よって順序付き正整数解の個数は（A374090 の数え方）
#       P(k) = M(k) - [k は平方数]
#
#   x = y の解は 3x^2 = k、すなわち k が 3 倍の平方数のときちょうど 1 個。
#   P(k) は x < y と x > y を別々に数えるので
#       P(k) = 2*U(k) + [k は 3 倍の平方数]
#   ここで U(k) は 0 < x < y の解の個数。したがって
#       U(k) = (M(k) - eps(k)) / 2,   eps(k) := [k は平方数] + [k は 3 倍の平方数]
#
#   k = u^2 = 3v^2 は不可能なので eps ∈ {0, 1}。さらに eps は M のパリティで
#   自動的に決まる:
#       M が奇 <=> 全 e_i が偶 <=> k = 3^t * (平方数)
#                 => t が偶なら k は平方数、t が奇なら k は 3 倍の平方数
#                 => いずれにせよ eps = 1
#       M が偶 <=> ある e_i が奇 => k は平方数でも 3 倍平方数でもない => eps = 0
#   よって
#       U(k) = floor( M(k) / 2 )
#   （k <= 200000 で総当たりと完全一致）
#
# ------------------------------------------------------------------
# 最小化
# ------------------------------------------------------------------
#   U(k) = n (n >= 1) となるのは M(k) ∈ {2n, 2n+1} のときだけ。3^t や
#   q^(偶数) は M を変えず k を大きくするだけなので、分解素数だけを使えばよい。
#   したがって候補はわずか 2 個:
#
#       a(n) = min( minprod(2n), minprod(2n+1) )    (n >= 1)
#       a(0) = 0
#
#   minprod(M) := 3k+1 型素数 7, 13, 19, 31, ... のみを使って
#                 Π(e_i + 1) = M となる最小の数
#
#   姉妹列との関係:
#     A374090（順序付き、P(k) = M - [k=□]）は n の偶奇で候補が変わるが、
#     こちらは平方性の補正がパリティに吸収されるので分岐がない。
#     形としては A200977（x^2+2y^2）と同型で、素数の集合だけが違う。
#
# ------------------------------------------------------------------
# 系
#   すべての項が 7 の倍数（a(0) = 0 も含む）。n >= 1 では 2n >= 2 かつ
#   2n+1 >= 3 なので、どちらの候補も最小の分解素数 7 を必ず含む。
# ------------------------------------------------------------------
#
# 使い方:
#   ruby 374094_02.rb           # a(n) が 1000 桁を超える手前まで出力
#   ruby 374094_02.rb 30        # n = 0..30 を出力（桁数制限も併用）
#   ruby 374094_02.rb --digits=200   # 桁数の上限を変更
#   ruby 374094_02.rb --known   # 総当たりで確定済みの項と照合
#   ruby 374094_02.rb --verify  # 総当たりと突き合わせて検証

module A374094
  module_function

  # 出力する a(n) の桁数上限。ちょうどこの桁数までは出力し、
  # 超えた時点で打ち切る（1000 桁は OK、1001 桁で停止）。
  MAX_DIGITS = 1000

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

    cands = []
    [2 * n, 2 * n + 1].each { |m|
      v = minprod(m)
      cands << v if v
    }
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
    0 => 0, 1 => 7, 2 => 91, 3 => 637, 4 => 1729, 5 => 31213, 6 => 12103,
    7 => 405769, 8 => 53599, 9 => 157339, 10 => 593047, 12 => 375193,
    13 => 2989441, 14 => 29059303, 15 => 7709611, 16 => 1983163,
    18 => 4877509, 20 => 18384457, 24 => 13882141
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

  # x^2 + x*y + y^2 = k を満たす 0 < x < y の個数
  def count_pairs(k)
    cnt = 0
    x = 1
    while 3 * x * x < k
      disc = x * x - 4 * (x * x - k)
      if disc >= 0
        s = Integer.sqrt(disc)
        if s * s == disc
          num = -x + s
          cnt += 1 if num.positive? && num.even? && num / 2 > x
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
    while 3 * x * x <= limit
      y = x + 1
      while x * x + x * y + y * y <= limit
        cnt[x * x + x * y + y * y] += 1
        y += 1
      end
      x += 1
    end

    first = {}
    cnt.each_with_index { |c, k| first[c] ||= k }
    first[0] = 0 # k = 0 に解はない

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
    A374094.check_known
  elsif ARGV.include?('--verify')
    nums = ARGV.select { |s| s =~ /\A\d+\z/ }.map(&:to_i)
    A374094.verify(nums[0] || 5_000_000, nums[1] || 40)
  else
    dig = ARGV.find { |t| t =~ /\A--digits=(\d+)\z/ } ? Regexp.last_match(1).to_i
                                                      : A374094::MAX_DIGITS
    cap = ARGV.find { |t| t =~ /\A\d+\z/ }&.to_i
    rows = A374094.sequence(cap, dig)
    rows.each { |n, v| puts "#{n} #{v}" }
    last = rows.last
    if last && (cap.nil? || last[0] < cap)
      nxt = last[0] + 1
      warn "stopped: a(#{nxt}) has #{A374094.a(nxt).to_s.size} digits (> #{dig})"
    end
  end
end
